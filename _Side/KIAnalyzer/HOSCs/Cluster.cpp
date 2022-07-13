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
        // to do
        std::set_difference(pRight->ext_my_pins_.begin(), pRight->ext_my_pins_.end(),
                            nodes_to_remove1.begin(), nodes_to_remove1.end(),
                            std::back_inserter(res->ext_my_pins_));

        return res;
    }

    KICluster::int_ki_cont::int_ki_cont(main_cluster_ptr cluster, const boundary_connections_list &connections) : clusters_p(cluster),
                                                                                                                  is_parallel_running(Parallel_running::not_yet),
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
        if (is_parallel_running == Parallel_running::running)
            return;
        clusters_p->Solve();
        NodeTrans::NtoN map;
        get_boundary_translation(map);
        denom() = get_denom_tr(map);
        numers() = get_numers_tr(map);
        numers_ext() = get_numers_ext_tr(map);
    }

    void KICluster::int_ki_cont::Solve2()
    {
        if (is_parallel_running == Parallel_running::running)
            return;
        clusters_p->Solve2();
        NodeTrans::NtoN map;
        get_boundary_translation(map);
        denom() = get_denom_tr(map);
        numers() = get_numers_tr(map);
        numers_ext() = get_numers_ext_tr(map);
    }

    void KICluster::int_ki_cont::SolveParallel() // to implement
    {
        if (is_parallel_running == Parallel_running::running)
            return;
        std::thread([this]
                    {
            this->is_parallel_running = Parallel_running::running;
            this->clusters_p->Solve();
            NodeTrans::NtoN map;
            this->get_boundary_translation(map);
            this->denom() = get_denom_tr(map);
            this->numers() = get_numers_tr(map);
            this->numers_ext() = get_numers_ext_tr(map);
            this->is_parallel_running = Parallel_running::done;
            this->promise.set_value(true); })
            .detach();
    }

    void KICluster::int_ki_cont::SolveParallel2()
    {
        if (is_parallel_running == Parallel_running::running)
            return;
        std::thread([this]
                    {
            this->is_parallel_running = Parallel_running::running;
            this->clusters_p->Solve2();
            NodeTrans::NtoN map;
            this->get_boundary_translation(map);
            this->denom() = get_denom_tr(map);
            this->numers() = get_numers_tr(map);
            this->numers_ext() = get_numers_ext_tr(map);
            // this->is_parallel_running = Parallel_running::done;
            this->promise.set_value(true); })
            .detach();
    }

    void KICluster::int_ki_cont::get_boundary_translation(NodeTrans::NtoN &map) const
    {
        clusters_p->get_boundary_translation(map);
        for (auto [ni, no] : map)
        {
            map[ni] = translator.inN2extN(no);
        }
        // to do
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

    KICluster::map_iterator KICluster::min_incidence()
    {
        return std::min_element(incidences_.begin(), incidences_.end(), [this](const auto &T1, const auto &T2)
                                {
            bool is_b_T1 = this->bound_nodes_.contains(T1.first);
            bool is_b_T2 = this->bound_nodes_.contains(T2.first);
            if (is_b_T1 == is_b_T2)
                return T1.second.size() < T2.second.size();
            else
                return is_b_T2; });
    }
    
    long long gcd(long long a, long long b)
    {
        if (b == 0)
            return a;
        return gcd(b, a % b);
    }

    void KICluster::to_stream(std::ostream &os)
    {
        if (denom_value_.has_value() && numer_value_.has_value())
        {
            long long denom = denom_value_.value();
            long long numer = numer_value_.value();
            long long int_value = numer / denom;
            long long reminder = numer % denom;
            if(reminder == 0)
                os << "KI = " << int_value << std::endl;
            else
            {
                auto cd = gcd(reminder, denom);
                os << "KI = " << int_value << " and " << reminder / cd  << " / " << denom / cd << std::endl;
                os << "or" << std::endl;
                os << "KI = " << (double)numer / denom << std::endl;
            }

        }
        else
        {
            os << "The cluster results:\n";
            os << "\tDenominator = " << denom_->String(node_trans_) << std::endl;
            os << "\tInternal Numerators = " << numers_->String(node_trans_) << std::endl;
            os << "\tExternal Numerators = " << numers_ext->String(node_trans_) << std::endl;
        }
    }

    void KICluster::get_incidence_with(int node, set_of_nodes &res)
    {
        auto my_edge_list = incidences_[node];
        for (auto &e : my_edge_list)
        {
            res.insert(e->get_node_a() == node ? e->get_node_b() : e->get_node_a());
        }
    }

    int KICluster::choose_com_incidence(const set_of_nodes &set)
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
        auto min_it = std::min_element(sets.begin(), sets.end(), [](const auto &T1, const auto &T2)
                                       { return T1.second.size() < T2.second.size(); });
        return *min_it->second.begin();
    }

    int KICluster::prepare_next_edges(const set_of_nodes &set, edges_list_ptr &newEdges)
    {
        // auto newNode = choose_com_incidence(set);
        auto newNode = choose_com_incidence2(set);
        auto &local_inc_eges = incidences_[newNode];
        for (auto &e : local_inc_eges)
        {
            if (set.contains(e->get_node_a()) || set.contains(e->get_node_b()))
                newEdges.push_back(e);
            // auto &edge = e->second
        }
        return newNode;
    }

    void KICluster::prepare_all_possible_incidence_nodes(const set_of_nodes &set, set_of_nodes &res) const
    {
        res.clear();
        for (auto n : set)
        {
            const auto edges_it = incidences_.find(n);
            if (edges_it == incidences_.end())
                throw std::invalid_argument("No edges! Impossible!!!!");
            const auto &edges_list = edges_it->second;
            for (auto edge_ptr : edges_list)
            {
                auto a = edge_ptr->get_node_a();
                auto b = edge_ptr->get_node_b();
                res.insert(a == n ? b : a);
            }
        }
    }

    void KICluster::prepare_set_of_nodes_that_reduce(const set_of_nodes &set, set_of_nodes_that_reduce &res) const
    {
        set_of_nodes all_pos_incidences;
        prepare_all_possible_incidence_nodes(set, all_pos_incidences);
        for (auto n : all_pos_incidences)
        {
            auto &entry = res[n];
            auto n_incidences_edges_it = incidences_.find(n);
            if (n_incidences_edges_it == incidences_.end())
                throw std::invalid_argument("No edges! Oooops!!!!");
            auto &n_incidences_edges = n_incidences_edges_it->second;
            for (auto &edg_ptr : n_incidences_edges)
            {
                auto a = edg_ptr->get_node_a();
                auto b = edg_ptr->get_node_b();
                auto other_n = a == n ? b : a;
                if (set.contains(other_n))
                    entry[other_n]++;
            }
        }
    }

    int KICluster::choose_com_incidence2(const set_of_nodes &set) const
    {
        int best_node = -1;
        int best_incidence_edges_left = std::numeric_limits<int>::max();
        int number_of_nodes_removal = 0;
        set_of_nodes_that_reduce all_nodes_that_reduces;
        prepare_set_of_nodes_that_reduce(set, all_nodes_that_reduces);
        for (auto &[m_node, ord_map] : all_nodes_that_reduces)
        {
            int local_number_of_nodes_removal = 0;
            int local_best_incidence_edges_left = best_incidence_edges_left;
            for (auto &[other_node, node_rem] : ord_map)
            {
                auto left_incidence = incidences_.find(other_node)->second.size() - node_rem;
                if (left_incidence < local_best_incidence_edges_left)
                {
                    local_best_incidence_edges_left = left_incidence;
                }
                if (left_incidence == 0)
                {
                    local_best_incidence_edges_left = 0;
                    local_number_of_nodes_removal++;
                }
            }
            bool is_better = (local_best_incidence_edges_left < best_incidence_edges_left) ||
                             (local_best_incidence_edges_left == 0 &&
                              local_number_of_nodes_removal > number_of_nodes_removal);
            if (is_better)
            {
                best_incidence_edges_left = local_best_incidence_edges_left;
                number_of_nodes_removal = local_number_of_nodes_removal;
                best_node = m_node;
            }
        }
        return best_node;
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
    // KICluster::KICluster(const std::list<int> &_boundary_nodes, bool use_threads) noexcept : use_threads_(use_threads)
    // {
    //     bound_nodes_.insert(virtual_node);
    //     for (auto n : _boundary_nodes)
    //     {
    //         n = node_trans_.extN2inN(n);
    //         bound_nodes_.insert(n);
    //     }
    // }

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

    bool KICluster::dirty()
    {
        // return !(denom_value_.has_value() && numer_value_.has_value());
        // bool i_am_dirty = !(denom_value_.has_value() && numer_value_.has_value());
        bool i_am_dirty = !(denom_ && numers_ && numers_ext);
        if (i_am_dirty)
        {
            denom_ = nullptr;
            numers_ = nullptr;
            numers_ext = nullptr;
            return true;
        }
        bool desc_dirty = false;
        if (clusters_p)
            for (auto &cluster : *clusters_p)
            {
                desc_dirty |= cluster.dirty();
            }
        if (desc_dirty)
            make_dirty();
        return desc_dirty;
    }

    void KICluster::Solve2()
    {
        if (!dirty())
            return;
        set_of_nodes Nodes_to_be_removed_as_first;
        set_of_nodes Nodes_with_edges;
        for (auto &e : main_list_)
        {
            auto a = node_trans_.extN2inN(e->get_node_a());
            auto b = node_trans_.extN2inN(e->get_node_b());
            Nodes_with_edges.insert(a);
            Nodes_with_edges.insert(b);
            auto newE = std::make_shared<Edge<W>>(a, b, e->get_weight());
            incidences_[a].push_back(newE);
            incidences_[b].push_back(newE);
            if (a != virtual_node)
                max_nodes_ = std::max(max_nodes_, a);
            if (b != virtual_node)
                max_nodes_ = std::max(max_nodes_, b);
        }
        if (clusters_p)
        {
            ext_interface_stack interface_stack;
            if (use_threads_)
            {
                // parallel
                // for (auto &Cluster : *clusters_p)
                // {
                //     Cluster.is_parallel_running = int_ki_cont::Parallel_running::not_yet;
                // }
                for (auto &Cluster : *clusters_p)
                {
                    // if (!Cluster.dirty())
                    // {
                    //     Cluster.is_parallel_running = int_ki_cont::Parallel_running::done;
                    //     Cluster.update_n_nodes(max_nodes_ + 1);
                    //     auto &interface_ptr = Cluster.interface_ptr;
                    //     interface_ptr->denom_ = denom_;
                    //     interface_ptr->numers_ = numers_;
                    //     interface_ptr->numers_ext_ = numers_ext;
                    //     interface_stack.push_back(interface_ptr);
                    //     continue;
                    // }
                    // else
                    Cluster.is_parallel_running = int_ki_cont::Parallel_running::not_yet;
                    Cluster.leave_only_internal_nodes(Nodes_with_edges);
                    Cluster.leave_only_internal_nodes(bound_nodes_);
                    Cluster.leave_external();
                    Cluster.SolveParallel2(); // change to thread and everything
                }
                auto num_of_clusters = clusters_p->size();
                while (!all_finished())
                {
                    for (auto &Cluster : *clusters_p)
                    {
                        if (Cluster.is_parallel_running != int_ki_cont::Parallel_running::done && Cluster.solve_future.wait_for(std::chrono::microseconds(100)) == std::future_status::ready)
                        {
                            Cluster.solve_future.get();
                            Cluster.update_n_nodes(max_nodes_ + 1);
                            auto p_interface = Cluster.get_interface();
                            // p_interface->p_my_ext_nodes = &bound_nodes_;
                            interface_stack.push_back(p_interface);
                            Cluster.is_parallel_running = int_ki_cont::Parallel_running::done;
                            // break;
                        }
                    }
                }
            }
            else
            {
                // serial
                for (auto &Cluster : *clusters_p)
                {
                    Cluster.leave_only_internal_nodes(Nodes_with_edges);
                    Cluster.leave_only_internal_nodes(bound_nodes_);
                    Cluster.leave_external();
                    Cluster.Solve2();
                    Cluster.update_n_nodes(max_nodes_ + 1);
                    auto p_interface = Cluster.get_interface();
                    // p_interface->p_my_ext_nodes = &bound_nodes_;
                    interface_stack.push_back(p_interface);
                }
            }

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
                            if (n != edge.get_node_a())
                            {
                                std::erase(incidences_[edge.get_node_a()], edge_ptr);
                                if (incidences_[edge.get_node_a()].size() == 0)
                                {
                                    if (!bound_nodes_.contains(edge.get_node_a()))
                                        loose_dongles.insert(edge.get_node_a());
                                    incidences_.erase(edge.get_node_a());
                                }
                            }
                            else
                            {
                                std::erase(incidences_[edge.get_node_b()], edge_ptr);
                                if (incidences_[edge.get_node_b()].size() == 0)
                                {
                                    if (!bound_nodes_.contains(edge.get_node_b()))
                                        loose_dongles.insert(edge.get_node_b());
                                    incidences_.erase(edge.get_node_b());
                                }
                            }
                            KIIndexCol::del_pair ed = std::make_pair(edge.get_node_a(), edge.get_node_b());
                            nodes_to_remove rn;
                            if (i == 0 && !bound_nodes_.contains(n))
                                rn.push_back(n);
                            i1->numers_->add_edge_remove_node(ed, rn, edge.get_weight());
                            i1->denom_->add_edge_remove_node(ed, rn, edge.get_weight());
                            i1->numers_ext_->add_edge_remove_node(ed, rn, edge.get_weight());
                        }
                        incidences_.erase(iter);
                    }
                }
