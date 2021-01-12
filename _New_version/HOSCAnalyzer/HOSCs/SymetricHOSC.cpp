#include "helpers/helpers.h"
#include "SymetricHOSC.h"
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
    void SymetricalHOSC::insert_del(int source, int target)
    {
        auto pos_to_ins = [this, source]() {
            int pos = this->deletions_.size();
            while (pos > 0)
                if (this->deletions_.at(pos - 1).source() < source)
                    break;
                else
                    pos--;
            return pos;
        };
        auto place_to_insert = pos_to_ins();
        auto new_it = deletions_.emplace(deletions_.begin() + place_to_insert, SingleDel(source, target));
        std::for_each(++new_it, deletions_.end(), [source, target](auto &Del) {
            if (std::abs(Del.target_) == source)
            {
                Del.target_ = sgn(Del.target_) * target;
            }
        });
    }

    void SymetricalHOSC::reserve(int node)
    {
        if (node > n_nodes_)
        {
            deletions_.reserve(node);
            n_nodes_ = node;
        }
    }

    inline void clear_simp_canonical(int &source, int &target)
    {
        if (source < target)
        {
            std::swap(source, target);
        }
    }

    SymetricalHOSC::SymetricalHOSC(int source, int target, short max_nodes)
        : SymetricalHOSC(max_nodes)
    {
        clear_simp_canonical(source, target);
        deletions_.emplace_back(SingleDel(source, target));
    }

    SymetricalHOSC::SymetricalHOSC(initial_del_set initial_dels, short max_nodes) : n_nodes_(max_nodes)
    {
        if (initial_dels.size() > n_nodes_)
        {
            valid_ = false;
            return;
        }
        auto replacer = Node_replacer();
        for (auto del : initial_dels)
        {
            auto p = replacer.get(del.source());
            auto r = replacer.get(del.target());
            if (p == r)
            {
                valid_ = false;
                return;
            }
            clear_simp_canonical(p, r);
            insert_del(p, r);
            replacer.set(p, r);
        }
    }

    std::string SymetricalHOSC::String() const
    {
        std::ostringstream out;
        out << "\\Delta";

        for (auto rit = deletions_.begin(); rit != deletions_.end(); rit++)
        {
            auto del_r = *rit;
            auto p = del_r.source();
            auto r = del_r.target();
            out << "(" << p << "+" << r << ")"
                << ", ";
        }
        auto str = out.str();
        return str.substr(0, str.length() - 1);
    }

    std::size_t SymetricalHOSC::hash() const
    {
        size_t h = std::hash<short>{}(n_nodes_);
        std::for_each(deletions_.begin(), deletions_.end(), [&h](const SingleDel &del) {
            HashCombine(h, std::hash<SingleDel>{}(del));
        });
        return h;
    }
    
    bool SymetricalHOSC::operator==(const SymetricalHOSC &Other) const
    {
        return n_nodes_ == Other.n_nodes_ && deletions_ == Other.deletions_;
    }

    SymetricalHOSC::HOSC_oper_result SymetricalHOSC::HOSC_big_dot(HOSC_oper_result h2, const nodes_to_remove &nodes)
    {
        if (!h2)
            throw std::invalid_argument("BigDot needs real argument!");
        if (n_nodes_ != h2->n_nodes_)
            throw std::logic_error("SymetricalHOSC must have the same No of nodes!");
        auto &ext_deletions = h2->deletions_;
        if (deletions_.size() + ext_deletions.size() > n_nodes_)
            return {};
        if (deletions_.size() + ext_deletions.size() < nodes.size())
            return {};
        auto rem_nodes = Removing_pair_nodes(nodes);
        auto del_cleanear = [&rem_nodes](int &p, int &r) -> int /*sgn chnage*/
        {
            if (p == r)
                return 0;
            auto a_is_to_be_removed = rem_nodes.is_to_be_removed(p);
            auto b_is_to_be_removed = rem_nodes.is_to_be_removed(r);
            auto c_left_smaller = p < r;
            if ((b_is_to_be_removed && !c_left_smaller) || (!a_is_to_be_removed && c_left_smaller))
            {
                std::swap(p, r);
            }
            return 1;
        };
        auto replacer = Node_replacer();
        auto rmit = deletions_.begin();
        auto rmit_e = deletions_.end();
        auto rhit = ext_deletions.begin();
        auto rhit_e = ext_deletions.end();
        HOSC_oper_result res = std::make_shared<SymetricalHOSC>(n_nodes_);
        auto &out_deletion = res->deletions_;
        while (rmit != rmit_e || rhit != rhit_e)
        {
            int p, r;
            p = r = 0;
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
                p = replacer.get(rmit->source());
                r = replacer.get(rmit->target());
                rsgn = del_cleanear(p, r);
                if (rsgn == 0)
                    return {};
                which = mine;
            }
            if (rhit != rhit_e)
            {
                auto p1 = replacer.get(rhit->source());
                auto r1 = replacer.get(rhit->target());
                lsgn = del_cleanear(p1, r1);
                if (lsgn == 0)
                    return {};
                if ((p1 == p && r1 == p)) 
                    return {};
                bool replace_right = which == none;
                if (!replace_right)
                {
                    replace_right = (p1 < p) || (p1 == p && r1 < r);
                }
                if (replace_right)
                {
                    p = p1;
                    r = r1;
                    rsgn = lsgn;
                    which = yours;
                }
            }
            if (which == none)
                break;
            if (!rem_nodes.set_was_done_row(p))
            {
                res->insert_del(p, r);
            }
            replacer.set(p, r);
            switch (which)
            {
            case mine:
                rmit++;
                break;
            case yours:
                rhit++;
                break;
            case none:;
            }
        } //while
        if (!rem_nodes.is_complete())
            return {};
        return std::move(res);
    }
}; // namespace HOSC