#ifndef __SYMETRICHOSC_H__
#define __SYMETRICHOSC_H__

#include "SingleDeletion.h"
#include "helpers/helpers.h"
#include <iostream>
#include <vector>
#include <optional>
#include <memory>
#include <functional>

namespace HOSC
{

    class SymetricalHOSC : public std::enable_shared_from_this<SymetricalHOSC>
    {
    private:
        using deletion_type = std::vector<SingleDel>;
        deletion_type deletions_;
        short n_nodes_ = 0;
        bool valid_ = true;
        void insert_del(int source, int target);
        void reserve(int node);
        SymetricalHOSC &operator=(const SymetricalHOSC &Source) = default;

    public:
        using HOSC_oper_result = std::shared_ptr<SymetricalHOSC>;
        using initial_del_set = std::initializer_list<SingleDel>;
        explicit SymetricalHOSC(short max_nodes = 10)
            : n_nodes_{max_nodes}
        {
            deletions_.reserve(n_nodes_);
        }
        SymetricalHOSC(int source, int target, short max_nodes = 5);
        SymetricalHOSC(initial_del_set initial_dels, short max_nodes = 5);
        SymetricalHOSC(const SymetricalHOSC &Source) = default;
        SymetricalHOSC(SymetricalHOSC &&Source) = default;
        ~SymetricalHOSC()
        {
#ifdef _DEBUG_TEST
            std::cout << String() << " is deleting\n";
#endif
        }
        inline bool is_valid() const { return valid_; }
        std::string String() const;
        std::size_t hash() const;
        size_t del_numbers() { return deletions_.size(); }
        bool operator==(const SymetricalHOSC &Other) const;
        inline bool operator!=(const SymetricalHOSC &Other) const { return !operator==(Other); }
        [[nodiscard]] HOSC_oper_result HOSC_big_dot(HOSC_oper_result h2, const nodes_to_remove &nodes);
    };

    inline SymetricalHOSC::HOSC_oper_result HOSC_big_dot(SymetricalHOSC::HOSC_oper_result h1, SymetricalHOSC::HOSC_oper_result h2, const nodes_to_remove &nodes)
    {
        return h1->HOSC_big_dot(h2, nodes);
    }
}; // namespace HOSC

namespace std
{
    template <>
    struct hash<HOSC::SymetricalHOSC>
    {
        size_t operator()(const HOSC::SymetricalHOSC &o)
        {
            return o.hash();
        }
    };

} // namespace std

#endif // __SYMETRICHOSC_H__