#ifdef _INSIDE_HIERARCHY_
                std::cout << "Partial result after edge adding:\n";
                std::cout << "Denominator: " << i1->denom_->String(node_trans_) << std::endl;
                std::cout << "Numerator: " << i1->numers_->String(node_trans_) << std::endl;
                std::cout << "Numerator_ext: " << i1->numers_ext_->String(node_trans_) << std::endl;
#endif
                auto it2 = std::max_element(interface_stack.begin(), interface_stack.end(), [&loose_dongles](auto &v1, auto &v2)
                                            { return v1->num_of_incidents(loose_dongles) < v2->num_of_incidents(loose_dongles); });
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

            auto iter = min_incidence();
            auto init_edge_ptr = *iter->second.begin();
            set_of_nodes set_n{{init_edge_ptr->get_node_a()}, {init_edge_ptr->get_node_b()}};
            denom_ = std::make_shared<KIIndexCol>(*init_edge_ptr, max_nodes_ + 1, KIIndexCol::com_denominator);
            numers_ = std::make_shared<KIIndexCol>(*init_edge_ptr, max_nodes_ + 1, KIIndexCol::sum_numerators);
            numers_ext = std::make_shared<KIIndexCol>(*init_edge_ptr, max_nodes_ + 1, KIIndexCol::extern_connections);
            nodes_to_remove rn;
            std::erase(incidences_[init_edge_ptr->get_node_b()], init_edge_ptr);
            if (incidences_[init_edge_ptr->get_node_b()].empty())
            {
                set_n.erase(init_edge_ptr->get_node_b());
                incidences_.erase(init_edge_ptr->get_node_b());
                if (!bound_nodes_.contains(init_edge_ptr->get_node_b()))
                    rn.push_back(init_edge_ptr->get_node_b());
            }
            std::erase(incidences_[init_edge_ptr->get_node_a()], init_edge_ptr);
            if (incidences_[init_edge_ptr->get_node_a()].empty())
            {
                set_n.erase(init_edge_ptr->get_node_a());
                incidences_.erase(init_edge_ptr->get_node_a());
                if (!bound_nodes_.contains(init_edge_ptr->get_node_a()))
                    rn.push_back(init_edge_ptr->get_node_a());
            }
            while (!incidences_.empty())
            {
                edges_list_ptr local_list;
                auto newNode = prepare_next_edges(set_n, local_list);
                set_n.insert(newNode);
                numers_->add_to_with_virtual_node_update(*numers_ext, newNode);
                numers_ext->add_shortcut_remove_node(*denom_, {virtual_node, newNode}, {});
                for (auto edge_ptr : local_list)
                {
                    auto a = edge_ptr->get_node_a();
                    auto b = edge_ptr->get_node_b();
                    auto it_a = incidences_.find(a);
                    auto it_b = incidences_.find(b);
                    std::erase(it_a->second, edge_ptr);
                    std::erase(it_b->second, edge_ptr);
                    if (it_b->second.empty())
                    {
                        set_n.erase(b);
                        incidences_.erase(it_b);
                        bool do_not_remove = bound_nodes_.contains(b) || (bound_nodes_.empty() && incidences_.empty());
                        if (!do_not_remove)
                            rn.push_back(b);
                    }
                    if (it_a->second.empty())
                    {
                        set_n.erase(a);
                        incidences_.erase(it_a);
                        bool do_not_remove = bound_nodes_.contains(a) || (bound_nodes_.empty() && incidences_.empty());
                        if (!do_not_remove)
                            rn.push_back(a);
                    }
                    auto ed = std::make_pair(a, b);
                    denom_->add_edge_remove_node(ed, rn, edge_ptr->get_weight());
                    numers_->add_edge_remove_node(ed, rn, edge_ptr->get_weight());
                    numers_ext->add_edge_remove_node(ed, rn, edge_ptr->get_weight());
                    rn.clear();
                }
            }

            // denom_ = std::make_shared<KIIndexCol>(max_nodes_ + 1, KIIndexCol::com_denominator);
            // numers_ = std::make_shared<KIIndexCol>(max_nodes_ + 1, KIIndexCol::sum_numerators);
            // numers_ext = std::make_shared<KIIndexCol>(max_nodes_ + 1, KIIndexCol::extern_connections);
            // while (!incidences_.empty())
            // {
            //     auto iter = min_incidence();
            //     auto index = iter->first;
            //     auto i = iter->second.size() - 1;
            //     for (auto eit = iter->second.begin(); eit != iter->second.end(); eit++, i--)
            //     {
            //         auto edge_ptr = *eit;
            //         auto &edge = *edge_ptr;
            //         nodes_to_remove rn;
            //         if (index != edge.node_a)
            //         {
            //             std::erase(incidences_[edge.node_a], edge_ptr);
            //             if (incidences_[edge.node_a].empty() && !bound_nodes_.contains(edge.node_a))
            //                 rn.push_back(edge.node_a);
            //         }
            //         else
            //         {
            //             std::erase(incidences_[edge.node_b], edge_ptr);
            //             if (incidences_[edge.node_b].empty() && !bound_nodes_.contains(edge.node_b))
            //                 rn.push_back(edge.node_b);
            //         }
            //         KIIndexCol::del_pair ed = std::make_pair(edge.node_a, edge.node_b);
            //         if (i == 0 && !bound_nodes_.contains(index))
            //             rn.push_back(index);
            //         numers_->add_edge_remove_node(ed, rn, edge.weight_);
            //         denom_->add_edge_remove_node(ed, rn, edge.weight_);
            //         numers_ext->add_edge_remove_node(ed, rn, edge.weight_);
            //     }
            // #ifdef _DEBUG_TEST
            //                 std::cout << "Node: " << index << " was removed.\n";
            // #endif

            // incidences_.erase(iter);
            // }

            denom_value_ = denom_->value();
            numer_value_ = numers_->value();
        }
