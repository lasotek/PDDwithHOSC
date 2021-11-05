#include <algorithm>
#include "HOSCOperator.h"

HOSCOperator::HOSCOperator()
{
}

HOSCOperator::HOSCOperator(const std::shared_ptr<HOSC::HOSC> &init_HOSC) : _lHOSC(init_HOSC)
{
}

void HOSCOperator::AddOperand(std::shared_ptr<HOSC::HOSC> hosc, const HOSC::nodes_to_remove &nodes)
{
    _list_operations.emplace_back(std::make_shared<rOperand>(hosc, nodes));
}

HOSCOperator::~HOSCOperator()
{
}

std::shared_ptr<HOSC::HOSC> HOSCOperator::determine_result()
{
    if(_lHOSC)
        for (auto &oper : _list_operations)
        {
            _lHOSC = _lHOSC->HOSC_big_dot(oper->_hosc, oper->_nodes);
        }
    return _lHOSC;
}
// HOSC::nodes_to_remove HOSCOperator::get_global() const
// {

// }
