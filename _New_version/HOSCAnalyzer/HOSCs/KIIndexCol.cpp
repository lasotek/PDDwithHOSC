#include "KIIndexCol.h"
#include <exception>
namespace HOSC
{
#ifdef _DEBUG_TEST
    void KCounter::combine(const KCounter &Other)
    {
        if(*_pHOSC != *(Other._pHOSC))
            throw std::invalid_argument("Combined counter should be the same");
        _value += Other._value;
    }
#endif
}