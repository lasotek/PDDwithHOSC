#include "KIIndexCol.h"
#include <exception>
namespace HOSC
{
    KIIndexCol::KIIndexCol(int _no_nodes, func function)
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
    }

    std::shared_ptr<SingleHOSC> KIIndexCol::insert(std::shared_ptr<SingleHOSC> &HOSC)
    {
        auto uniqueHOSC = _HOSCmap.insert_unique(HOSC);
        if (uniqueHOSC != HOSC)
        {
            uniqueHOSC->combine(*HOSC);
        }
        return uniqueHOSC;
    }

} // namespace HOSC