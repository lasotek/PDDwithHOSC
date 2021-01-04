#ifndef __SINGLEDELTION_H__
#define __SINGLEDELTION_H__

// #pragma once
#include <vector>
#include <type_traits>

namespace HOSC
{

    class SingleDel
    {
    private:
        int source_ = 0;
        int target_ = 0;

        friend class SymetricalHOSC;
        friend class HOSC;

    public:
        SingleDel(int source, int target) noexcept : source_(source), target_(target) {}
        SingleDel(const std::pair<int, int> &del) noexcept : source_(del.first), target_(del.second) {}
        SingleDel(const SingleDel &Source) = default;
        SingleDel() = default;

        ~SingleDel() {}
        bool operator==(const SingleDel &Other) const { return source_ == Other.source_ && target_ == Other.target_; }
        bool invalid() const { return source_ == target_; }
        int source() const { return source_; }
        int target() const { return target_; }
        size_t hash() const;
    };

}; // namespace HOSC

namespace std
{
    template <>
    struct hash<HOSC::SingleDel>
    {
        size_t operator()(const HOSC::SingleDel &o)
        {
            return o.hash();
        }
    };

} // namespace  std

#endif // __SINGLEDELTION_H__