#ifdef _INSIDE_HIERARCHY_
        std::cout << "Nodes translator\n";
        for (auto &[i, e] : node_trans_.intN2extN_)
        {
            std::cout << '\t' << i << " -> " << e << std::endl;
        }
        std::cout << *this << std::endl;
#endif
        denom_value_ = denom_->value();
        numer_value_ = numers_->value();
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
            auto a = node_trans_.extN2inN(e->get_node_a());
            auto b = node_trans_.extN2inN(e->get_node_b());
            Nodes_with_edges.insert(a);
            Nodes_with_edges.insert(b);
            auto newE = std::make_shared<Edge<W>>(a, b, e->get_weight());
            incidences_[a].push_back(newE);
            incidences_[b].push_back(newE);
            if (a != virtual_node)
                max_nodes_ = std::max(max_nodes_, a);
            if (b != virtual_node)
                max_nodes_ = std::max(max_nodes_, b);
        }
        // nodes_pins Edges_pins(Nodes_with_edges.begin(), Nodes_with_edges.end());
        // Edges_pins.assign(Nodes_with_edges.begin(), Nodes_with_edges.end());
        ext_interface_stack interface_stack;
        if (clusters_p)
        {
            if (use_threads_)
            {
                // parallel
                for (auto &Cluster : *clusters_p)
                {
                    Cluster.is_parallel_running = int_ki_cont::Parallel_running::not_yet;
                }
                for (auto &Cluster : *clusters_p)
                {
                    Cluster.leave_only_internal_nodes(Nodes_with_edges);
                    Cluster.leave_only_internal_nodes(bound_nodes_);
                    Cluster.leave_external();
                    Cluster.SolveParallel(); // change to thread and everything
                }
                auto num_of_clusters = clusters_p->size();
                while (!all_finished())
                {
                    for (auto &Cluster : *clusters_p)
                    {
                        if (Cluster.is_parallel_running != int_ki_cont::Parallel_running::done && Cluster.solve_future.wait_for(std::chrono::microseconds(100)) == std::future_status::ready)
                        {
                            Cluster.update_n_nodes(max_nodes_ + 1);
                            auto p_interface = Cluster.get_interface();
                            // p_interface->p_my_ext_nodes = &bound_nodes_;
                            interface_stack.push_back(p_interface);
                            Cluster.is_parallel_running = int_ki_cont::Parallel_running::done;
                            Cluster.solve_future.get();
                            // break;
                        }
                    }
                }
            }
            else
            {
                // serial
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
                            if (n != edge.get_node_a())
                            {
                                std::erase(incidences_[edge.get_node_a()], edge_ptr);
                                if (incidences_[edge.get_node_a()].size() == 0)
                                {
                                    if (!bound_nodes_.contains(edge.get_node_a()))
                                        loose_dongles.insert(edge.get_node_a());
                                    incidences_.erase(edge.get_node_a());
                                }
                            }
                            else
                            {
                                std::erase(incidences_[edge.get_node_b()], edge_ptr);
                                if (incidences_[edge.get_node_b()].size() == 0)
                                {
                                    if (!bound_nodes_.contains(edge.get_node_b()))
                                        loose_dongles.insert(edge.get_node_b());
                                    incidences_.erase(edge.get_node_b());
                                }
                            }
                            KIIndexCol::del_pair ed = std::make_pair(edge.get_node_a(), edge.get_node_b());
                            nodes_to_remove rn;
                            if (i == 0 && !bound_nodes_.contains(n))
                                rn.push_back(n);
                            i1->numers_->add_edge_remove_node(ed, rn, edge.get_weight());
                            i1->denom_->add_edge_remove_node(ed, rn, edge.get_weight());
                            i1->numers_ext_->add_edge_remove_node(ed, rn, edge.get_weight());
                        }
                        incidences_.erase(iter);
                    }
                }
                auto it2 = std::max_element(interface_stack.begin(), interface_stack.end(), [&loose_dongles](auto &v1, auto &v2)
                                            { return v1->num_of_incidents(loose_dongles) < v2->num_of_incidents(loose_dongles); });
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
            denom_ = std::make_shared<KIIndexCol>(max_nodes_ + 1, KIIndexCol::com_denominator);
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
                    nodes_to_remove rn;
                    if (index != edge.get_node_a())
                    {
                        std::erase(incidences_[edge.get_node_a()], edge_ptr);
                        if (incidences_[edge.get_node_a()].empty() && !bound_nodes_.contains(edge.get_node_a()))
                            rn.push_back(edge.get_node_a());
                    }
                    else
                    {
                        std::erase(incidences_[edge.get_node_b()], edge_ptr);
                        if (incidences_[edge.get_node_b()].empty() && !bound_nodes_.contains(edge.get_node_b()))
                            rn.push_back(edge.get_node_b());
                    }
                    KIIndexCol::del_pair ed = std::make_pair(edge.get_node_a(), edge.get_node_b());
                    if (i == 0 && !bound_nodes_.contains(index))
                        rn.push_back(index);
                    numers_->add_edge_remove_node(ed, rn, edge.get_weight());
                    denom_->add_edge_remove_node(ed, rn, edge.get_weight());
                    numers_ext->add_edge_remove_node(ed, rn, edge.get_weight());
                }
