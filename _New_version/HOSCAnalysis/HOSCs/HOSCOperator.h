#ifndef __HOSCOPERATOR_H__
#define __HOSCOPERATOR_H__
#include <memory>
#include <list>
#include "FullHOSC.h"

class HOSCOperator
{
private:
    struct rOperand
    {
        HOSC::nodes_to_remove _nodes;
        std::shared_ptr<HOSC::HOSC> _hosc;
        rOperand(std::shared_ptr<HOSC::HOSC> hosc, const HOSC::nodes_to_remove &nodes): _hosc(hosc), _nodes(nodes) {}
    };
    std::shared_ptr<HOSC::HOSC> _lHOSC;
    std::list<std::shared_ptr<rOperand>> _list_operations;

public:
    HOSCOperator();
    HOSCOperator(const std::shared_ptr<HOSC::HOSC>& init_HOSC);
    void AddOperand(std::shared_ptr<HOSC::HOSC> hosc, const HOSC::nodes_to_remove& nodes);
    std::shared_ptr<HOSC::HOSC> determine_result();
    // HOSC::nodes_to_remove get_global() const;
    ~HOSCOperator();
};

#endif // __HOSCOPERATOR_H__