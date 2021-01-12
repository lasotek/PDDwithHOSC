#ifndef __KIINDEXCOL_H__
#define __KIINDEXCOL_H__
#include <unordered_map>
#include "UniqueHOSCCollections.h"
#include "SymetricHOSC.h"

namespace HOSC
{
    class KIIndexCol
    {
    private:
        using unique_simplyHOSC_map = HOSCUniqueCollection<SymetricalHOSC>;
        using unique_iterator = unique_simplyHOSC_map::HOSC_unique_iterator;
        using counter_maps = std::unordered_map<unique_iterator,unsigned long>;    
        /* data */
    public:
        KIIndexCol(/* args */)
        {
        }
        ~KIIndexCol() {}
    };

} // namespace HOSC

#endif // __KIINDEXCOL_H__