#ifdef _DEBUG_TEST
                std::cout << "Node: " << index << " was removed.\n";
#endif

                incidences_.erase(iter);
            }
        }
        denom_value_ = denom_->value();
        numer_value_ = numers_->value();
#ifdef _INSIDE_HIERARCHY_
        std::cout << "Nodes translator\n";
        for (auto &[i, e] : node_trans_.intN2extN_)
        {
            std::cout << '\t' << i << " -> " << e << std::endl;
        }
        std::cout << *this << std::endl;
#endif
    }

    KICluster::int_ki_cont *KICluster::insert_cluster(const main_cluster_ptr cluster, const boundary_connections_list &connection_list)
    {
        init_clusters_list();
        boundary_connections_list local_list(connection_list);

        for (auto &node_pair : local_list)
        {
            node_pair.second = node_trans_.extN2inN(node_pair.second);
        }
        cluster->use_threads_ = use_threads_;
        clusters_p->emplace_back(int_ki_cont(cluster, local_list));
        make_dirty();
        return &(clusters_p->back());
    }

    KICluster::int_ki_cont *KICluster::insert_cluster(const main_cluster_ptr cluster, const init_boundary_connections_list &connection_list)
    {
        init_clusters_list();
        boundary_connections_list local_list(connection_list);

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
        auto ext_node = node_trans_.extN2inNcheck(node);
        if (ext_node < 0)
            return false;
        return bound_nodes_.contains(ext_node);
    }

} // namespace HOSC