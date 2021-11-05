#include "SignedUniqueHOSCCollection.h"

namespace HOSC
{
    [[nodiscard]] SignedUniqueHOSCCollection::signed_component_type SignedUniqueHOSCCollection::sgn_insert_unique(component_type to_insert)
    {
        int sgn = to_insert->clean_sgn();
        auto res = insert_unique(to_insert);
        if (to_insert->sgn_ == -res->sgn_)
            sgn *= -1;
        return std::make_tuple(sgn, res);
    }

}; // namespace HOSC