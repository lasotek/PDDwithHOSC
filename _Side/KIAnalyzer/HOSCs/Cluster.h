/**
 * @file Cluster.h
 * @author Sławomir Lasota  (lasotek@gmail.com)
 * @brief
 * @version 0.1
 * @date 2021-02-05
 *
 * @copyright Copyright (c) 2021
 *
 */
#ifndef __CLUSTER_H__
#define __CLUSTER_H__
#include <set>
#include <list>
#include <stack>
#include <limits>
#include <iostream>
#include <memory>
#include <thread>
#include <future>
#include <mutex>
#include "Edge.h"
#include "KIIndexCol.h"
#include <initializer_list>

namespace HOSC
{
    class KICluster : std::enable_shared_from_this<KICluster>
    {
    public:
        /**
         * @brief any set of nodes
         *
         */
        using set_of_nodes = std::set<int>;
        /**
         * @brief external nodes
         *
         */
        using boundary_nodes_list = std::initializer_list<int>;

        /**
         * @brief connection list: first - internal No node, second - external No node
         *
         */
        using init_boundary_connections_list = std::initializer_list<std::pair<int, int>>;

        /**
         * @brief connection list: first - internal No node, second - external No node
         *
         */
        using boundary_connections_list = std::list<std::pair<int, int>>;

        /**
         * @brief internal cluster
         *
         */
        using main_cluster_ptr = std::shared_ptr<KICluster>;

        /**
         * @brief first - out nodes, second.first - in nodes, second.second - counter
         *
         */
        using set_of_nodes_that_reduce = std::unordered_map<int, std::unordered_map<int, int>>;

        friend class int_ki_cont;
        class int_ki_cont
        {
            struct ext_interface
            {
                nodes_pins ext_my_pins_;
                nodes_pins only_my_pins_;
                // set_of_nodes *p_my_ext_nodes = nullptr;
                std::shared_ptr<KIIndexCol> denom_;
                std::shared_ptr<KIIndexCol> numers_;
                std::shared_ptr<KIIndexCol> numers_ext_;
                ext_interface() = default;
                ext_interface(ext_interface &&Source) = delete;
                ext_interface(const ext_interface &Source);
                void leave_external();
                template <Iterable T>
                int num_of_incidents(const T &pins)
                {
                    nodes_pins common;
                    std::set_intersection(ext_my_pins_.begin(), ext_my_pins_.end(),
                                          pins.begin(), pins.end(),
                                          std::back_inserter(common));
                    return common.size();
                }
                inline void update_n_nodes(int n_nodes)
                {
                    denom_->update_n_nodes(n_nodes);
                    numers_->update_n_nodes(n_nodes);
                    numers_ext_->update_n_nodes(n_nodes + 1);
                }
            };
            using ext_interface_ptr = std::shared_ptr<ext_interface>;
            ext_interface_ptr interface_ptr;
            static ext_interface_ptr big_O_dot(ext_interface_ptr pLeft,
                                               ext_interface_ptr pRight,
                                               nodes_pins nodes_to_remove = {});

            main_cluster_ptr clusters_p;
            NodeTrans translator;
            // nodes_pins only_my_pins_;
            nodes_pins &only_my_pins() { return interface_ptr->only_my_pins_; }
            nodes_pins &ext_my_pins() { return interface_ptr->ext_my_pins_; }
            enum class Parallel_running
            {
                not_yet,
                running,
                done
            };
            Parallel_running is_parallel_running = Parallel_running::not_yet;
            std::promise<bool> promise;
            std::future<bool> solve_future;

            // std::mutex p_mutex;
            // to do
            // std::shared_ptr<KIIndexCol> denom_;
            std::shared_ptr<KIIndexCol> &denom() { return interface_ptr->denom_; }
            // std::shared_ptr<KIIndexCol> numers_;
            std::shared_ptr<KIIndexCol> &numers() { return interface_ptr->numers_; }
            // std::shared_ptr<KIIndexCol> numers_ext_;
            std::shared_ptr<KIIndexCol> &numers_ext() { return interface_ptr->numers_ext_; }

