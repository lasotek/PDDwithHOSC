/**
 * @file Cluster.cpp
 * @author Sławomir Lasota  (lasotek@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2021-02-05
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#include <exception>
#include <sstream>
#include <algorithm>
#include "Cluster.h"

namespace HOSC
{
    KICluster::int_ki_cont::int_ki_cont(mine_cluster_ptr cluster, const bounadry_connections_list &connections) : clusters_p(cluster)
    {
        for (auto c : connections)
        {
            auto in = c.first;
            if (!clusters_p->is_boundary_node(in))
            {
                std::ostringstream ss;
                ss << "Internal node " << in << " is not defined as a boundary one\n";
                throw std::invalid_argument(ss.str());
            }
            if (translator.inN2extN(in) >= 0)
            {
                std::ostringstream ss;
                ss << "Internal node " << in << " appears more than once\n";
                throw std::invalid_argument(ss.str());
            }

            auto ex = c.second;
            if (translator.extN2inNcheck(ex) >= 0)
            {
                std::ostringstream ss;
                ss << "External node " << ex << " appears more than once\n";
                throw std::invalid_argument(ss.str());
            }
            if (!translator.forceNodesTranslation(ex, in))
            {
                std::ostringstream ss;
                ss << "Nodes: external " << ex << " or internal " << in << " appear more than once\n";
                throw std::invalid_argument(ss.str());
            }
            // ext2int[ex] = in;
            // int2ext[in] = ex;
        }
    }

    KICluster::set_of_nodes KICluster::int_ki_cont::get_cluster_ex_nodes() const
    {
        set_of_nodes local_set;
        auto out_set = clusters_p->get_bound_nodes_ext();
        for (auto n : out_set)
        {
            local_set.insert(translator.inN2extN(n));
        }
        return local_set;
    }

    KICluster::set_of_nodes KICluster::get_bound_nodes_ext() const
    {
        set_of_nodes local_set;
        for (auto n : bound_nodes_)
        {
            if(n==virtual_node)
                continue;
            local_set.insert(node_trans_.inN2extN(n));
        }
        return local_set;
    }

    void KICluster::to_stream(std::ostream &os)
    {
        os << "The cluster results:\n";
        os << "\tDenominator = " << denom_->String(node_trans_) << std::endl;
        os << "\tInternal Numerators = " << numers_->String(node_trans_) << std::endl;
        os << "\tExternal Numerators = " << numers_ext->String(node_trans_) << std::endl;
    }

    KICluster::KICluster(bool use_threads) noexcept : use_threads_(use_threads)
    {
        bound_nodes_.insert(virtual_node);
    }

    KICluster::KICluster(const boundary_nodes_list &_boundary_nodes, bool use_threads) noexcept : use_threads_(use_threads)
    {
        bound_nodes_.insert(virtual_node);
        for (auto n : _boundary_nodes)
        {
            n = node_trans_.extN2inN(n);
            bound_nodes_.insert(n);
        }
    }

    KICluster::~KICluster()
    {
    }

    bool KICluster::remove_boundary_node(int node)
    {
        auto it = bound_nodes_.find(node);
        if (it == bound_nodes_.end())
            return false;
        bound_nodes_.erase(it);
        return true;
    }

    void KICluster::Solve()
    {
        if (!dirty())
            return;
        set_of_nodes Nodes_to_be_removed_as_first;
        if (clusters_p)
        {
            for (auto &Cluster : *clusters_p)
            {
                auto exn_set = Cluster.get_cluster_ex_nodes();
                for (auto n : exn_set)
                {
                    if (!bound_nodes_.contains(n))
                        Nodes_to_be_removed_as_first.insert(n);
                }
                Cluster.clusters_p->Solve(); //change to thread
            }
        }
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

    KICluster::int_ki_cont *KICluster::insert_cluster(const mine_cluster_ptr cluster, const init_bounadry_connections_list &connection_list)
    {
        init_clusters_list();
        bounadry_connections_list local_list(connection_list);

        for (auto &node_pair : local_list)
        {
            node_pair.second = node_trans_.extN2inN(node_pair.second);
        }

        clusters_p->emplace_back(int_ki_cont(cluster, local_list));
        return &(clusters_p->back());
    }

    bool KICluster::is_boundary_node(int node) const
    {
        auto exnode = node_trans_.extN2inNcheck(node);
        if (exnode < 0)
            return false;
        return bound_nodes_.contains(exnode);
    }

} // namespace HOSC