/**
 * @file KIIndexCol.h
 * @author Sławomir Lasota  (lasotek@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2021-02-05
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#ifndef __KIINDEXCOL_H__
#define __KIINDEXCOL_H__
#include <unordered_map>
#include <memory>
#include <type_traits>
#include <list>
#include <unordered_map>
#include <map>
#include <optional>
#include <limits>
#include "UniqueHOSCCollections.h"
#include "SingleHOSC.h"
#include "Edge.h"

namespace HOSC
{
    class KIIndexCol : public std::enable_shared_from_this<KIIndexCol>
    {
    public:
        /**
         * @brief 
         * 
         */
        using KIndexCol_ptr = std::shared_ptr<KIIndexCol>;

        /**
         * @brief 
         * 
         */
        using del_pair = std::pair<int, int>;

            /**
         * @brief pair of nodes to be contracted. If empty, means no 
         * 
         */
        using del_pair_opt = std::optional<del_pair>;

        /**
         * @brief node to be removed. Is optional
         * 
         */
        using rem_node = std::optional<int>;

    private:
        using unique_simplyHOSC_map = HOSCUniqueCollection<SingleHOSC>;
        unique_simplyHOSC_map _HOSCmap1, _HOSCmap2;
        bool first_main = false;
        int max_nodes;
        // bool has_virtual_node = false;
        unique_simplyHOSC_map &_HOSCmap()
        {
            if (first_main)
                return _HOSCmap1;
            else
                return _HOSCmap2;
        }
        const unique_simplyHOSC_map &_HOSCmapConst() const
        {
            if (first_main)
                return _HOSCmap1;
            else
                return _HOSCmap2;
        }
        unique_simplyHOSC_map &dest_HOSCmap()
        {
            if (!first_main)
                return _HOSCmap1;
            else
                return _HOSCmap2;
        }
        inline void switch_maps()
        {
            first_main = !first_main;
        }

    public:
        using W = long long;

        using func = enum { com_denominator,
                            sum_numerators,
                            extern_connections };
        KIIndexCol() = delete;
        /**
         * @brief Construct a new KIIndexCol object
         * 
         * @param _no_nodes number of nodes in graph
         * @param function type of set: com_denominator, sum_numerators
         */
        KIIndexCol(int _no_nodes, func function) noexcept;
        KIIndexCol(int _no_nodes) noexcept;
        KIIndexCol(Edge<W> &in_edge, int _no_nodes, func function);
        KIIndexCol(const KIIndexCol &Source);
        KIIndexCol(KIIndexCol &&Source) = default;
        ~KIIndexCol() {}

        /**
         * @brief Inserts  new HOSC. If already exists, the coefficients are added and new HOSC is destroyed
         * 
         * @param HOSC shared pointer to a new HOSC
         * @return std::shared_ptr<SingleHOSC> If HOSC already exists, it return old HOSC. Otherwise an ew one.
         */
        std::shared_ptr<SingleHOSC> insert(std::shared_ptr<SingleHOSC> &HOSC);

        /**
         * @brief Test if result is complete, i.e. the last but one node was removed
         * 
         * @return true if so
         * @return false otherwise
         */
        inline bool complete() { return _HOSCmap().size() == 1 && _HOSCmap().complete(); }

        /**
         * @brief return numerical value, provided that the result is complete 
         * 
         * @return std::optional<long long> value if is complete, or empty o
         */
        std::optional<long long> value()
        {
            if (complete())
                return _HOSCmap().value<long long>();
            return {};
        }

        /**
         * @brief consider another edge in graph
         * 
         * @param edge Pair of nodes in edge. Can be empty. 
         * @param nodes Nodes to remove, if there is no edges left
         * @param weight Weight of edge (resistance). Default is 1
         * @return true if the last but one node was removed and each edges were analyzed
         * @return false otherwise
         */
        bool add_edge_remove_node(const del_pair_opt &edge, const nodes_to_remove &nodes, long long weight = 1);

        /**
         * @brief 
         * 
         * @param edge 
         * @param nodes 
         * @return true 
         * @return false 
         */
        bool add_shortcut_remove_node(const del_pair &edge, const nodes_to_remove &nodes);

        /**
         * @brief 
         * 
         * @param OtherIndexCol 
         * @param edge 
         * @param nodes 
         */
        void add_shortcut_remove_node(KIIndexCol &OtherIndexCol, const del_pair &edge, const nodes_to_remove &nodes);

        /**
         * @brief 
         * 
         * @param OtherIndexCol 
         * @param nodes 
         * @return true 
         * @return false 
         */
        bool big_O_dot(KIIndexCol &OtherIndexCol, nodes_to_remove &nodes);

        /**
         * @brief 
         * 
         * @param Right 
         * @return KIIndexCol& 
         */
        KIIndexCol &operator+=(KIIndexCol &Right);

        /**
         * @brief 
         * 
         * @param Right 
         * @param newNode 
         */
        void add_to_with_virtual_node_update(KIIndexCol &Right, int newNode);
        
        /**
         * @brief 
         * 
         * @param Right 
         */
        void add_to_with_virtual_node_removed(KIIndexCol &Right);

        /**
         * @brief reset collection
         * 
         */
        void reset();
        
        /**
         * @brief 
         * 
         * @param NodeTrans 
         * @return std::string 
         */
        std::string String(const NodeTrans &NodeTrans);
        
        /**
         * @brief 
         * 
         * @param tr_map 
         * @return std::shared_ptr<KIIndexCol> 
         */
        KIndexCol_ptr copy_col_with_node_translated(const NodeTrans::NtoN &tr_map);

        /**
         * @brief 
         * 
         * @param real_node real node to replace
         * @return KIndexCol_ptr new HOSC sum 
         */
        KIndexCol_ptr copy_vitual_to_real(int real_node);
        
        /**
         * @brief 
         * 
         * @param n_nodes 
         */
        void update_n_nodes(int n_nodes);
    };
    /**
     * @brief 
     * 
     * @param L 
     * @param R 
     * @param nodes 
     * @param extra_nodes Remove it!!!
     * @return KIIndexCol::KIndexCol_ptr 
     */
    KIIndexCol::KIndexCol_ptr big_O_dot_Col(KIIndexCol::KIndexCol_ptr L, KIIndexCol::KIndexCol_ptr R, nodes_to_remove &nodes);

} // namespace HOSC

#endif // __KIINDEXCOL_H__