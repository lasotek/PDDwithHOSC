#include "helpers/helpers.h"
#include "FullHOSC.h"
#include "../Hash/hash_combine.h"
#include <memory>
#include <optional>
#include <cassert>
#include <exception>
#include <unordered_map>
#include <map>
#include <concepts>
#include <algorithm>
#include <numeric>
#include <optional>
#include <sstream>

namespace HOSC
{
    int HOSC::insert_del_to_vector(int source, int target, deletion_type &vector)
    {
        auto pos_to_ins = [&vector, source]() {
            int pos = vector.size();
            while (pos > 0)
                if (vector.at(pos - 1).source() < source)
                    break;
                else
                    pos--;
            return pos;
        };
        auto place_to_insert = pos_to_ins();
        auto new_it = vector.emplace(vector.begin() + place_to_insert, SingleDel(source, target));
        std::for_each(++new_it, vector.end(), [source, target](auto &Del) {
            if (std::abs(Del.target_) == source)
            {
                Del.target_ = sgn(Del.target_) * target;
            }
        });
        return place_to_insert;
    }

    void HOSC::reserve(int node)
    {
        if (node > n_nodes_)
        {
            row_deletions_.reserve(node);
            col_deletions_.reserve(node);
            n_nodes_ = node;
        }
    }

    // HOSC& HOSC::operator=(const HOSC& Source)
    // {
    //     n_nodes_ = Source.n_nodes_;
    // }

    [[nodiscard]] int HOSC::clean_sgn()
    {
        if (is_complete())
        {
            int res = sgn_;
            sgn_ = 1;
            return res;
        }
        else
        {
            int res = sgn_ / sgn(sgn_);
            sgn_ = sgn(sgn_);
            return res;
        }
    }

    template <typename MAP>
    void HOSC::copy_to_replacer(Node_replacer_temp<MAP> &Target, del_set which_one)
    {
        Target.clear();
        auto fill_in_replacer = [this, &Target](deletion_type &dels) {
            for (const auto &del : dels)
            {
                Target.set(del.source(), del.target());
            }
        };
        switch (which_one)
        {
        case rows:
            fill_in_replacer(row_deletions_);
            break;
        case columns:
        default:
            fill_in_replacer(col_deletions_);
            break;
        }
    }

    template <typename MAP>
    void HOSC::copy_from_replacer(const Node_replacer_temp<MAP> &Source, del_set which_one)
    {
        auto recreate_dels = [this, &Source](deletion_type &dels) {
            dels.clear();
            dels.reserve(n_nodes_);
            for (auto s = 0; s < n_nodes_; s++)
            {
                auto t = Source.get(s);
                if (t == s)
                    continue;
                dels.emplace_back(SingleDel(s,t));
            }
        };
        switch (which_one)
        {
        case rows:
            recreate_dels(row_deletions_);
            break;
        case columns:
        default:
            recreate_dels(col_deletions_);
            break;
        }
    }
    
    void HOSC::Test() 
    {
        Node_linker linker_r, linker_c;
        copy_to_replacer(linker_r,rows);
        copy_to_replacer(linker_c,columns);
    }
    /**
     * @brief 
     * 
     * @param source 
     * @param target 
     * @return int 
     */
    int clear_cannonical(int &source, int &target)
    {
        if (source == target)
            return 0;
        int sgn = 1;
        if (std::abs(source) < std::abs(target))
        {
            std::swap(source, target);
            sgn *= -1;
        }
        if (source < 0)
        {
            source *= -1;
            target *= -1;
            sgn *= -1;
        }
        if (source == -target)
        {
            target = 0;
            sgn *= 2;
        }
        return sgn;
    }

    HOSC::HOSC(int sgn, int rsource, int rtarget, int csource, int ctarget, short max_nodes)
        : HOSC(max_nodes)
    {
        sgn_ *= sgn;
        sgn_ *= clear_cannonical(rsource, rtarget);
        sgn_ *= clear_cannonical(csource, ctarget);
        row_deletions_.emplace_back(SingleDel(rsource, rtarget));
        col_deletions_.emplace_back(SingleDel(csource, ctarget));
    }

