#include "KIIndexCol.h"
#include <exception>
namespace HOSC
{
    std::shared_ptr<SingleHOSC> KIIndexCol::insert(std::shared_ptr<SingleHOSC> &HOSC) 
    {
        auto uniqueHOSC = _HOSCmap.insert_unique(HOSC);
        if(uniqueHOSC != HOSC)
        {
            uniqueHOSC->combine(*HOSC);
        }
        return uniqueHOSC;
    }

} // namespace HOSC