#include "Cluster.h"

namespace HOSC
{
    void KICluster::to_stream(std::ostream &os)
    {
        os << "The cluster results:\n";
        os << "\tDenominator = " << denom_->String(node_trans_) << std::endl;
        os << "\tInternal Numerators = " << numers_->String(node_trans_) << std::endl;
        os << "\tExternal Numerators = " << numers_ext->String(node_trans_) << std::endl;
    }

    KICluster::KICluster()
    {
        bound_nodes_.insert(virtual_node);
    }

    KICluster::KICluster(const boundary_nodes_list &_boundary_nodes)
    {
        bound_nodes_.insert(virtual_node);
        for (auto n : _boundary_nodes)
        {
            n = node_trans_.extN2inN(n);
            bound_nodes_.insert(n);
        }
    }

    KICluster::~KICluster()
    {
    }

    bool KICluster::remove_boundary_node(int node)
    {
        auto it = bound_nodes_.find(node);
        if (it == bound_nodes_.end())
            return false;
        bound_nodes_.erase(it);
        return true;
    }
} // namespace HOSC