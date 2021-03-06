/**
 * @file SingleHOSC.h
 * @author Sławomir Lasota  (lasotek@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2021-02-05
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#ifndef __SINGLEHOSC_H__
#define __SINGLEHOSC_H__

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
        long long weight_ = 1;
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
        explicit SingleHOSC(int max_nodes, long long weight = 1) noexcept
            : n_nodes_{max_nodes}
        {
            _deletions_.reserve(n_nodes_);
        }
        explicit SingleHOSC(int source, int target, short max_nodes, long long weight = 1) noexcept;
        explicit SingleHOSC(initial_del_set initial_dels, short max_nodes, long long weight = 1) noexcept;
        explicit SingleHOSC(const SingleHOSC &Source) : n_nodes_(Source.n_nodes_), weight_(Source.weight_), _deletions_(Source._deletions_) {}
        explicit SingleHOSC(const SingleHOSC &Source, const NodeTrans::NtoN &trans_map);

        /**
         * @brief Construct a new SingleHOSC object ewplacing virtual node
         * 
         * @param Source 
         * @param newNode 
         */
        explicit SingleHOSC(const SingleHOSC &Source, int newNode);

        /**
         * @brief Construct a new Single H O S C object
         * 
         * @param Source 
         * @param renove_virtual sentinel arbitrary
         */
        explicit SingleHOSC(const SingleHOSC &Source, bool renove_virtual);

        SingleHOSC(SingleHOSC &&Source) = delete;
        ~SingleHOSC()
        {
#ifdef _DEBUG_TEST
            std::cout << "HOSC: " << String() << " is deleting\n";
#endif
        }
        void update_n_nodes(int n_nodes) { n_nodes_ = n_nodes; }
        bool operator==(const SingleHOSC &Other) const;
        inline bool operator!=(const SingleHOSC &Other) const { return !operator==(Other); }
        inline void multiply(int multipolier) { weight_ *= multipolier; }
        inline bool is_valid() const { return weight_ != 0; }
        inline bool is_complete() const { return _deletions_.empty(); }
        std::string String(const NodeTrans *pTranslater = nullptr) const;
        std::size_t hash() const;
        size_t del_numbers() const { return _deletions_.size(); }
        inline long long weight() const { return weight_; }

        std::optional<int> numeric_value() const;
        [[nodiscard]] HOSC_oper_result HOSC_big_dot(HOSC_oper_result h2, const nodes_to_remove &nodes) const;
        int get_n_nodes() const { return n_nodes_; }
        SingleHOSC &operator*=(long long mult)
        {
            weight_ *= mult;
            return *this;
        }
#ifdef _DEBUG_TEST
        void combine(const SingleHOSC &right);
#else
        inline void combine(const SingleHOSC &right)
        {
            weight_ += right.weight_;
        }
#endif
    };

    inline SingleHOSC::HOSC_oper_result HOSC_big_dot(SingleHOSC::HOSC_oper_result h1, SingleHOSC::HOSC_oper_result h2, const nodes_to_remove &nodes)
    {
        return std::move(h1->HOSC_big_dot(h2, nodes));
    }
    // extern HOSCUniqueCollection<SingleHOSC> SingleHOSCCollection;
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

#endif // __SINGLEHOSC_H__