#ifndef __HELPERS_H__
#define __HELPERS_H__

// #pragma once
#include <map>
#include <vector>
#include <set>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <type_traits>
#include <memory>
#include <optional>

namespace HOSC
{
    template <typename T>
    T sgn(T t) { return t < 0 ? -1 : 1; }

    template <typename T>
    requires std::is_integral_v<T> bool is_odd(const T &n) { return (n & 1) != 0; }

    using nodes_to_remove = std::vector<int>;

    class Removing_pair_nodes
    {
    private:
        struct ack_entry
        {
            bool is_row_ = false;
            bool is_col_ = false;
        };
        using rem_map = std::map<int, ack_entry>;
        rem_map map_;

    public:
        Removing_pair_nodes(const nodes_to_remove &nodes);
        inline bool is_to_be_removed(int node) const { return map_.find(node) != map_.end(); }

        // Returns true if node is notified as to be removed
        bool set_was_done_row(int node);

        // Returns true if node is notified as to be removed
        bool set_was_done_col(int node);

        // Returns true if node is notified as to be removed
        bool set_was_done_both(int node);

        // Returns false if there is some node untuched
        bool is_complete() const;
    };
    
    class Removing_single_nodes
    {
    private:
        std::map<int, bool> map_;

    public:
        Removing_single_nodes() = default;
        Removing_single_nodes(const std::vector<int> &Input);
        Removing_single_nodes(const Removing_single_nodes &) = delete;
        Removing_single_nodes(Removing_single_nodes &&) = delete;
        auto operator=(const Removing_single_nodes &) = delete;
        auto operator=(Removing_single_nodes &&) = delete;
        void touch(int n);
        bool complete() const;
    };

    template <typename MAP>
    class Node_replacer_temp
    {
    protected:
        MAP map_;

    public:
        Node_replacer_temp() = default;
        Node_replacer_temp(const Node_replacer_temp &) = delete;
        Node_replacer_temp(Node_replacer_temp &&) = delete;
        auto operator=(const Node_replacer_temp &) = delete;
        auto operator=(Node_replacer_temp &&) = delete;
        bool operator==(const Node_replacer_temp &Other) const { return map_ == Other.map_; }
        inline void set(int source, int target)
        {
            map_[source] = target;
        }
        inline bool replaced(int source)
        {
            return map_.find(source) != map_.end();
        }
        int get(int source) const
        {
            auto it = map_.find(source);
            if (it == map_.end())
                return source;
            auto sgn = HOSC::sgn(it->second);
            return sgn * get(std::abs(it->second));
        }
        void clear() { map_.clear(); }
    };
    using Node_replacer = Node_replacer_temp<std::unordered_map<int, int>>;
    // class Node_replacer
    // {
    // private:
    //     std::unordered_map<int, int> map_;

    // public:
    //     Node_replacer() = default;
    //     Node_replacer(const Node_replacer &) = delete;
    //     Node_replacer(Node_replacer &&) = delete;
    //     auto operator=(const Node_replacer &) = delete;
    //     auto operator=(Node_replacer &&) = delete;
    //     bool operator==(const Node_replacer &Other) const { return map_ == Other.map_; }
    //     inline void set(int source, int target)
    //     {
    //         map_[source] = target;
    //     }
    //     inline bool replaced(int source)
    //     {
    //         return map_.find(source) != map_.end();
    //     }
    //     int get(int source) const;
    // };

    class Node_linker : public Node_replacer_temp<std::map<int, int>>
    {
        // private:
        //     std::map<int, int> map_;

    public:
        using similarity = enum { different = 0,
                                  equal,
                                  similar };
        using similarity_modifier = struct
        {
            std::shared_ptr<Node_replacer> new_left;
            std::shared_ptr<Node_replacer> new_right;
        };
        using similarity_res = std::tuple<similarity, std::optional<similarity_modifier>>;

        Node_linker() = default;
        Node_linker(const Node_linker &) = delete;
        Node_linker(Node_linker &&) = delete;
        auto operator=(const Node_linker &) = delete;
        auto operator=(Node_linker &&) = delete;
        bool operator==(const Node_linker &Other) const { return map_ == Other.map_; }
        // inline void set(int source, int target)
        // {
        //     map_[source] = target;
        // }
        // inline bool replaced(int source)
        // {
        //     return map_.find(source) != map_.end();
        // }
        // int get(int source) const;
    };

    class Node_replacer_opt
    {
    private:
        std::vector<std::optional<int>> map_;
        void ensure_size(int index);

    public:
        Node_replacer_opt() = default;
        Node_replacer_opt(const Node_replacer_opt &) = delete;
        Node_replacer_opt(Node_replacer_opt &&) = delete;
        auto operator=(const Node_replacer_opt &) = delete;
        auto operator=(Node_replacer_opt &&) = delete;
        void set(int source, int target);
        bool replaced(int source) const;
        int get(int source) const;
    };
};     // namespace HOSC
#endif // __HELPERS_H__