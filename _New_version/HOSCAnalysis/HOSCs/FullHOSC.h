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

    class HOSC : public std::enable_shared_from_this<HOSC>
    {
    public:
        using HOSC_oper_result = std::shared_ptr<HOSC>; //std::shared_ptr<HOSC>;
        using initial_del_set = std::initializer_list<std::pair<SingleDel, SingleDel>>;
        using vec_del_set = std::vector<std::pair<SingleDel, SingleDel>>;

    private:
        using deletion_type = std::vector<SingleDel>;
        deletion_type row_deletions_, col_deletions_;
        int n_nodes_ = 0;
        int sgn_ = 1;
        int insert_del_to_vector(int source, int target, deletion_type &vector);
        inline int insert_del_to_row(int source, int target) { return insert_del_to_vector(source, target, row_deletions_); }
        inline int insert_del_to_col(int source, int target) { return insert_del_to_vector(source, target, col_deletions_); }
        void reserve(int node);
        HOSC &operator=(const HOSC &Source) = default;

        // extracts multiplier different tha ±1, compare with pattern HOSC to math the sign
        [[nodiscard]] int clean_sgn();
        friend class SignedUniqueHOSCCollection;
        using del_set = enum { rows,
                               columns };
        template <typename MAP>
        void copy_to_replacer(Node_replacer_temp<MAP> &Target, del_set which_one);
        template <typename MAP>
        void copy_from_replacer(const Node_replacer_temp<MAP> &Source, del_set which_one);
        void Test();

    public:
        explicit HOSC(int max_nodes = 10)
            : n_nodes_{max_nodes}
        {
            row_deletions_.reserve(n_nodes_);
            col_deletions_.reserve(n_nodes_);
        }
        HOSC(int sgn, int rsource, int rtarget, int csource, int ctarget, short max_nodes = 5);
        // HOSC(int sgn, initial_del_set initial_dels, short max_nodes = 5);
        HOSC(int sgn, const vec_del_set &initial_dels, short max_nodes = 5);
        HOSC(const HOSC &Source) = default;
        HOSC(HOSC &&Source) = default;
        ~HOSC()
        {
#ifdef _DEBUG_TEST
            std::cout << "Nodes No: " << n_nodes_ << ", HOSC: " << String() << " is deleting\n";
#endif
        }
        bool operator==(const HOSC &Other) const;
        inline bool operator!=(const HOSC &Other) const { return !operator==(Other); }
        inline void multiply(int multipolier) { sgn_ *= multipolier; }
        inline bool is_valid() const { return sgn_ != 0; }
        inline bool is_complete() const { return n_nodes_ == 0 && row_deletions_.empty() && col_deletions_.empty(); }
        std::string String(const std::vector<std::string>* pMaper = nullptr) const;
        std::size_t hash() const;
        size_t del_numbers() const { return row_deletions_.size(); }
        inline int get_sgn() const { return sgn_; }
        inline int weight() const { return sgn_; }

        [[nodiscard]] int move_sgn();

        std::optional<int> numeric_value() const;
        [[nodiscard]] HOSC_oper_result HOSC_big_dot(HOSC_oper_result h2, const nodes_to_remove &nodes) const;
        int get_n_nodes() const { return n_nodes_; }
    };

    inline HOSC::HOSC_oper_result HOSC_big_dot(HOSC::HOSC_oper_result h1, HOSC::HOSC_oper_result h2, const nodes_to_remove &nodes)
    {
        return std::move(h1->HOSC_big_dot(h2, nodes));
    }
    extern HOSCUniqueCollection<HOSC> HOSCCollection;
}; // namespace HOSC

namespace std
{
    template <>
    struct hash<HOSC::HOSC>
    {
        size_t operator()(const HOSC::HOSC &o)
        {
            return o.hash();
        }
    };

} // namespace std

#endif // __FULLHOSC_H__