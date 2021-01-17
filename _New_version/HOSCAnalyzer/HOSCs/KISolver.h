#ifndef __KISOLVER_H__
#define __KISOLVER_H__

namespace HOSC
{
    template <Arithmetic W>
    struct Edge
    {
        int node_a;
        int node_b;
        W weight;
    };

    template <Arithmetic W>
    using Edges = std::initializer_list<Edge<W>>;

    template <Arithmetic W>
    class KISolver
    {
    private:
        std::list<Edge<W>> list_;
        std::unordered_map<int, int> counters;

    public:
        KISolver() = default;
    };
} // namespace HOSC
#endif // __KISOLVER_H__