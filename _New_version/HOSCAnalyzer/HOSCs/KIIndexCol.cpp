#include "KIIndexCol.h"
#include <exception>
namespace HOSC
{
    KIIndexCol::KIIndexCol(int _no_nodes, func function) : max_nodes(_no_nodes)
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

        default:
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

} // namespace HOSC