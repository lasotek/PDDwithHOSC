#include "helpers/helpers.h"
#include "MultilevelHOSC.h"
#include <algorithm>

namespace HOSC
{
    void MultilevelHOSC::TryCompress()
    {
    }

    MultilevelHOSC::which_del MultilevelHOSC::compress(multi_hosc_item &al, multi_hosc_item &ar)
    {
        return none;
    }

    MultilevelHOSC::MultilevelHOSC(std::initializer_list<multi_hosc_item> InitList) noexcept : n_nodes_(-1)
    {
        data_.assign(InitList);
        if (std::any_of(data_.begin(), data_.end(), [this](const multi_hosc_item &item) {
                if (this->n_nodes_ == -1)
                {
                    this->n_nodes_ = item.second->get_n_nodes();
                }
                else
                {
                    if (this->n_nodes_ != item.second->get_n_nodes())
                        return true;
                }
                return false;
            }))
        {
            data_.clear();
        }
        else
        {
            {
                TryCompress();
            }
        }
    }
} // namespace HOSC