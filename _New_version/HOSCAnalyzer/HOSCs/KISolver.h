#ifndef __KISOLVER_H__
#define __KISOLVER_H__
#include <list>
#include <memory>
#include <unordered_map>
#include <algorithm>
#include "KIIndexCol.h"

namespace HOSC
{
    /**
     * @brief Edge of the graph
     * 
     * @tparam W the type of weight. Should be arithemtic type.
     */
    template <Arithmetic W>
    struct Edge : public std::enable_shared_from_this<Edge<W>>
    {
        int node_a;
        int node_b;
        W weight_;
        /**
         * @brief Construct a new Edge object
         * 
         * @param a Node a
         * @param b Node b
         * @param weight Weight of the edge
         */
        Edge(int a, int b, W weight = 1) : node_a(a), node_b(b), weight_(weight) {}
        Edge(const Edge &Source) = default;
        ~Edge()
        {
#ifdef _DEBUG_TEST
            std::cout << "Nodes :" << node_a << " and " << node_b << ", weight " << weight_ << " is removing\n";
#endif
        }
    };

    /**
     * @brief Defaul list initializer
     * 
     * @tparam W Should be arithmetic
     */
    template <Arithmetic W>
    using Edges = std::initializer_list<Edge<W>>;

    /**
     * @brief Solver
     * 
     * @tparam W Type of the wieight. Arithmetic
     */
    template <Arithmetic W>
    class KISolver
    {
        private:
        using edges_list_ptr = std::list<std::shared_ptr<Edge<W>>>;
        using incidence_map = std::unordered_map<int, edges_list_ptr>;
        using map_iterator = incidence_map::iterator;
        incidence_map incidences_;
        int max_nodes_ = 0;
        edges_list_ptr main_list_;
        std::shared_ptr<KIIndexCol> denom_;
        std::shared_ptr<KIIndexCol> numers_;
        map_iterator min_incidence()
        {
            return std::min_element(incidences_.begin(), incidences_.end(), [](const auto &T1, const auto &T2) {
                return T1.second.size() < T2.second.size();
            });
        }

    public:
        KISolver() = default;
        KISolver(const Edges<W> &edges_list)
        {
            for (auto &e : edges_list)
            {
                main_list_.emplace_back(std::make_shared<Edge<W>>(e));
            }
        }
        void Solve()
        {
            for (auto &e : main_list_)
            {
                // auto E = e.shared_from_this(); // std::make_shared<Edge<W>>(e);
                // auto E =  std::make_shared<Edge<W>>(e);
                auto a = e->node_a;
                auto b = e->node_b;
                incidences_[a].push_back(e);
                incidences_[b].push_back(e);
                max_nodes_ = std::max(max_nodes_, a);
                max_nodes_ = std::max(max_nodes_, b);
            }
            while (!incidences_.empty())
            {
                auto iter = min_incidence();
                auto index = iter->first;
                for (auto eit = iter->second.begin(); eit != iter->second.end(); eit++)
                {
                    auto edge_ptr = *eit;
                    auto &edge = *edge_ptr;
                    if (index != edge.node_a)
                        std::erase(incidences_[edge.node_a], edge_ptr);
                    else
                        std::erase(incidences_[edge.node_b], edge_ptr);
                }
                incidences_.erase(iter);
            }
        }
    };
} // namespace HOSC
#endif // __KISOLVER_H__