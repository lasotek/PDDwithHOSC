#ifndef __SIGNEDUNIQUEHOSCCOLLECTION_H__
#define __SIGNEDUNIQUEHOSCCOLLECTION_H__
#include "UniqueHOSCCollections.h"
#include "FullHOSC.h"
#include <tuple>

namespace HOSC
{
    class SignedUniqueHOSCCollection : public HOSCUniqueCollection<HOSC>
    {
    private:
        inline component_type insert_unique(component_type to_insert)
        {
            return HOSCUniqueCollection<HOSC>::insert_unique(to_insert);
        }

    public:
        using signed_component_type = std::tuple<int /* multiplier */, component_type /* Pattern HOSc */>;
        [[nodiscard]] signed_component_type sgn_insert_unique(component_type to_insert);
    };

} // namespace HOSC

#endif // __SIGNEDUNIQUEHOSCCOLLECTION_H__