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
#include <thread>
#include "Cluster.h"
#include "KIIndexCol.h"

namespace HOSC
{
    KICluster::int_ki_cont::ext_interface::ext_interface(const ext_interface &Source) : only_my_pins_(Source.only_my_pins_),
                                                                                        denom_(std::make_shared<KIIndexCol>(*Source.denom_)),
                                                                                        numers_(std::make_shared<KIIndexCol>(*Source.numers_)),
                                                                                        numers_ext_(std::make_shared<KIIndexCol>(*Source.numers_ext_))
    {
    }

    void KICluster::int_ki_cont::ext_interface::leave_external()
    {
        nodes_pins res;
        std::set_difference(ext_my_pins_.begin(), ext_my_pins_.end(),
                            only_my_pins_.begin(), only_my_pins_.end(),
                            std::back_inserter(res));
        ext_my_pins_ = res;
    }

    KICluster::int_ki_cont::ext_interface_ptr KICluster::int_ki_cont::big_O_dot(ext_interface_ptr pLeft,
                                                                                ext_interface_ptr pRight,
                                                                                nodes_pins nodes_to_remove)
    {
        nodes_pins nodes_to_remove1;
        std::set_intersection(pRight->ext_my_pins_.begin(), pRight->ext_my_pins_.end(),
                              nodes_to_remove.begin(), nodes_to_remove.end(),
                              std::back_inserter(nodes_to_remove1));
        nodes_pins nodes_to_remove2(nodes_to_remove);
        nodes_to_remove2.push_back(virtual_node);
        auto res = std::make_shared<ext_interface>();
        res->denom_ = big_O_dot_Col(pLeft->denom_, pRight->denom_, nodes_to_remove1);
        res->numers_ = big_O_dot_Col(pLeft->numers_, pRight->denom_, nodes_to_remove1);
        *(res->numers_) += *big_O_dot_Col(pLeft->denom_, pRight->numers_, nodes_to_remove1);
        *(res->numers_) += *big_O_dot_Col(pLeft->numers_ext_, pRight->numers_ext_, nodes_to_remove2);
        res->numers_ext_ = big_O_dot_Col(pLeft->numers_ext_, pRight->denom_, nodes_to_remove1);
        *(res->numers_ext_) += *big_O_dot_Col(pLeft->denom_, pRight->numers_ext_, nodes_to_remove1);
        //to do
        std::set_difference(pRight->ext_my_pins_.begin(), pRight->ext_my_pins_.end(),
                            nodes_to_remove1.begin(), nodes_to_remove1.end(),
                            std::back_inserter(res->ext_my_pins_));

        return res;
    }

