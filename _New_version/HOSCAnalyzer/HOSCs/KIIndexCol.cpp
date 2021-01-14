#include "KIIndexCol.h"
#include <exception>
namespace HOSC
{
#ifdef _DEBUG_TEST
    void KCounter::combine(const KCounter &Other)
    {
        if (*_pHOSC != *(Other._pHOSC))
            throw std::invalid_argument("Combined counter should be the same");
        _value += Other._value;
    }

    std::shared_ptr<KCounter> KIIndexCol::insert(std::shared_ptr<KCounter> &&pKICounter)
    {
        auto uniqKI = _HOSCmap.insert_unique(pKICounter);
        if(uniqKI != pKICounter)
        {
            uniqKI->combine(*pKICounter);
        }
        return uniqKI;
    }

#endif
} // namespace HOSC