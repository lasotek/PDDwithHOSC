#ifndef __CLUSTER_H__
#define __CLUSTER_H__
#include <set>
#include <list>
#include <limits>
#include <iostream>
#include "Edge.h"
#include "KIIndexCol.h"
#include <initializer_list>

namespace HOSC
{
    class KICluster
    {
    public:
        using boundary_nodes_list = std::initializer_list<int>;

    private:
        friend std::ostream &operator<<(std::ostream &os, KICluster &claster);
        using W = long long;
        std::set<int> bound_nodes_;
        using edges_list_ptr = std::list<std::shared_ptr<Edge<W>>>;
        using incidence_map = std::unordered_map<int, edges_list_ptr>;
        using map_iterator = incidence_map::iterator;
        NodeTrans node_trans_;
        incidence_map incidences_;
        int max_nodes_ = -1;
        std::optional<W> denom_value_ = {};
        std::optional<W> numer_value_ = {};
        edges_list_ptr main_list_;
        std::shared_ptr<KIIndexCol> denom_;
        std::shared_ptr<KIIndexCol> numers_;
        std::shared_ptr<KIIndexCol> numers_ext;
        ;
        map_iterator min_incidence()
        {
            return std::min_element(incidences_.begin(), incidences_.end(), [this](const auto &T1, const auto &T2) {
                bool is_b_T1 = this->bound_nodes_.contains(T1.first);
                bool is_b_T2 = this->bound_nodes_.contains(T2.first);
                if (is_b_T1 == is_b_T2)
                    return T1.second.size() < T2.second.size();
                else
                    return is_b_T2;
            });
        }
        void make_dirty()
        {
            // dirty_ = true;
            // max_nodes_ = -1;
            denom_value_.reset();
            numer_value_.reset();
            // node_trans_.reset();
        }
        bool dirty() const { return !(denom_value_.has_value() && numer_value_.has_value()); }
        void to_stream(std::ostream &os);

    public:
        KICluster();
        KICluster(const boundary_nodes_list &_boundary_nodes);
        ~KICluster();
        /**
         * @brief Add another boundaty node
         * 
         * @param node New nodes
         * @return true The node is new
         * @return false The not has been already declared
         */
        inline bool add_bundary_node(int node)
        {
            return bound_nodes_.insert(node_trans_.extN2inN(node)).second;
        }
        /**
         * @brief Remove boundary node if it exists
         * 
         * @param node Boundary node
         * @return true If node has been defined and was removed
         * @return false If node has not been defined at all
         */
        bool remove_boundary_node(int node);
        inline bool is_boundary_node(int node) { return bound_nodes_.contains(node); }
        KICluster(const Edges<W> &edges_list)
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
                if (a != virtual_node)
                    max_nodes_ = std::max(max_nodes_, a);
                if (b != virtual_node)
                    max_nodes_ = std::max(max_nodes_, b);
            }
            denom_ = std::make_shared<KIIndexCol>(max_nodes_ + 1, KIIndexCol::com_denomnator);
            numers_ = std::make_shared<KIIndexCol>(max_nodes_ + 1, KIIndexCol::sum_numerators);
            numers_ext = std::make_shared<KIIndexCol>(max_nodes_ + 1, KIIndexCol::extern_connections);
            while (!incidences_.empty())
            {
                auto iter = min_incidence();
                auto index = iter->first;
                auto i = iter->second.size() - 1;
                for (auto eit = iter->second.begin(); eit != iter->second.end(); eit++, i--)
                {
                    auto edge_ptr = *eit;
                    auto &edge = *edge_ptr;
                    if (index != edge.node_a)
                        std::erase(incidences_[edge.node_a], edge_ptr);
                    else
                        std::erase(incidences_[edge.node_b], edge_ptr);
                    KIIndexCol::del_pair ed = std::make_pair(edge.node_a, edge.node_b);
                    KIIndexCol::rem_node rn;
                    if (i == 0 && !bound_nodes_.contains(index))
                        rn = index;
                    numers_->add_edge_remove_node(ed, rn, edge.weight_);
                    denom_->add_edge_remove_node(ed, rn, edge.weight_);
                    numers_ext->add_edge_remove_node(ed, rn, edge.weight_);
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
    };

    inline std::ostream &operator<<(std::ostream &os, KICluster &cluster)
    {
        cluster.to_stream(os);
        return os;
    }
} // namespace HOSC

#endif // __CLUSTER_H__