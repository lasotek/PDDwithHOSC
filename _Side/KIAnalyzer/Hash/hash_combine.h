#ifndef __HASH_COMBINE_H__
#define __HASH_COMBINE_H__

// #pragma once

#include <cstddef>
#include <functional>

constexpr void HashCombine(std::size_t &seed, std::size_t hash_value)
{
    seed ^= hash_value + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}


#endif // __HASH_COMBINE_H__