    HOSC::HOSC(int sgn, initial_del_set initial_dels, short max_nodes) : n_nodes_(max_nodes)
    {
        if (initial_dels.size() > n_nodes_)
        {
            sgn_ = 0;
            return;
        }
        auto replacer_row = Node_replacer();
        auto replacer_col = Node_replacer();
        sgn_ *= sgn;
        for (auto del : initial_dels)
        {
            auto p = replacer_row.get(del.first.source());
            auto r = replacer_row.get(del.first.target());
            sgn_ *= clear_cannonical(p, r);
            if (sgn == 0)
                return;
            auto k = replacer_col.get(del.second.source());
            auto l = replacer_col.get(del.second.target());
            sgn_ *= clear_cannonical(k, l);
            if (sgn == 0)
                return;
            auto p_pos = insert_del_to_row(p, r);
            replacer_row.set(p, r);
            int c_pos = insert_del_to_col(k, l);
            replacer_col.set(k, l);
            if (is_odd(p_pos - c_pos))
                sgn_ *= -1;
        }
    }

    std::string HOSC::String() const
    {
        std::ostringstream out;
        if (is_complete())
        {
            out << sgn_;
        }
        else
        {
            if (sgn_ == -1)
                out << "-";
            else if (sgn_ != 1)
                out << sgn_ << "*";
            out << "\\Delta";
            auto rit = row_deletions_.begin();
            auto cit = col_deletions_.begin();
            while (rit != row_deletions_.end() && cit != col_deletions_.end())
            {
                auto del_r = *rit++;
                auto del_c = *cit++;
                auto p = del_r.source();
                auto r = del_r.target();
                auto k = del_c.source();
                auto l = del_c.target();
                out << "(" << p << (r >= 0 ? "+" : "") << r << ")(" << k << (l >= 0 ? "+" : "") << l << ")"
                    << ",";
            }
        }
        auto str = out.str();
        if (str[str.length() - 1] == ',')
            str.substr(0, str.length() - 1);
        return str;
    }

    bool HOSC::operator==(const HOSC &Other) const
    {
        /*
                deletion_type row_deletions_, col_deletions_;
                short n_nodes_ = 0;
                int sgn_ = 1;
        */
        return n_nodes_ == Other.n_nodes_ && row_deletions_ == Other.row_deletions_ && col_deletions_ == Other.col_deletions_;
    }

    std::size_t HOSC::hash() const
    {
        size_t h = std::hash<short>{}(n_nodes_);
        // HashCombine(h, std::hash<int>{}(sgn_));
        std::for_each(row_deletions_.begin(), row_deletions_.end(), [&h](const SingleDel &del) {
            HashCombine(h, std::hash<SingleDel>{}(del));
        });
        std::for_each(col_deletions_.begin(), col_deletions_.end(), [&h](const SingleDel &del) {
            HashCombine(h, std::hash<SingleDel>{}(del));
        });
        return h;
    }

    [[nodiscard]] int HOSC::move_sgn()
    {
        auto aux = 1;
        std::swap(sgn_, aux);
        return aux;
    }

    std::optional<int> HOSC::numeric_value() const
    {
        if (is_complete())
            return sgn_;
        return {};
    }

