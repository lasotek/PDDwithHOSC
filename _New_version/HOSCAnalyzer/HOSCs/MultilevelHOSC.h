#ifndef __MULTILEVELHOSC_H__
#define __MULTILEVELHOSC_H__

#include <vector>
#include <memory>
#include <tuple>
#include <optional>
#include <limits>
#include "FullHOSC.h"

namespace HOSC
{
    class MultilevelHOSC
    {
    public:
        using multi_hosc_item = std::pair<short, std::shared_ptr<HOSC>>;
        using multi_hosc = std::vector<multi_hosc_item>;

    private:
        /* data */
        multi_hosc data_;
        void TryCompress();
        short n_nodes_;

    protected:
        using which_del = enum { none = 0,
                                 left,
                                 right };
        using return_hosc_item = std::tuple<std::optional<multi_hosc_item>, which_del>;
        which_del compress(multi_hosc_item &al, multi_hosc_item &ar);

    public:
        MultilevelHOSC(int n_nodes = -1) noexcept: n_nodes_(n_nodes) {}
        MultilevelHOSC(std::initializer_list<multi_hosc_item> InitList) noexcept;
        ~MultilevelHOSC() {}
        auto IsValid() const { return !data_.empty(); }
    };

} // namespace HOSC

#endif // __MULTILEVELHOSC_H__