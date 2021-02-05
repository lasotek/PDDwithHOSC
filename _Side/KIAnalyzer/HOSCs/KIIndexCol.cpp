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
            has_virtual_node = true;
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

    bool KIIndexCol::add_edge_remove_node(const del_pair &edge, const rem_node &node, long long weight)
    {
        if (!edge.has_value() && !node.has_value())
            return false;
        auto &Source = _HOSCmap();
        auto &Dest = dest_HOSCmap();
        Dest.clear();
        nodes_to_remove ntr;
        if (node.has_value())
            ntr.push_back(node.value());
        SingleHOSC::HOSC_oper_result h1 = std::make_shared<SingleHOSC>(max_nodes);
        for (auto it = Source.begin(); it != Source.end(); it++)
        {
            auto hRes = h1->HOSC_big_dot(*it, ntr);
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
                auto hRes = h0->HOSC_big_dot(*it, ntr);
                if (hRes)
                {
                    insert(hRes);
                }
            }
        }
        switch_maps();
        return Dest.size() == 1;
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

} // namespace HOSC