#ifndef __FULLHOSC_H__
#define __FULLHOSC_H__

// #pragma once

#include "SingleDeletion.h"
#include "helpers/helpers.h"
#include "UniqueHOSCCollections.h"
#include <iostream>
#include <vector>
#include <optional>
#include <memory>
#include <functional>

namespace HOSC
{

    class SingleHOSC : public std::enable_shared_from_this<SingleHOSC>
    {
    public:
        using HOSC_oper_result = std::shared_ptr<SingleHOSC>;
        using initial_del_set = std::initializer_list<SingleDel>;

    private:
        using deletion_type = std::vector<SingleDel>;
        deletion_type _deletions_;
        int n_nodes_ = 0;
        int counter_ = 1;
        void insert_del(int source, int target);
        void reserve(int node);
        SingleHOSC &operator=(const SingleHOSC &Source) = default;

        // extracts multiplier different tha ±1, compare with pattern HOSC to math the sign
        friend class SignedUniqueHOSCCollection;
        template <typename MAP>
        void copy_to_replacer(Node_replacer_temp<MAP> &Target);
        template <typename MAP>
        void copy_from_replacer(const Node_replacer_temp<MAP> &Source);
        void Test();

    public:
        explicit SingleHOSC(int max_nodes = 10)
            : n_nodes_{max_nodes}
        {
            _deletions_.reserve(n_nodes_);
        }
        SingleHOSC(int source, int target, short max_nodes = 5);
        SingleHOSC(initial_del_set initial_dels, short max_nodes = 5);
        SingleHOSC(const SingleHOSC &Source) = default;
        SingleHOSC(SingleHOSC &&Source) = default;
        ~SingleHOSC()
        {
#ifdef _DEBUG_TEST
            std::cout << "Nodes No: " << n_nodes_ << ", HOSC: " << String() << " is deleting\n";
#endif
        }
        bool operator==(const SingleHOSC &Other) const;
        inline bool operator!=(const SingleHOSC &Other) const { return !operator==(Other); }
        inline void multiply(int multipolier) { counter_ *= multipolier; }
        inline bool is_valid() const { return counter_ != 0; }
        inline bool is_complete() const { return n_nodes_ == 0 && _deletions_.empty(); }
        std::string String() const;
        std::size_t hash() const;
        size_t del_numbers() const { return _deletions_.size(); }
        inline int counter() const { return counter_; }

        std::optional<int> numeric_value() const;
        [[nodiscard]] HOSC_oper_result HOSC_big_dot(HOSC_oper_result h2, const nodes_to_remove &nodes) const;
        int get_n_nodes() const { return n_nodes_; }
    };

    inline SingleHOSC::HOSC_oper_result HOSC_big_dot(SingleHOSC::HOSC_oper_result h1, SingleHOSC::HOSC_oper_result h2, const nodes_to_remove &nodes)
    {
        return std::move(h1->HOSC_big_dot(h2, nodes));
    }
    extern HOSCUniqueCollection<SingleHOSC> SingleHOSCCollection;
}; // namespace HOSC

namespace std
{
    template <>
    struct hash<HOSC::SingleHOSC>
    {
        size_t operator()(const HOSC::SingleHOSC &o)
        {
            return o.hash();
        }
    };

} // namespace std

#endif // __FULLHOSC_H__