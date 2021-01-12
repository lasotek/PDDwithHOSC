#ifndef __KIINDEXCOL_H__
#define __KIINDEXCOL_H__
#include <unordered_map>
#include <memory>
#include "UniqueHOSCCollections.h"
#include "SymetricHOSC.h"

namespace HOSC
{

    class KCounter
    {
        std::shared_ptr<SymetricalHOSC> _pHOSC;
        long _value = 1;

    public:
        KCounter(const std::shared_ptr<SymetricalHOSC> &pHOSC) : _pHOSC(pHOSC) {}
        bool operator==(const std::shared_ptr<SymetricalHOSC> &Other) const { return *Other == *_pHOSC; }
        bool operator==(const KCounter &Other) const { return *Other._pHOSC == *_pHOSC; }
        auto value() const { return _value; }
        auto hash() const { return _pHOSC->hash(); }
#ifdef _DEBUG_TEST
        void combine(const KCounter &Other);
#else
        inline void combine(const KCounter &Other)
        {
            _value += Other._value;
        }
#endif
    };

    class KIIndexCol
    {
    private:
        using unique_simplyHOSC_map = HOSCUniqueCollection<KCounter>;
        unique_simplyHOSC_map _HOSCmap;
        // counter_maps _counters;
    public:
        KIIndexCol(/* args */)
        {
        }
        ~KIIndexCol() {}
    };

} // namespace HOSC

namespace std
{
    template <>
    struct hash<HOSC::KCounter>
    {
        size_t operator()(const HOSC::KCounter &o)
        {
            return o.hash();
        }
    };

} // namespace std

#endif // __KIINDEXCOL_H__