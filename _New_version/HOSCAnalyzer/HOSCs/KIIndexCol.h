#ifndef __KIINDEXCOL_H__
#define __KIINDEXCOL_H__
#include <unordered_map>
#include <memory>
#include "UniqueHOSCCollections.h"
// #include "SymetricHOSC.h"
#include "SingleHOSC.h"

namespace HOSC
{


    class KIIndexCol
    {
    private:
        using unique_simplyHOSC_map = HOSCUniqueCollection<SingleHOSC>;
        unique_simplyHOSC_map _HOSCmap;
        // counter_maps _counters;
    public:
        KIIndexCol(/* args */)
        {
        }
        ~KIIndexCol() {}
        std::shared_ptr<SingleHOSC> insert(std::shared_ptr<SingleHOSC> &HOSC);
    };

} // namespace HOSC


#endif // __KIINDEXCOL_H__