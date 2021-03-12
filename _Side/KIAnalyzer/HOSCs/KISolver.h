/**
 * @file KISolver.h
 * @author Sławomir Lasota  (lasotek@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2021-02-05
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#ifndef __KISOLVER_H__
#define __KISOLVER_H__
#include <list>
#include <memory>
#include <unordered_map>
#include <algorithm>
#include <optional>
#include <set>
#include "KIIndexCol.h"
#include "helpers/helpers.h"
#include "Edge.h"

namespace HOSC
{

    /**
     * @brief Solver
     * 
     * @tparam W Type of the wieight. Arithmetic
     */
    template <Arithmetic W>
    class KISolver : public std::enable_shared_from_this<KISolver<W>>
    {
    private:
        using edges_list_ptr = std::list<std::shared_ptr<Edge<W>>>;
        using edges_list_iter = edges_list_ptr::iterator;
        using incidence_map = std::unordered_map<int, edges_list_ptr>;
        using map_iterator = incidence_map::iterator;
        using incidence_sets = std::unordered_map<int, std::set<int>>;
        using set_of_nodes = std::set<int>;
        NodeTrans node_trans_;
        incidence_map incidences_;
        // incidence_sets incidences_set_;
        int max_nodes_ = -1;
        std::optional<W> denom_value_ = {};
        std::optional<W> numer_value_ = {};
        edges_list_ptr main_list_;
        std::shared_ptr<KIIndexCol> denom_;
        std::shared_ptr<KIIndexCol> numers_;
        std::shared_ptr<KIIndexCol> numers_j;
        map_iterator min_incidence()
        {
            return std::min_element(incidences_.begin(), incidences_.end(), [](const auto &T1, const auto &T2) {
                return T1.second.size() < T2.second.size();
            });
        }
        void get_incidence_with(int node, set_of_nodes &res)
        {
            auto my_edge_list = incidences_[node];
            for (auto &e : my_edge_list)
            {
                res.insert(e->node_a == node ? e->node_b : e->node_a);
            }
        }

        /**
         * @brief 
         * 
         * @param set 
         * @return int the best incident node
         */
        int choose_com_incidence(const set_of_nodes &set)
        {
            if (set.empty())
                throw std::invalid_argument("No nodes! Impossible!!!!");
            incidence_sets sets;
            auto it = set.begin();
            auto prev_n = *it++;
            get_incidence_with(prev_n, sets[prev_n]);
            for (; it != set.end(); it++)
            {
                auto n = *it;
                get_incidence_with(n, sets[n]);
                auto prev_it = std::prev(it);
                for (auto prev_it = std::prev(it);; prev_it--)
                {
                    prev_n = *prev_it;
                    nodes_pins com_incidences;
                    std::set_intersection(sets[prev_n].begin(), sets[prev_n].end(),
                                          sets[n].begin(), sets[n].end(),
                                          std::back_inserter(com_incidences));
                    if (!com_incidences.empty())
                        return *com_incidences.begin();
                    if (prev_it == set.begin())
                        break;
                }
                // prev_n = n;
            }
            auto min_it = std::min_element(sets.begin(), sets.end(), [](const auto &T1, const auto &T2) {
                return T1.second.size() < T2.second.size();
            });
            return *min_it->second.begin();
        }

        /**
         * @brief 
         * 
         * @param set 
         * @param newEdges 
         * @return int 
         */
        int prepare_next_edges(const set_of_nodes &set, edges_list_ptr &newEdges)
        {
            auto newNode = choose_com_incidence(set);
            auto &local_inc_eges = incidences_[newNode];
            for (auto &e : local_inc_eges)
            {
                if (set.contains(e->node_a) || set.contains(e->node_b))
                    newEdges.push_back(e);
                // auto &edge = e->second
            }
            return newNode;
        }

        void make_dirty()
        {
            // dirty_ = true;
            max_nodes_ = -1;
            denom_value_.reset();
            numer_value_.reset();
            node_trans_.reset();
        }
        bool dirty() const { return !(denom_value_.has_value() && numer_value_.has_value()); }

    public:
        KISolver() = default;
        KISolver(const Edges<W> &edges_list) noexcept
        {
            for (auto &e : edges_list)
            {
                main_list_.emplace_back(std::make_shared<Edge<W>>(e));
            }
        }
        void AddEdge(const Edge<W> &edge)
        {
            main_list_.emplace_back(std::make_shared<Edge<W>>(edge));
            make_dirty();
        }

        /**
         * @brief New version
         * 
         */
        void Solve2()
        {
            if (!dirty())
                return;
            for (auto &e : main_list_)
            {
                // auto E = e.shared_from_this(); // std::make_shared<Edge<W>>(e);
                // auto E =  std::make_shared<Edge<W>>(e);
                auto a = node_trans_.extN2inN(e->node_a);
                auto b = node_trans_.extN2inN(e->node_b);
                auto newE = std::make_shared<Edge<W>>(a, b, e->weight_);
                incidences_[a].push_back(newE);
                incidences_[b].push_back(newE);
                max_nodes_ = std::max(max_nodes_, a);
                max_nodes_ = std::max(max_nodes_, b);
                // incidences_set_[a].insert(b);
                // incidences_set_[b].insert(a);
            }
            auto iter = min_incidence();
            auto init_edge_ptr = *iter->second.begin();
            set_of_nodes set_n{{init_edge_ptr->node_a}, {init_edge_ptr->node_b}};
            denom_ = std::make_shared<KIIndexCol>(*init_edge_ptr, max_nodes_ + 1, KIIndexCol::com_denomnator);
            numers_ = std::make_shared<KIIndexCol>(*init_edge_ptr, max_nodes_ + 1, KIIndexCol::sum_numerators);
            numers_j = std::make_shared<KIIndexCol>(*init_edge_ptr, max_nodes_ + 1, KIIndexCol::extern_connections);
            nodes_to_remove rn;
            std::erase(incidences_[init_edge_ptr->node_b], init_edge_ptr);
            if (incidences_[init_edge_ptr->node_b].empty())
            {
                rn.push_back(init_edge_ptr->node_b);
                set_n.erase(init_edge_ptr->node_b);
                incidences_.erase(init_edge_ptr->node_b);
            }
            std::erase(incidences_[init_edge_ptr->node_a], init_edge_ptr);
            if (incidences_[init_edge_ptr->node_a].empty())
            {
                rn.push_back(init_edge_ptr->node_a);
                set_n.erase(init_edge_ptr->node_a);
                incidences_.erase(init_edge_ptr->node_a);
            }
            while (!incidences_.empty())
            {
                edges_list_ptr local_list;
                auto newNode = prepare_next_edges(set_n, local_list);
                set_n.insert(newNode);
                numers_->add_to_with_virtual_node_update(*numers_j, newNode);
                numers_j->add_shortcut_remove_node(*denom_, {virtual_node, newNode}, {});
                for (auto edge_ptr : local_list)
                {
                    auto a = edge_ptr->node_a;
                    auto b = edge_ptr->node_b;
                    auto it_a = incidences_.find(a);
                    auto it_b = incidences_.find(b);
                    std::erase(it_a->second, edge_ptr);
                    std::erase(it_b->second, edge_ptr);
                    if (it_b->second.empty())
                    {
                        set_n.erase(b);
                        incidences_.erase(it_b);
                        if (!incidences_.empty())
                            rn.push_back(b);
                    }
                    if (it_a->second.empty())
                    {
                        set_n.erase(a);
                        incidences_.erase(it_a);
                        if (!incidences_.empty())
                            rn.push_back(a);
                    }
                    auto ed = std::make_pair(a, b);
                    denom_->add_edge_remove_node(ed, rn, edge_ptr->weight_);
                    numers_->add_edge_remove_node(ed, rn, edge_ptr->weight_);
                    numers_j->add_edge_remove_node(ed, rn, edge_ptr->weight_);
                    rn.clear();
                }
            }
            denom_value_ = denom_->value();
            numer_value_ = numers_->value();
        }

        /**
         * 
         * @brief Old version
         */
        void Solve()
        {
            if (!dirty())
                return;
            for (auto &e : main_list_)
            {
                // auto E = e.shared_from_this(); // std::make_shared<Edge<W>>(e);
                // auto E =  std::make_shared<Edge<W>>(e);
                auto a = node_trans_.extN2inN(e->node_a);
                auto b = node_trans_.extN2inN(e->node_b);
                auto newE = std::make_shared<Edge<W>>(a, b, e->weight_);
                incidences_[a].push_back(newE);
                incidences_[b].push_back(newE);
                max_nodes_ = std::max(max_nodes_, a);
                max_nodes_ = std::max(max_nodes_, b);
            }
            denom_ = std::make_shared<KIIndexCol>(max_nodes_ + 1, KIIndexCol::com_denomnator);
            numers_ = std::make_shared<KIIndexCol>(max_nodes_ + 1, KIIndexCol::sum_numerators);
            while (!incidences_.empty())
            {
                auto iter = min_incidence();
                auto index = iter->first;
                auto i = iter->second.size() - 1;
                for (auto eit = iter->second.begin(); eit != iter->second.end(); eit++, i--)
                {
                    auto edge_ptr = *eit;
                    auto &edge = *edge_ptr;
                    nodes_to_remove rn;
                    if (index != edge.node_a)
                    {
                        std::erase(incidences_[edge.node_a], edge_ptr);
                        if (incidences_[edge.node_a].empty())
                            rn.push_back(edge.node_a);
                    }
                    else
                    {
                        std::erase(incidences_[edge.node_b], edge_ptr);
                        if (incidences_[edge.node_b].empty())
                            rn.push_back(edge.node_b);
                    }
                    KIIndexCol::del_pair ed = std::make_pair(edge.node_a, edge.node_b);
                    if (i == 0 && incidences_.size() > 2)
                        rn.push_back(index);
                    numers_->add_edge_remove_node(ed, rn, edge.weight_);
                    denom_->add_edge_remove_node(ed, rn, edge.weight_);
                }
#ifdef _DEBUG_TEST
                std::cout << "Node: " << index << " was removed.\n";
#endif

                incidences_.erase(iter);
            }

            denom_value_ = denom_->value();
            numer_value_ = numers_->value();
        }
        inline auto KIIndex() const { return (double)numer_value_.value() / denom_value_.value(); }
        inline auto numerator() const { return numer_value_.value(); }
        inline auto denominator() const { return denom_value_.value(); }
        inline bool is_ready_and_connected() { return denom_value_.has_value() && numer_value_.has_value(); }
    };
} // namespace HOSC
#endif // __KISOLVER_H__