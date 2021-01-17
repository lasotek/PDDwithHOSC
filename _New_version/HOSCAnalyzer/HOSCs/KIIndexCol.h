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
    public:
        using func = enum{com_denomnator, sum_numerators};
        KIIndexCol() = delete;
        /**
         * @brief Construct a new KIIndexCol object
         * 
         * @param _no_nodes number of nodes in graph
         * @param function type of set: com_denominator, sum_numerators
         */
        KIIndexCol(int _no_nodes, func function = com_denomnator);
        ~KIIndexCol() {}
        std::shared_ptr<SingleHOSC> insert(std::shared_ptr<SingleHOSC> &HOSC);
    };

} // namespace HOSC


#endif // __KIINDEXCOL_H__