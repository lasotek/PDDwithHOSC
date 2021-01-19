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
    concept Arithmetic = std::is_arithmetic<T>::value;

    template <typename T>
    T sgn(T t) { return t < 0 ? -1 : 1; }

    /**
     * @brief Checking if 
     * 
     * @tparam T Any integral
     * @param n The numebr to check parity
     * @return bool - true if odd 
     */
    template <typename T>
    requires std::is_integral_v<T> bool is_odd(const T &n) { return (n & 1) != 0; }

    /**
     * @brief Vector of No nodes that have just been separated
     * 
     */
    using nodes_to_remove = std::vector<int>;

    /**
     * @brief Helper that note if the node has alredy been removed for rows and columns simultanously
     * 
     */
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
        /**
         * @brief Construct a new Removing_pair_nodes object
         * 
         * @param nodes a vector of nodes to be removed 
         */
        Removing_pair_nodes(const nodes_to_remove &nodes);


        Removing_pair_nodes() = delete;
        Removing_pair_nodes(const Removing_pair_nodes &) = delete;
        Removing_pair_nodes(Removing_pair_nodes &&) = delete;
        auto operator=(const Removing_pair_nodes &) = delete;
        auto operator=(Removing_pair_nodes &&) = delete;

        /**
         * @brief Checking if node is going to be removed
         * 
         * @param node Node
         * @return true if node is one of the node to be removed (does not metter if it was already removef)
         * @return false if node is not going to be removed, at all
         */
        inline bool is_to_be_removed(int node) const { return map_.find(node) != map_.end(); }

        /**
         * @brief Set the stat the node was removed for rows
         * 
         * @param node Node
         * @return true Node was on the list to be removed
         * @return false Node was not on the list. Nothing was notified
         */
        bool set_was_done_row(int node);

        /**
         * @brief Set the stat the node was removed for columns
         * 
         * @param node Node
         * @return true Node was on the list to be removed
         * @return false Node was not on the list. Nothing was notified
         */
        bool set_was_done_col(int node);

        // Returns true if node is notified as to be removed
        /**
         * @brief Set the stat the node was removed for both rows and columns
         * 
         * @param node Node
         * @return true Node was on the list to be removed
         * @return false Node was not on the list. Nothing was notified
         */
        bool set_was_done_both(int node);

        /**
         * @brief Checking if all nodes to be removed realy was removed
         * 
         * @return true Each node was removed
         * @return false Some nodes shoul be but they was not removed
         */
        bool is_complete() const;
    };

    /**
     * @brief Helper that note if the node has alredy been 
     * 
     */
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
        
        /**
         * @brief set that node has just beeing removed
         * 
         * @param n node
         * @return true if is in the list
         * @return false  if it is not - it is illegal
         */
        bool touch(int n);
        /**
         * @brief Check id all nodes to be removead are removed
         * 
         * @return true if so
         * @return false otherwise
         */
        bool complete() const;
    };

    /**
     * @brief Abstract Node Replacer
     * 
     * @tparam MAP type of of map
     */
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
        
        /**
         * @brief Set contraction of nodes
         * 
         * @param source source node
         * @param target target node
         */
        inline void set(int source, int target)
        {
            map_[source] = target;
        }

        /**
         * @brief CHeck if node was contracted
         * 
         * @param source node
         * @return true contracted
         * @return false not contracted
         */
        inline bool replaced(int source)
        {
            return map_.find(source) != map_.end();
        }

        /**
         * @brief Transform node to its current state
         * 
         * @param source node  
         * @return int current contrtaction of node
         */
        int get(int source) const
        {
            auto it = map_.find(source);
            if (it == map_.end())
                return source;
            auto sgn = HOSC::sgn(it->second);
            return sgn * get(std::abs(it->second));
        }
        /**
         * @brief reset
         * 
         */
        void clear() { map_.clear(); }
    };
    /**
     * @brief Actual Node-to-Node translater
     * 
     */
    using Node_replacer = Node_replacer_temp<std::unordered_map<int, int>>;

    /**
     * @brief Node_linker under development for similar HOSC addition
     * 
     */
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
    };

    // class Node_replacer_opt
    // {
    // private:
    //     std::vector<std::optional<int>> map_;
    //     void ensure_size(int index);

    // public:
    //     Node_replacer_opt() = default;
    //     Node_replacer_opt(const Node_replacer_opt &) = delete;
    //     Node_replacer_opt(Node_replacer_opt &&) = delete;
    //     auto operator=(const Node_replacer_opt &) = delete;
    //     auto operator=(Node_replacer_opt &&) = delete;
    //     void set(int source, int target);
    //     bool replaced(int source) const;
    //     int get(int source) const;
    // };
};     // namespace HOSC
#endif // __HELPERS_H__