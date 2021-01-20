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
    bool Removing_single_nodes::touch(int n)
    {
        auto it = map_.find(n);
        if (it != map_.end())
            return it->second = true;
        return false;
    }
    bool Removing_single_nodes::complete() const
    {
        for (auto &v : map_)
            if (!v.second)
                return false;
        return true;
    }

    int NodeTrans::extN2inN(int N)
    {
        auto oldNit = extN2intN_.find(N);
        if (oldNit == extN2intN_.end())
        {
            intN2extN_[curr_node_] = N;
            extN2intN_[N] = curr_node_;
            return curr_node_++;
        }
        else
            return oldNit->second;
    }

    int NodeTrans::inN2extN(int N)
    {
        auto oldNit = intN2extN_.find(N);
        if (oldNit == intN2extN_.end())
            return -1;
        else
            return oldNit->second;
    }
    
    void NodeTrans::reset() 
    {
        curr_node_=0;
        intN2extN_.clear();
        extN2intN_.clear();
    }

}; // namespace HOSC
