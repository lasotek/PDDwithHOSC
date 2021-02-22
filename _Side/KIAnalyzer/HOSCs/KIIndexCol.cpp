/**
 * @file KIIndexCol.cpp
 * @author Sławomir Lasota  (lasotek@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2021-02-05
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#include "KIIndexCol.h"
#include <exception>
#include <sstream>
namespace HOSC
{
    KIIndexCol::KIIndexCol(int _no_nodes) noexcept : max_nodes(_no_nodes)
    {
        // switch_maps();
    }

    KIIndexCol::KIIndexCol(int _no_nodes, func function) noexcept : max_nodes(_no_nodes)
    {
        switch (function)
        {
        case com_denomnator:
        {
            auto HOSC = std::make_shared<SingleHOSC>(_no_nodes);
            if (HOSC->is_valid())
                insert(HOSC);
        }
        break;

        case sum_numerators:
            for (int i = 0; i < _no_nodes; i++)
            {
                for (int j = i + 1; j < _no_nodes; j++)
                {
                    auto HOSC = std::make_shared<SingleHOSC>(j, i, _no_nodes);
                    if (HOSC->is_valid())
                        insert(HOSC);
                }
            }
            break;
        case extern_connections:
            for (int i = 0; i < _no_nodes; i++)
            {
                auto HOSC = std::make_shared<SingleHOSC>(virtual_node, i, _no_nodes);
                if (HOSC->is_valid())
                    insert(HOSC);
            }
            _no_nodes++;
            // has_virtual_node = true;
        }
        switch_maps();
    }

    KIIndexCol::KIIndexCol(const KIIndexCol &Source) : max_nodes(Source.max_nodes), first_main(false)
    {
        auto sHOSCmap = Source._HOSCmapConst();
        for (auto it = sHOSCmap.begin(); it != sHOSCmap.end(); it++)
        {
            auto HOSC = **it;
            auto new_shared_HOCS = std::make_shared<SingleHOSC>(HOSC);
            insert(new_shared_HOCS);
        }
        switch_maps();
    }

    std::shared_ptr<SingleHOSC> KIIndexCol::insert(std::shared_ptr<SingleHOSC> &HOSC)
    {
        auto uniqueHOSC = dest_HOSCmap().insert_unique(HOSC);
        if (uniqueHOSC != HOSC)
        {
            uniqueHOSC->combine(*HOSC);
        }
        return uniqueHOSC;
    }
    
    bool KIIndexCol::add_edge_remove_node(const del_pair &edge, const nodes_to_remove &nodes, long long weight) 
    {
        if (!edge.has_value() && nodes.empty())
            return false;
        auto &Source = _HOSCmap();
        auto &Dest = dest_HOSCmap();
        Dest.clear();
        SingleHOSC::HOSC_oper_result h1 = std::make_shared<SingleHOSC>(max_nodes);
        for (auto it = Source.begin(); it != Source.end(); it++)
        {
            auto hRes = h1->HOSC_big_dot(*it, nodes);
            if (hRes)
            {
                if (weight != 1)
                    *hRes *= weight;
                insert(hRes);
            }
        }
        if (edge.has_value())
        {
            auto pair = edge.value();
            SingleHOSC::HOSC_oper_result h0 = std::make_shared<SingleHOSC>(pair.first, pair.second, max_nodes);
            for (auto it = Source.begin(); it != Source.end(); it++)
            {
                auto hRes = h0->HOSC_big_dot(*it, nodes);
                if (hRes)
                {
                    insert(hRes);
                }
            }
        }
        max_nodes -= nodes.size();
        switch_maps();
        return Dest.size() == 1;
    }

    bool KIIndexCol::big_O_dot(KIIndexCol &OtherIndexCol, nodes_to_remove &nodes)
    {
        auto &Source = _HOSCmap();
        auto &Dest = dest_HOSCmap();
        Dest.clear();
        auto &OSource = OtherIndexCol._HOSCmap();
        for (auto iit = Source.begin(); iit != Source.end(); iit++)
        {
            auto h1 = *iit;
            for (auto oit = OSource.begin(); oit != OSource.end(); oit++)
            {
                auto h2 = *oit;
                auto hRes = h1->HOSC_big_dot(h2, nodes);
                if (hRes)
                {
                    insert(hRes);
                }
            }
        }
        max_nodes -= nodes.size();
        switch_maps();
        return Dest.size() == 1;
    }

    KIIndexCol &KIIndexCol::operator+=(KIIndexCol &Right)
    {
        switch_maps();
        // auto &Source = _HOSCmap();
        auto &OSource = Right._HOSCmap();
        for (auto it = OSource.begin(); it != OSource.end(); it++)
        {
            auto h2 = std::make_shared<SingleHOSC>(**it);
            insert(h2);
        }
        switch_maps();
        return *this;
    }

    void KIIndexCol::reset()
    {
    }

    std::string KIIndexCol::String(const NodeTrans &NodeTrans)
    {
        std::ostringstream ss;
        bool first = true;
        auto &map = _HOSCmap();
        for (auto it = map.begin(); it != map.end(); it++)
        {
            if (!first)
                ss << " + ";
            else
                first = false;
            auto h = *it;
            ss << h->String(&NodeTrans);
        }
        return ss.str();
    }

    KIIndexCol::KIndexCol_ptr KIIndexCol::copy_col_with_node_translated(const NodeTrans::NtoN &tr_map)
    {
        auto res = std::shared_ptr<KIIndexCol>(new KIIndexCol(max_nodes));
        auto CurHOSCs = _HOSCmap();
        for (auto it = CurHOSCs.begin(); it != CurHOSCs.end(); it++)
        {
            auto &HOSC = **it;
            auto newHOSC = std::make_shared<SingleHOSC>(HOSC, tr_map);
            res->insert(newHOSC);
        }
        res->switch_maps();
        // to do
        return res;
    }

    void KIIndexCol::update_n_nodes(int n_nodes)
    {
        max_nodes = n_nodes;
        auto CurHOSCs = _HOSCmap();
        for (auto it = CurHOSCs.begin(); it != CurHOSCs.end(); it++)
        {
            (*it)->update_n_nodes(n_nodes);
        }
    }

    KIIndexCol::KIndexCol_ptr big_O_dot_Col(KIIndexCol::KIndexCol_ptr L, KIIndexCol::KIndexCol_ptr R, nodes_to_remove &nodes)
    {
        auto h1 = std::make_shared<KIIndexCol>(*L);
        h1->big_O_dot(*R, nodes);
        return h1;
    }

} // namespace HOSC