            // std::unordered_map<int, int> int2ext, ext2int;
            friend class KICluster;
            std::shared_ptr<KIIndexCol> get_translated(std::shared_ptr<KIIndexCol> Origin, NodeTrans::NtoN &map);
            inline std::shared_ptr<KIIndexCol> get_denom_tr(NodeTrans::NtoN &map)
            {
                return get_translated(clusters_p->denom_, map);
            }
            inline std::shared_ptr<KIIndexCol> get_numers_tr(NodeTrans::NtoN &map)
            {
                return get_translated(clusters_p->numers_, map);
            }
            inline std::shared_ptr<KIIndexCol> get_numers_ext_tr(NodeTrans::NtoN &map)
            {
                return get_translated(clusters_p->numers_ext, map);
            }
            /**
             * @brief remove nodes from edges, leaves only private not connected nodes
             *
             * @param edge_nodes edge's nodes
             */
            template <Iterable T>
            void leave_only_internal_nodes(const T &edge_nodes)
            {
                if (only_my_pins().empty())
                    return;
                nodes_pins diff;
                std::set_difference(ext_my_pins().begin(), ext_my_pins().end(),
                                    edge_nodes.begin(), edge_nodes.end(),
                                    std::back_inserter(diff));
                only_my_pins() = diff;
            }
            void update_n_nodes(int n_nodes);
            inline void leave_external()
            {
                interface_ptr->leave_external();
            }

        public:
            using ext_interface_stack = std::list<ext_interface_ptr>;
            // int_ki_cont() = default;
            int_ki_cont(main_cluster_ptr cluster, const boundary_connections_list &connections);
            set_of_nodes get_cluster_ex_nodes() const;
            int translate_node(int clust_int_node);
            void get_boundary_translation(NodeTrans::NtoN &map) const;
            void Solve();
            void Solve2();
            void SolveParallel();
            void SolveParallel2();
            inline ext_interface_ptr get_interface() { return interface_ptr; }
            inline ext_interface_ptr const_get_interface() const { return std::make_shared<ext_interface>(*interface_ptr); }
            // const auto only_my_pins() const { return only_my_pins()}; }
            // main_cluster_ptr big_o_dot();
            bool dirty() { return clusters_p->dirty(); }
        };

    private:
        friend std::ostream &operator<<(std::ostream &os, KICluster &cluster);
        friend class int_ki_cont;
        using ext_interface_stack = int_ki_cont::ext_interface_stack;
        using W = long long;
        set_of_nodes bound_nodes_;
        using edges_list_ptr = std::list<std::shared_ptr<Edge<W>>>;
        using incidence_map = std::unordered_map<int, edges_list_ptr>;
        using map_iterator = incidence_map::iterator;
        using incidence_sets = std::unordered_map<int, std::set<int>>;
        NodeTrans node_trans_;
        incidence_map incidences_;
        int max_nodes_ = -1;
        std::optional<W> denom_value_ = {};
        std::optional<W> numer_value_ = {};
        edges_list_ptr main_list_;
        std::shared_ptr<KIIndexCol> denom_;
        std::shared_ptr<KIIndexCol> numers_;
        std::shared_ptr<KIIndexCol> numers_ext;
        using clusters_list = std::list<int_ki_cont>;
        std::unique_ptr<clusters_list> clusters_p;
        bool use_threads_ = false;
        inline bool all_finished()
        {
            if (clusters_p)
                for (auto &ic : *clusters_p)
                {
                    if (ic.is_parallel_running != int_ki_cont::Parallel_running::done)
                        return false;
                }
            return true;
        }

        void get_boundary_translation(NodeTrans::NtoN &map) const;
        map_iterator min_incidence();
        inline void make_dirty()
        {
            // dirty_ = true;
            // max_nodes_ = -1;
            denom_value_.reset();
            numer_value_.reset();
            // node_trans_.reset();
            denom_.reset();
            numers_.reset();
            numers_ext.reset();
        }
        bool dirty();
        void to_stream(std::ostream &os);
        void init_clusters_list()
        {
            if (!clusters_p)
                clusters_p = std::make_unique<clusters_list>();
        }
        void get_incidence_with(int node, set_of_nodes &res);

        int choose_com_incidence(const set_of_nodes &set);

        int prepare_next_edges(const set_of_nodes &set, edges_list_ptr &newEdges);

        void prepare_all_possible_incidence_nodes(const set_of_nodes &set, set_of_nodes &res) const;

        void prepare_set_of_nodes_that_reduce(const set_of_nodes &set, set_of_nodes_that_reduce &res) const;

        int choose_com_incidence2(const set_of_nodes &set) const;

    public:
        KICluster(bool use_threads = false) noexcept;
        KICluster(const boundary_nodes_list &_boundary_nodes, bool use_threads = false) noexcept;
        template <Iterable T>
        KICluster(const T &_boundary_nodes, bool use_threads = false) noexcept
        {
            bound_nodes_.insert(virtual_node);
            for (auto n : _boundary_nodes)
            {
                n = node_trans_.extN2inN(n);
                bound_nodes_.insert(n);
            }
        }

