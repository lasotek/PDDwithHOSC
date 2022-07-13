/**
 * @file SingleHOSC.cpp
 * @author Sławomir Lasota  (lasotek@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2021-02-05
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#include "helpers/helpers.h"
#include "SingleHOSC.h"
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
    void SingleHOSC::insert_del(int source, int target)
    {
        auto pos_to_ins = [source](deletion_type &vector) {
            int pos = vector.size();
            while (pos > 0)
                if (vector.at(pos - 1).source() < source)
                    break;
                else
                    pos--;
            return pos;
        };
        auto place_to_insert = pos_to_ins(_deletions_);
        auto new_it = _deletions_.emplace(_deletions_.begin() + place_to_insert, SingleDel(source, target));
        std::for_each(++new_it, _deletions_.end(), [source, target](auto &Del) {
            if (Del.target_ == source)
            {
                Del.target_ = target;
            }
        });
        return;
    }

    void SingleHOSC::reserve(int node)
    {
        if (node > n_nodes_)
        {
            _deletions_.reserve(node);
            n_nodes_ = node;
        }
    }

    // HOSC& HOSC::operator=(const HOSC& Source)
    // {
    //     n_nodes_ = Source.n_nodes_;
    // }

    template <typename MAP>
    void SingleHOSC::copy_to_replacer(Node_replacer_temp<MAP> &Target)
    {
        Target.clear();
        for (const auto &del : _deletions_)
        {
            Target.set(del.source(), del.target());
        }
    }

    template <typename MAP>
    void SingleHOSC::copy_from_replacer(const Node_replacer_temp<MAP> &Source)
    {
        _deletions_.clear();
        _deletions_.reserve(n_nodes_);
        for (auto s = 0; s < n_nodes_; s++)
        {
            auto t = Source.get(s);
            if (t == s)
                continue;
            _deletions_.emplace_back(SingleDel(s, t));
        }
    }

    void SingleHOSC::Test()
    {
        Node_linker linker;
        copy_to_replacer(linker);
    }
    /**
     * @brief 
     * 
     * @param source 
     * @param target 
     * @return int 
     */
    inline int clear_canonical_single(int &source, int &target)
    {
        if (source == target)
            return 0;
        if (source < target)
        {
            std::swap(source, target);
        }
        return 1;
    }

    SingleHOSC::SingleHOSC(int source, int target, short max_nodes, long long weight) noexcept
        : n_nodes_(max_nodes), weight_(weight)
    {
        weight_ *= clear_canonical_single(source, target);
        if (weight_)
            _deletions_.emplace_back(SingleDel(source, target));
    }

    SingleHOSC::SingleHOSC(initial_del_set initial_dels, short max_nodes, long long weight) noexcept
        : n_nodes_(max_nodes), weight_(weight)
    {
        if (initial_dels.size() > n_nodes_)
        {
            weight_ = 0;
            return;
        }
        auto replacer = Node_replacer();
        for (auto del : initial_dels)
        {
            auto p = replacer.get(del.source());
            auto r = replacer.get(del.target());
            weight_ *= clear_canonical_single(p, r);
            if (weight_ == 0)
            {
                _deletions_.clear();
                return;
            }
            insert_del(p, r);
            replacer.set(p, r);
        }
    }

    SingleHOSC::SingleHOSC(const SingleHOSC &Source, const NodeTrans::NtoN &trans_map) : n_nodes_(Source.n_nodes_), weight_(Source.weight_)
    {
        auto replacer = Node_replacer();
        for (auto del : Source._deletions_)
        {
            // auto source = replacer.get(del.source());
            auto source = del.source();
            if (source != virtual_node)
            {
                auto source_it = trans_map.find(source);
                if (source_it == trans_map.end())
                {
                    std::ostringstream ss;
                    ss << source << " is expected in the map\n";
                    throw std::invalid_argument(ss.str());
                }
                source = source_it->second;
            }
            // auto target = replacer.get(del.target());
            auto target = del.target();
            if (target != virtual_node)
            {
                auto target_it = trans_map.find(target);
                if (target_it == trans_map.end())
                {
                    std::ostringstream ss;
                    ss << target << " is expected in the map\n";
                    throw std::invalid_argument(ss.str());
                }
                target = target_it->second;
            }
            source = replacer.get(source);
            target = replacer.get(target);
            weight_ *= clear_canonical_single(source, target);
            if (weight_ == 0)
            {
                _deletions_.clear();
                return;
            }
            insert_del(source, target);
            replacer.set(source, target);
        }
    }

    SingleHOSC::SingleHOSC(const SingleHOSC &Source, int newNode) : n_nodes_(Source.n_nodes_), weight_(Source.weight_)
    {
        auto replacer = Node_replacer();
        for (auto del : Source._deletions_)
        {
            auto source = del.source();
            if (source == virtual_node)
                source = newNode;
            auto target = del.target();
            if (target == virtual_node)
                target = newNode;
            source = replacer.get(source);
            target = replacer.get(target);
            weight_ *= clear_canonical_single(source, target);
            if (weight_ == 0)
            {
                _deletions_.clear();
                return;
            }
            insert_del(source, target);
            replacer.set(source, target);
        }
    }

    SingleHOSC::SingleHOSC(const SingleHOSC &Source, bool renove_virtual) : n_nodes_(Source.n_nodes_), weight_(Source.weight_)
    {
        auto replacer = Node_replacer();
        bool virtual_deleted = false;
        for (auto del : Source._deletions_)
        {
            auto source = replacer.get(del.source());
            auto target = replacer.get(del.target());
            weight_ *= clear_canonical_single(source, target);
            if (weight_ == 0)
            {
                _deletions_.clear();
                return;
            }
            if (source != virtual_node && target != virtual_node)
                insert_del(source, target);
            else
                virtual_deleted = true;
            replacer.set(source, target);
        }
        if (!virtual_deleted)
        {
            weight_ = 0;
            _deletions_.clear();
        }
    }

    std::string SingleHOSC::String(const NodeTrans *pTranslater) const
    {
        auto translate = [pTranslater](int node) {
            if (pTranslater == nullptr || node == virtual_node)
                return node;
            return pTranslater->inN2extN(node);
        };
        std::ostringstream out;
        if (weight_ != 1)
            out << weight_ << "*";
        out << "Δ";
        for (const auto &del : _deletions_)
        {
            auto p = translate(del.source());
            auto r = translate(del.target());
            out << "(";
            if (p == virtual_node)
                out << "j";
            else
                out << p;
            out << "+";
            if (r == virtual_node)
                out << "j";
            else
                out << r;
            out << "),";
        }
        auto str = out.str();
        if (str[str.length() - 1] == ',')
            str = str.substr(0, str.length() - 1);
        return str;
    }

    bool SingleHOSC::operator==(const SingleHOSC &Other) const
    {
        return /*n_nodes_ == Other.n_nodes_ &&*/ _deletions_ == Other._deletions_;
    }

    std::size_t SingleHOSC::hash() const
    {
        size_t h = 1; //std::hash<short>{}(n_nodes_);
        // HashCombine(h, std::hash<int>{}(weight_));
        std::for_each(_deletions_.begin(), _deletions_.end(), [&h](const SingleDel &del) {
            HashCombine(h, std::hash<SingleDel>{}(del));
        });
        return h;
    }

    std::optional<int> SingleHOSC::numeric_value() const
    {
        if (is_complete())
            return weight_;
        return {};
    }

    SingleHOSC::HOSC_oper_result SingleHOSC::HOSC_big_dot(HOSC_oper_result h2, const nodes_to_remove &nodes) const
    {
        if (!h2)
            throw std::invalid_argument("BigDot needs real argument!");
        // if (n_nodes_ != h2->n_nodes_)
        //     throw std::logic_error("HOSC must have the same No of nodes!");
        auto &ext_deletions = h2->_deletions_;
        if (_deletions_.size() + ext_deletions.size() < nodes.size())
            return {};
        if (_deletions_.size() + ext_deletions.size() > n_nodes_)
            return {};
        // if (_deletions_.size() + ext_deletions.size() > (n_nodes_+extra_no_nodes))
        //     return {};
        // auto rem_nodes = Removing_single_nodes(nodes);
        std::set<int> rem_nodes(nodes.begin(), nodes.end());
        auto del_cleanear = [&rem_nodes](int &p, int &r) {
            if (!rem_nodes.contains(p) && (p < r || rem_nodes.contains(r)))
            {
                std::swap(p, r);
            }
        };
        // auto del_cleanear = [&rem_nodes](int &p, int &r) {
        //     if (!rem_nodes.is_to_be_removed(p) && (p < r || rem_nodes.is_to_be_removed(r)))
        //     {
        //         std::swap(p, r);
        //     }
        // };
        auto replacer = Node_replacer();
        auto mit = _deletions_.begin();
        auto mit_e = _deletions_.end();
        auto hit = ext_deletions.begin();
        auto hit_e = ext_deletions.end();
        HOSC_oper_result res = std::make_shared<SingleHOSC>(n_nodes_);
        auto no_del = _deletions_.size() + ext_deletions.size();
        auto no_rem = nodes.size();
        while (mit != mit_e || hit != hit_e)
        {
            if (no_del < no_rem)
                return {};
            int p, r;
            p = r = 0;
            enum
            {
                none,
                mine,
                yours
            } which = none;
            if (mit != mit_e)
            {
                p = replacer.get(mit->source());
                r = replacer.get(mit->target());
                if (p == r)
                    return {};
                del_cleanear(p, r);
                which = mine;
            }
            // bool left_must_be_removed = (which != none) && rem_nodes.is_to_be_removed(p);
            bool left_must_be_removed = (which != none) && rem_nodes.contains(p);
            if (!left_must_be_removed)
            {
                if (hit != hit_e)
                {
                    auto p1 = replacer.get(hit->source());
                    auto r1 = replacer.get(hit->target());
                    if (p1 == r1)
                        return {};
                    del_cleanear(p1, r1);
                    if ((p1 == p && r1 == r))
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
                        which = yours;
                    }
                }
            }
            if (which == none)
                break;
            // if (!rem_nodes.touch(p))
            if (!rem_nodes.contains(p))
            {
                res->insert_del(p, r);
            }
            else
                no_rem--;
            replacer.set(p, r);
            switch (which)
            {
            case mine:
                mit++;
                break;
            case yours:
                hit++;
                break;
            case none:;
            }
            no_del--;
        } //while
        // if (!rem_nodes.complete())
        if (no_rem != 0)
            return {};
        else
        {
            res->weight_ = weight_ * h2->weight_;
            res->n_nodes_ -= nodes.size();
        }

        return res;
    }
#ifdef _DEBUG_TEST

    void SingleHOSC::combine(const SingleHOSC &right)
    {
        if (_deletions_ != right._deletions_)
            throw std::invalid_argument("You can combine only the same HOSCs");
        weight_ += right.weight_;
    }
#endif
    // HOSCUniqueCollection<SingleHOSC> SingleHOSCCollection;
}; // namespace HOSC