    KICluster::int_ki_cont::int_ki_cont(mine_cluster_ptr cluster, const bounadry_connections_list &connections) : clusters_p(cluster),
                                                                                                                  is_parallel_running(not_yet),
                                                                                                                  interface_ptr(std::make_shared<ext_interface>())
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
            // only_my_pins().push_back(ex);
            ext_my_pins().push_back(ex);
            // ext2int[ex] = in;
            // int2ext[in] = ex;
        }
        std::sort(only_my_pins().begin(), only_my_pins().end());
        solve_future = promise.get_future();
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

    int KICluster::int_ki_cont::translate_node(int clust_int_node)
    {
        if (clust_int_node == virtual_node)
            return clust_int_node;
        if (clusters_p->is_boundary_node(clust_int_node))
        {
            std::ostringstream ss;
            ss << "Internal nodes " << clust_int_node << " cannot be visible outside\n";
            throw std::invalid_argument(ss.str());
        }
        auto cl_ex_node = clusters_p->node_trans_.inN2extN(clust_int_node);

        return translator.inN2extN(cl_ex_node);
    }

    std::shared_ptr<KIIndexCol> KICluster::int_ki_cont::get_translated(std::shared_ptr<KIIndexCol> Origin, NodeTrans::NtoN &map)
    {
        auto res = Origin->copy_col_with_node_translated(map);
        return res;
    }

    void KICluster::int_ki_cont::update_n_nodes(int n_nodes)
    {
        interface_ptr->update_n_nodes(n_nodes);
    }

    void KICluster::int_ki_cont::Solve()
    {
        if (is_parallel_running)
            return;
        clusters_p->Solve();
        NodeTrans::NtoN map;
        get_boundary_translation(map);
        denom() = get_denom_tr(map);
        numers() = get_numers_tr(map);
        numers_ext() = get_numers_ext_tr(map);
    }

    void KICluster::int_ki_cont::SolveParallel() //to implement
    {
        if (is_parallel_running)
            return;
        std::thread([this] {
            this->is_parallel_running = running;
            this->clusters_p->Solve();
            NodeTrans::NtoN map;
            this->get_boundary_translation(map);
            this->denom() = get_denom_tr(map);
            this->numers() = get_numers_tr(map);
            this->numers_ext() = get_numers_ext_tr(map);
            this->promise.set_value(true);
        }).detach();
        // solve_future = std::async(std::launch::async, [this] {
        //     is_parallel_running = running;
        //     clusters_p->Solve();
        //     NodeTrans::NtoN map;
        //     get_boundary_translation(map);
        //     denom() = get_denom_tr(map);
        //     numers() = get_numers_tr(map);
        //     numers_ext() = get_numers_ext_tr(map);
        //     // is_parallel_running = false;

        //     // is_parallel_running = done;
        //     return false; });
    }

    void KICluster::int_ki_cont::get_boundary_translation(NodeTrans::NtoN &map) const
    {
        clusters_p->get_boundary_translation(map);
        for (auto [ni, no] : map)
        {
            map[ni] = translator.inN2extN(no);
        }
        //to do
    }

    void KICluster::get_boundary_translation(NodeTrans::NtoN &map) const
    {
        map.clear();
        for (auto n : bound_nodes_)
        {
            if (n == virtual_node)
                continue;
            map[n] = node_trans_.inN2extN(n);
        }
    }

    KICluster::set_of_nodes KICluster::get_bound_nodes_ext() const
    {
        set_of_nodes local_set;
        for (auto n : bound_nodes_)
        {
            if (n == virtual_node)
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
        set_of_nodes Nodes_with_edges;
        for (auto &e : main_list_)
        {
            // auto E = e.shared_from_this(); // std::make_shared<Edge<W>>(e);
            // auto E =  std::make_shared<Edge<W>>(e);
            auto a = node_trans_.extN2inN(e->node_a);
            auto b = node_trans_.extN2inN(e->node_b);
            Nodes_with_edges.insert(a);
            Nodes_with_edges.insert(b);
            auto newE = std::make_shared<Edge<W>>(a, b, e->weight_);
            incidences_[a].push_back(newE);
            incidences_[b].push_back(newE);
            if (a != virtual_node)
                max_nodes_ = std::max(max_nodes_, a);
            if (b != virtual_node)
                max_nodes_ = std::max(max_nodes_, b);
        }
        // nodes_pins Edges_pins(Nodes_with_edges.begin(), Nodes_with_edges.end());
        // Egdges_pins.assign(Nodes_with_edges.begin(), Nodes_with_edges.end());
        ext_interface_stack interface_stack;
        if (clusters_p)
        {
            if (use_threads_)
            {
                //parallel
                for (auto &Cluster : *clusters_p)
                {
                    Cluster.is_parallel_running = int_ki_cont::not_yet;
                }
                for (auto &Cluster : *clusters_p)
                {
                    Cluster.leave_only_internal_nodes(Nodes_with_edges);
                    Cluster.leave_only_internal_nodes(bound_nodes_);
                    Cluster.leave_external();
                    Cluster.SolveParallel(); //change to thread and everything
                }
                auto num_of_clusters = clusters_p->size();
                while (!all_finished())
                {
                    for (auto &Cluster : *clusters_p)
                    {
                        if (Cluster.is_parallel_running != int_ki_cont::done && Cluster.solve_future.wait_for(std::chrono::microseconds(100)) == std::future_status::ready)
                        {
                            Cluster.update_n_nodes(max_nodes_ + 1);
                            auto p_interface = Cluster.get_interface();
                            // p_interface->p_my_ext_nodes = &bound_nodes_;
                            interface_stack.push_back(p_interface);
                            Cluster.is_parallel_running = int_ki_cont::done;
                            Cluster.solve_future.get();
                            // break;
                        }
                    }
                }
            }
            else
            {
                //serial
                for (auto &Cluster : *clusters_p)
                {
                    Cluster.leave_only_internal_nodes(Nodes_with_edges);
                    Cluster.leave_only_internal_nodes(bound_nodes_);
                    Cluster.leave_external();
                    Cluster.Solve();
                    Cluster.update_n_nodes(max_nodes_ + 1);
                    auto p_interface = Cluster.get_interface();
                    // p_interface->p_my_ext_nodes = &bound_nodes_;
                    interface_stack.push_back(p_interface);
                }
            }
            // while (interface_stack.size() > 1)
            // {
            //     auto i1 = interface_stack.top();
            //     interface_stack.pop();
            //     auto i2 = interface_stack.top();
            //     interface_stack.pop();
            //     auto ires = int_ki_cont::big_O_dot(i1, i2);
            //     interface_stack.push(ires);
            // }
            // alternative way
            while (interface_stack.size() > 1)
            {
                auto i1 = interface_stack.back();
                interface_stack.pop_back();
                auto &n_e = i1->ext_my_pins_;
                set_of_nodes loose_dongles;
                for (auto &n : n_e)
                {
                    auto iter = incidences_.find(n);
                    if (iter != incidences_.end())
                    {
                        auto i = iter->second.size() - 1;
                        for (auto eit = iter->second.begin(); eit != iter->second.end(); eit++, i--)
                        {
                            auto edge_ptr = *eit;
                            auto &edge = *edge_ptr;
                            if (n != edge.node_a)
                            {
                                std::erase(incidences_[edge.node_a], edge_ptr);
                                if (incidences_[edge.node_a].size() == 0)
                                {
                                    if (!bound_nodes_.contains(edge.node_a))
                                        loose_dongles.insert(edge.node_a);
                                    incidences_.erase(edge.node_a);
                                }
                            }
                            else
                            {
                                std::erase(incidences_[edge.node_b], edge_ptr);
                                if (incidences_[edge.node_b].size() == 0)
                                {
                                    if (!bound_nodes_.contains(edge.node_b))
                                        loose_dongles.insert(edge.node_b);
                                    incidences_.erase(edge.node_b);
                                }
                            }
                            KIIndexCol::del_pair ed = std::make_pair(edge.node_a, edge.node_b);
                            KIIndexCol::rem_node rn;
                            if (i == 0 && !bound_nodes_.contains(n))
                                rn = n;
                            i1->numers_->add_edge_remove_node(ed, rn, edge.weight_);
                            i1->denom_->add_edge_remove_node(ed, rn, edge.weight_);
                            i1->numers_ext_->add_edge_remove_node(ed, rn, edge.weight_);
                        }
                        incidences_.erase(iter);
                    }
                }
                auto it2 = std::max_element(interface_stack.begin(), interface_stack.end(), [&loose_dongles](auto &v1, auto &v2) {
                    return v1->num_of_incidents(loose_dongles) < v2->num_of_incidents(loose_dongles);
                });
                auto i2 = *it2;
                interface_stack.remove(i2);
                if (incidences_.empty() && (bound_nodes_.size() == 1) /* && !loose_dongles.empty()*/)
                    loose_dongles.extract(loose_dongles.begin());
                if (bound_nodes_.size() > 1)
                {
                    set_of_nodes aux(bound_nodes_);
                    aux.insert(loose_dongles.begin(), loose_dongles.end());
                    max_nodes_ = incidences_.size() + aux.size();
                }
                else
                    max_nodes_ = incidences_.size() + loose_dongles.size();
                i1->update_n_nodes(max_nodes_ + 1);
                i2->update_n_nodes(max_nodes_ + 1);
                auto ires = int_ki_cont::big_O_dot(i1, i2, {loose_dongles.begin(), loose_dongles.end()});
                interface_stack.push_back(ires);
            }
            auto res_top = interface_stack.back();
            denom_ = res_top->denom_;
            numers_ = res_top->numers_;
            numers_ext = res_top->numers_ext_;
        }
        else
        {
            denom_ = std::make_shared<KIIndexCol>(max_nodes_ + 1, KIIndexCol::com_denomnator);
            numers_ = std::make_shared<KIIndexCol>(max_nodes_ + 1, KIIndexCol::sum_numerators);
            numers_ext = std::make_shared<KIIndexCol>(max_nodes_ + 1, KIIndexCol::extern_connections);
        }
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
        cluster->use_threads_ = use_threads_;
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