#ifndef __UNIQUEHOSCCOLLECTIONS_H__
#define __UNIQUEHOSCCOLLECTIONS_H__

#include <unordered_map>
#include <memory>
#include <iterator>

namespace HOSC
{
    template <typename HOSCComp>
    class HOSCUniqueCollection
    {
    public:
        using component_type = std::shared_ptr<HOSCComp>;

    private:
        using map_type = std::unordered_multimap<size_t, component_type>;
        map_type map_;

    public:
        class HOSC_unique_iterator
        {
            map_type::iterator it_;

        public:
            HOSC_unique_iterator(const map_type::iterator &it) : it_(it) {}
            HOSC_unique_iterator(HOSC_unique_iterator &) = default;
            using iterator_category = map_type::iterator::iterator_category;
            // using value_type = component_type;
            using difference_type = map_type::iterator::difference_type;
            // using pointer = component_type *;
            // using reference = component_type &;
            component_type operator*() { return it_->second; }
            HOSC_unique_iterator operator++(int)
            {
                auto temp(*this);
                ++*this;
                return temp;
            }
            HOSC_unique_iterator &operator++()
            {
                ++it_;
                return *this;
            }
            HOSC_unique_iterator operator--(int)
            {
                auto temp(*this);
                --*this;
                return temp;
            }
            HOSC_unique_iterator &operator--()
            {
                --it_;
                return *this;
            }
            bool operator==(const HOSC_unique_iterator &Other) const { return it_ == Other.it_; }
            bool operator!=(const HOSC_unique_iterator &Other) const { return it_ != Other.it_; }
        };

        component_type insert_unique(component_type to_insert)
        {
            size_t hash = std::hash<HOSCComp>{}(*to_insert);
            auto [beg, en] = map_.equal_range(hash);
            for (auto it = beg; it != en; it++)
            {
                if (*(it->second) == *to_insert)
                {
                    return it->second;
                }
            }
            return map_.emplace(hash, to_insert)->second;
        }
        bool empty() const { return map_.empty(); }
        HOSC_unique_iterator begin() { return HOSC_unique_iterator(map_.begin()); }
        HOSC_unique_iterator end() { return HOSC_unique_iterator(map_.end()); }
    };
} // namespace HOSC

#endif // __UNIQUEHOSCCOLLECTIONS_H__