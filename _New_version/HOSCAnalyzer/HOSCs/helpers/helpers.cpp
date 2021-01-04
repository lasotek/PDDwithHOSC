#include "helpers.h"

namespace HOSC
{
    // Removing_pair_nodes
    /**
     * @brief Construct a new Removing_pair_nodes::Removing_pair_nodes object
     * 
     * @param nodes 
     */
    Removing_pair_nodes::Removing_pair_nodes(const nodes_to_remove &nodes)
    {
        for (auto n : nodes)
            map_.emplace(std::make_pair(n, ack_entry()));
    }

    // Returns true if node is notified as to be removed
    bool Removing_pair_nodes::set_was_done_row(int node)
    {
        auto fit = map_.find(node);
        if (fit == map_.end())
            return false;
        fit->second.is_row_ = true;
        return true;
    }

    // Returns true if node is notified as to be removed
    bool Removing_pair_nodes::set_was_done_col(int node)
    {
        auto fit = map_.find(node);
        if (fit == map_.end())
            return false;
        fit->second.is_col_ = true;
        return true;
    }

    // Returns true if node is notified as to be removed
    bool Removing_pair_nodes::set_was_done_both(int node)
    {
        auto fit = map_.find(node);
        if (fit == map_.end())
            return false;
        fit->second.is_row_ = true;
        fit->second.is_col_ = true;
        return true;
    }

    // Returns false if there is some node untuched
    bool Removing_pair_nodes::is_complete() const
    {
        for (auto &[node, status] : map_)
            if (!status.is_col_ || !status.is_row_)
                return false;
        return true;
    }

    // Removing_single_nodes
    Removing_single_nodes::Removing_single_nodes(const std::vector<int> &Input)
    {
        for (auto n : Input)
            map_[n] = false;
    }
    void Removing_single_nodes::touch(int n)
    {
        auto it = map_.find(n);
        if (it != map_.end())
            it->second = true;
    }
    bool Removing_single_nodes::complete() const
    {
        for (auto &v : map_)
            if (!v.second)
                return false;
        return true;
    }

    // Node_replacer
    // int Node_replacer::get(int source) const
    // {
    //     auto it = map_.find(source);
    //     if (it == map_.end())
    //         return source;
    //     auto sgn = HOSC::sgn(it->second);
    //     return sgn * get(std::abs(it->second));
    // }

    // Node_linker
    // int Node_linker::get(int source) const
    // {
    //     auto it = map_.find(source);
    //     if (it == map_.end())
    //         return source;
    //     auto sgn = HOSC::sgn(it->second);
    //     return sgn * get(std::abs(it->second));
    // }

    // Node_replacer_opt
    void Node_replacer_opt::ensure_size(int index)
    {
        if (map_.size() < index)
            map_.resize(index);
    }

    void Node_replacer_opt::set(int source, int target)
    {
        ensure_size(source);
        map_[source] = target;
    }
    bool Node_replacer_opt::replaced(int source) const
    {
        if (source >= map_.size())
            return false;
        return map_[source].has_value();
    }
    int Node_replacer_opt::get(int source) const
    {
        if (source >= map_.size())
            return source;
        auto v = map_[source];
        if (!v.has_value())
            return source;
        auto sgn = HOSC::sgn(v.value());
        return sgn * get(std::abs(v.value()));
    }

}; // namespace HOSC