        ~KICluster();
        /**
         * @brief Add another boundary node
         *
         * @param node New nodes
         * @return true The node is new
         * @return false The not has been already declared
         */
        inline bool add_boundary_node(int node)
        {
            return bound_nodes_.insert(node_trans_.extN2inN(node)).second;
        }

        /**
         * @brief Get the bound nodes ext object
         *
         * @return set_of_nodes Set of boundary nodes as an integer
         */
        set_of_nodes get_bound_nodes_ext() const;
        /**
         * @brief Remove boundary node if it exists
         *
         * @param node Boundary node
         * @return true If node has been defined and was removed
         * @return false If node has not been defined at all
         */
        bool remove_boundary_node(int node);
        /**
         * @brief Construct a new KICluster object
         *
         * @param edges_list Initial list of edges
         */
        KICluster(const Edges<W> &edges_list)
        {
            for (auto &e : edges_list)
            {
                main_list_.emplace_back(std::make_shared<Edge<W>>(e));
            }
        }
        /**
         * @brief Add a single edge to cluster. It reset previous calculations
         *
         * @param edge
         */
        void AddEdge(const Edge<W> &edge)
        {
            main_list_.emplace_back(std::make_shared<Edge<W>>(edge));
            make_dirty();
        }
        /**
         * @brief Remove each edge named "name" from cluster. It reset previous calculations
         *
         * @param name name of edge.
         */
        void DeleteEdge(const std::string &name)
        {
            main_list_.remove_if([&name](auto &e)
                                 { return e->get_name() == name; });
            make_dirty();
        }

        bool IsEdge(const std::string &name) const
        {
            return std::any_of(main_list_.begin(), main_list_.end(),
                               [&name](auto &e)
                               { return e->get_name() == name; });
        }

        bool IsEdgeUnique(const std::string &name) const
        {
            auto pred =[&name](auto &e)
                { return e->get_name() == name; };
            auto it = std::find_if(main_list_.begin(), main_list_.end(), pred);
            if (it == main_list_.end())
                return false;
            return std::find_if(std::next(it,1), main_list_.end(), pred) == main_list_.end();
            
        }

        template <Arithmetic W>
        void Modify(const std::string &name, int a, int b, W value)
        {
            auto pred = [&name](auto &e)
                { return e->get_name() == name; };
            auto it = std::find_if(main_list_.begin(), main_list_.end(), pred);
            if (it == main_list_.end())
                throw std::runtime_error("Edge with name " + name + " not found");
            if (std::find_if(std::next(it), main_list_.end(), pred) != main_list_.end())
                throw std::runtime_error("Name:" + name + " is not unique. Only unique edges can be modified");
            (*it)->modify(a, b, value);
        }

        /**
         * @brief Force a cluster to be solved
         *
         */
        void Solve();

        /**
         * @brief
         *
         */
        void Solve2();
        /**
         * @brief Return the numerical KI as a double number. In case of cluster with boundary nodes, they are assumed to be not connected
         *
         * @return double KI
         */
        inline auto KIIndex() const { return (double)numer_value_.value() / denom_value_.value(); }
        /**
         * @brief
         *
         * @return auto
         */
        inline auto numerator() const { return numer_value_.value(); }
        /**
         * @brief
         *
         * @return auto
         */
        inline auto denominator() const { return denom_value_.value(); }

        /**
         * @brief
         *
         * @param cluster
         * @param connection_list
         * @return int_ki_cont*
         */
        int_ki_cont *insert_cluster(const main_cluster_ptr cluster, const boundary_connections_list &connection_list);

        /**
         * @brief
         *
         * @param cluster
         * @param connection_list
         * @return main_cluster_ptr
         */
        int_ki_cont *insert_cluster(const main_cluster_ptr cluster, const init_boundary_connections_list &connection_list);
        /**
         * @brief checking if node is defined as boundary
         *
         * @param node
         * @return true yes
         * @return false no
         */
        bool is_boundary_node(int node) const;
    };

    /**
     * @brief overloaded writing to stream operator
     *
     * @param os stream
     * @param cluster cluster to describe into stream,
     * @return std::ostream& stream
     */
    inline std::ostream &operator<<(std::ostream &os, KICluster &cluster)
    {
        cluster.to_stream(os);
        return os;
    }

    using cluster_ptr = std::shared_ptr<KICluster>;
    inline cluster_ptr new_cluster(const KICluster::boundary_nodes_list &_boundary_nodes, bool use_threads = false)
    {
        return std::make_shared<KICluster>(_boundary_nodes, use_threads);
    }
} // namespace HOSC

#endif // __CLUSTER_H__