    HOSC::HOSC_oper_result HOSC::HOSC_big_dot(HOSC_oper_result h2, const nodes_to_remove &nodes) const
    {
        if (!h2)
            throw std::invalid_argument("BigDot needs real argument!");
        if (n_nodes_ != h2->n_nodes_)
            throw std::logic_error("HOSC must have the same No of nodes!");
        auto &ext_row_deletions = h2->row_deletions_;
        auto &ext_col_deletions = h2->col_deletions_;
        if (row_deletions_.size() + ext_row_deletions.size() > n_nodes_)
            return {};
        if (row_deletions_.size() + ext_row_deletions.size() < nodes.size())
            return {};
        auto rem_nodes = Removing_pair_nodes(nodes);
        auto del_cleanear = [&rem_nodes](int &p, int &r, int &k, int &l) -> int /*sgn chnage*/
        {
            int lsgn = 1;
            auto cleaner = [&lsgn, &rem_nodes](bool toSwap, int &a, int &b) {
                if (toSwap)
                {
                    std::swap(a, b);
                    lsgn *= -1;
                }
                if (a < 0)
                {
                    a *= -1;
                    b *= -1;
                    lsgn *= -1;
                }
                if (a == -b)
                {
                    b = 0;
                    lsgn *= 2;
                }
            };
            auto a_r = std::abs(r), a_l = std::abs(l);
            bool swap_in_raw = false, swap_in_column = false, swap_r_done = false, swap_c_done = false;
            auto p_is_to_be_removed = rem_nodes.is_to_be_removed(p);
            auto r_is_to_be_removed = rem_nodes.is_to_be_removed(std::abs(r));
            auto k_is_to_be_removed = rem_nodes.is_to_be_removed(k);
            auto l_is_to_be_removed = rem_nodes.is_to_be_removed(std::abs(l));
            if (p_is_to_be_removed && p == k)
            {
            }
            else if (r_is_to_be_removed && a_r == a_l)
            {
                swap_in_column = swap_in_raw = true;
            }
            else if (p_is_to_be_removed && p == a_l)
            {
                swap_in_column = true;
            }
            else if (k_is_to_be_removed && k == a_r)
            {
                swap_in_raw = true;
            }
            else
            {
                if (p < a_r || r_is_to_be_removed)
                {
                    swap_in_raw = true;
                }
                if (k < a_l || l_is_to_be_removed)
                {
                    swap_in_column = true;
                }
            }
            cleaner(swap_in_raw, p, r);
            cleaner(swap_in_column, k, l);
            return lsgn;
        };
        auto replacer_row = Node_replacer();
        auto replacer_col = Node_replacer();
        auto rmit = row_deletions_.begin();
        auto rmit_e = row_deletions_.end();
        auto cmit = col_deletions_.begin();
        auto cmit_e = col_deletions_.end();
        auto rhit = ext_row_deletions.begin();
        auto rhit_e = ext_row_deletions.end();
        auto chit = ext_col_deletions.begin();
        auto chit_e = ext_col_deletions.end();
        HOSC_oper_result res = std::make_shared<HOSC>(n_nodes_);
        res->sgn_ = sgn_ * h2->sgn_;
        auto &out_row_deletion = res->row_deletions_;
        auto &out_col_deletion = res->col_deletions_;
        while (rmit != rmit_e || rhit != rhit_e)
        {
            int p, r, k, l;
            p = r = k = l = 0;
            enum
            {
                none,
                mine,
                yours
            } which = none;
            auto rsgn = 1;
            auto lsgn = 1;
            if (rmit != rmit_e)
            {
                p = replacer_row.get(rmit->source());
                r = replacer_row.get(rmit->target());
                k = replacer_col.get(cmit->source());
                l = replacer_col.get(cmit->target());
                if (p == r || k == l)
                    return {};
                rsgn *= del_cleanear(p, r, k, l);
                if (rsgn == 0)
                    return {};
                which = mine;
            }
            bool left_must_be_removed = (which != none) && (p == k) && rem_nodes.is_to_be_removed(p);
            if (!left_must_be_removed)
            {
                if (rhit != rhit_e)
                {
                    auto p1 = replacer_row.get(rhit->source());
                    auto r1 = replacer_row.get(rhit->target());
                    auto k1 = replacer_col.get(chit->source());
                    auto l1 = replacer_col.get(chit->target());
                    if (p1 == r1 || k1 == l1)
                        return {};
                    lsgn *= del_cleanear(p1, r1, k1, l1);
                    if (lsgn == 0)
                        return {};
                    if ((p1 == p && r1 == p) || (k1 == k && l1 == l))
                        return {};
                    bool replace_right = which == none;
                    if (!replace_right)
                        replace_right = (p1 == k1) && rem_nodes.is_to_be_removed(p1);
                    if (!replace_right)
                    {
                        bool right_lower_in_rows = (p1 < p) || (p1 == p && r1 < r);
                        bool right_lower_in_cols = (k1 < k) || (k1 == k && l1 < l);
                        replace_right = right_lower_in_rows && right_lower_in_cols;
                    }
                    if (replace_right)
                    {
                        p = p1;
                        r = r1;
                        k = k1;
                        l = l1;
                        rsgn = lsgn;
                        which = yours;
                    }
                }
            }
            if (which == none)
                break;
            int p_pos = 0;
            if (!rem_nodes.set_was_done_row(p))
            {
                p_pos = res->insert_del_to_row(p, r);
            }
            else
            {
                p_pos = res->row_deletions_.size();
            }
            replacer_row.set(p, r);
            int c_pos = 0;
            if (!rem_nodes.set_was_done_col(k))
            {
                c_pos = res->insert_del_to_col(k, l);
            }
            else
            {
                c_pos = res->col_deletions_.size();
            }
            replacer_col.set(k, l);
            if (is_odd(p_pos - c_pos))
                rsgn *= -1;
            res->sgn_ *= rsgn;
            switch (which)
            {
            case mine:
                rmit++;
                cmit++;
                break;
            case yours:
                rhit++;
                chit++;
                break;
            case none:;
            }
        } //while
        if (out_row_deletion.size() != out_col_deletion.size())
            return {};
        if (!rem_nodes.is_complete())
            return {};
        else
        {
            res->n_nodes_ -= nodes.size();
        }

        return std::move(res);
    }

    HOSCUniqueCollection<HOSC> HOSCCollection;
}; // namespace HOSC