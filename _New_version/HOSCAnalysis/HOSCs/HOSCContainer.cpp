#include "HOSCContainer.h"

HOSCContainer::~HOSCContainer()
{
}


HOSCContainer::HOSCContainer(const std::string &name, const NodeMaper& node_mapper): _name(name),
    _local_node_mapper(node_mapper), _no_nodes(node_mapper.size())
{
}

HOSCContainer::HOSCContainer(const std::shared_ptr<HOSC::HOSC> &initHOSC, const NodeMaper& node_mapper):
    _is_operation_defined(true), _local_node_mapper(node_mapper)
{
    //finish
    if (!_from_formula)
        _from_formula = std::make_shared<HOSCOperator>(initHOSC);
}

void HOSCContainer::InitOperations(const std::shared_ptr<HOSC::HOSC> &initHOSC, const NodeMaper &node_mapper)
{
    _local_node_mapper = node_mapper;
    _is_operation_defined = true;
    if (!_from_formula)
    {
        _from_formula = std::make_shared<HOSCOperator>(initHOSC);
    }
}

void HOSCContainer::determine_HOSC()
{
    if (_is_operation_defined)
    {
        _hosc = _from_formula->determine_result();
    }
    else
    {
        _hosc = std::make_shared<HOSC::HOSC>(_init_sgn, _del_set, _no_nodes);
        if (_nodes_to_remove.size())
        {
            auto empty = std::make_shared<HOSC::HOSC>(_no_nodes);
            auto new_hosc = _hosc->HOSC_big_dot(empty, _nodes_to_remove);
            _hosc = new_hosc;
        }
    }
    _local_node_mapper.remove_nodes(_nodes_to_remove);
}

void HOSCContainer::correct_nodes(const HOSC::nodes_to_remove &new_nodes)
{
    std::for_each(new_nodes.begin(), new_nodes.end(), [this](auto value)
                  { _nodes_to_remove.push_back(value); });
}