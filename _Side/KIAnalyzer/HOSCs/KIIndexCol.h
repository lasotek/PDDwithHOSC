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

namespace HOSC
{
    class KIIndexCol : public std::enable_shared_from_this<KIIndexCol>
    {
    public:
        using KIndexCol_ptr = std::shared_ptr<KIIndexCol>;
        /**
         * @brief pair of nodes to be contracted. If empty, means no 
         * 
         */
        using del_pair = std::optional<std::pair<int, int>>;
        /**
         * @brief node to be removed. Is oprional
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
        KIIndexCol(int _no_nodes) noexcept;

    public:
        using func = enum { com_denomnator,
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
         * @brief return numercial value, prvided that the result is complate 
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
         * @param node Node to remove, if there is no edges left
         * @param weight Weight of egde (resitance). Default is 1
         * @return true if the last but one node was removed and each edges were analyzed
         * @return false otherwise
         */
        bool add_edge_remove_node(const del_pair &edge, const rem_node &node, long long weight = 1);
        /**
         * @brief reset collection
         * 
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
         * @param n_nodes 
         */
        void update_n_nodes(int n_nodes);
    };

    KIIndexCol::KIndexCol_ptr big_O_dot_Col(KIIndexCol::KIndexCol_ptr L, KIIndexCol::KIndexCol_ptr R, nodes_to_remove &nodes);

} // namespace HOSC

#endif // __KIINDEXCOL_H__