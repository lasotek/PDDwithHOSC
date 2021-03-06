#include "SingleDeletion.h"
#include "../Hash/hash_combine.h"

namespace HOSC
{
    size_t SingleDel::hash() const
    {
        auto a = std::hash<int>{}(source_);
        auto b = std::hash<int>{}(target_);
        HashCombine(a, b);
        return a;
    }

}; // namespace HOSC
