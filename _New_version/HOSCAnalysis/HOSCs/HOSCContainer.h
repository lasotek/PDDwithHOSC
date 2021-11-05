#ifndef __HOSCCoNTAINER_H__
#define __HOSCCoNTAINER_H__

#include <string>
#include <memory>
#include "FullHOSC.h"
#include "HOSCOperator.h"
#include "helpers/NodeMaper.h"

class HOSCContainer
{
private:
    std::string _name;
    std::shared_ptr<HOSC::HOSC> _hosc;
    HOSC::HOSC::vec_del_set _del_set;
    int _init_sgn = 1;
    HOSC::nodes_to_remove _nodes_to_remove;
    int _no_nodes = 0;
    bool _is_operation_defined = false;
    std::shared_ptr<HOSCOperator> _from_formula;
    friend class Parser;
    void correct_nodes(const HOSC::nodes_to_remove& new_nodes);
    NodeMaper _local_node_mapper;

public:
    HOSCContainer(const std::string &name, const NodeMaper& node_mapper);
    HOSCContainer(const std::shared_ptr<HOSC::HOSC> &initHOSC, const NodeMaper& node_mapper);

    void InitOperations(const std::shared_ptr<HOSC::HOSC> &initHOSC, const NodeMaper &node_mapper);
    const auto& getHOSC() const { return _hosc; }
    const auto &getLocalMapper() { return _local_node_mapper;  }
    const auto &getName() { return _name; }

    ~HOSCContainer();
    void determine_HOSC();

    inline void set_multiplier(int multiplier) { _init_sgn = multiplier; }
    inline void add_node_to_remove(int node) { _nodes_to_remove.push_back(node); }
    inline void add_pair_of_deletions(int p, int r, int k, int l)
    {
        _del_set.emplace_back(std::pair(std::pair(p, r), std::pair(k, l)));
    }
    const HOSC::nodes_to_remove &nodes_to_remove() const
    {
        return _nodes_to_remove;
    }
};

#endif // __HOSCCoNTAINER_H__