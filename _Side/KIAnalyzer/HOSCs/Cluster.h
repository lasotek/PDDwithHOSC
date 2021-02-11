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
        using init_bounadry_connections_list = std::initializer_list<std::pair<int, int>>;

        /**
         * @brief connection list: first - internal No node, second - external No node
         * 
         */
        using bounadry_connections_list = std::list<std::pair<int, int>>;

        /**
         * @brief internal cluster 
         * 
         */
        using mine_cluster_ptr = std::shared_ptr<KICluster>;

        class int_ki_cont
        {
            struct ext_interface
            {
                nodes_pins only_my_pins_;
                std::shared_ptr<KIIndexCol> denom_;
                std::shared_ptr<KIIndexCol> numers_;
                std::shared_ptr<KIIndexCol> numers_ext_;
                ext_interface() = default;
                ext_interface(ext_interface &&Source) = delete;
                ext_interface(const ext_interface &Source);
            };
            using ext_interface_ptr = std::shared_ptr<ext_interface>;
            ext_interface_ptr interface_ptr;
            static ext_interface_ptr big_O_dot(ext_interface_ptr pLeft, ext_interface_ptr pRight);

            mine_cluster_ptr clusters_p;
            NodeTrans translator;
            // nodes_pins only_my_pins_;
            nodes_pins &only_my_pins() { return interface_ptr->only_my_pins_; }
            bool is_parallel_running;
            std::future<bool> solve_future;
            // std::mutex p_mutex;
            //to do
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
             * @brief remove nodes froma edges, leaves only private not connected nodes
             * 
             * @param edge_nodes Egdge's nodes
             */
            // void leave_only_internal_nodes(const nodes_pins& edge_nodes);
            // void leave_only_internal_nodes(const set_of_nodes& edge_nodes);
            template <Iterable T>
            void leave_only_internal_nodes(const T &edge_nodes)
            {
                if (only_my_pins().empty())
                    return;
                nodes_pins diff;
                std::set_difference(only_my_pins().begin(), only_my_pins().end(),
                                    edge_nodes.begin(), edge_nodes.end(),
                                    std::back_inserter(diff));
                only_my_pins() = diff;
            }
            void update_n_nodes(int n_nodes);

        public:
            using ext_interface_stack = std::stack<ext_interface_ptr>;
            // int_ki_cont() = default;
            int_ki_cont(mine_cluster_ptr cluster, const bounadry_connections_list &connections);
            set_of_nodes get_cluster_ex_nodes() const;
            int translate_node(int clust_int_node);
            void get_boundary_translation(NodeTrans::NtoN &map) const;
            void Solve();
            void SolveParallel();
            inline ext_interface_ptr get_interface() { return interface_ptr; }
            inline ext_interface_ptr const_get_interface() const { return std::make_shared<ext_interface>(*interface_ptr); }
            // const auto only_my_pins() const { return only_my_pins()}; }
            // mine_cluster_ptr big_o_dot();
        };

    private:
        friend std::ostream &operator<<(std::ostream &os, KICluster &claster);
        friend class int_ki_cont;
        using ext_interface_stack = int_ki_cont::ext_interface_stack;
        using W = long long;
        set_of_nodes bound_nodes_;
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
        using clusters_list = std::list<int_ki_cont>;
        std::unique_ptr<clusters_list> clusters_p;
        bool use_threads_ = false;
        void get_boundary_translation(NodeTrans::NtoN &map) const;
        set_of_nodes get_bound_nodes_ext() const;
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
        void init_clusters_list()
        {
            if (!clusters_p)
                clusters_p = std::make_unique<clusters_list>();
        }

    public:
        KICluster(bool use_threads = false) noexcept;
        KICluster(const boundary_nodes_list &_boundary_nodes, bool use_threads = false) noexcept;
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
         * @brief Force a cluster to be solved
         * 
         */
        void Solve();
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
         * @return mine_cluster_ptr 
         */
        int_ki_cont *insert_cluster(const mine_cluster_ptr cluster, const init_bounadry_connections_list &connection_list);
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
     * @brief overloaded writting to stream operator
     * 
     * @param os stream
     * @param cluster claster to describe into strea, 
     * @return std::ostream& stream
     */
    inline std::ostream &operator<<(std::ostream &os, KICluster &cluster)
    {
        cluster.to_stream(os);
        return os;
    }

    using cluster_ptr = std::shared_ptr<KICluster>;
    inline cluster_ptr new_cluster(const KICluster::boundary_nodes_list &_boundary_nodes)
    {
        return std::make_shared<KICluster>(_boundary_nodes);
    }
} // namespace HOSC

#endif // __CLUSTER_H__