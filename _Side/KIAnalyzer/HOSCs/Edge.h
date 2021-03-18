/**
 * @file Edge.h
 * @author Sławomir Lasota  (lasotek@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2021-02-05
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#ifndef __EDGE_H__
#define __EDGE_H__
#include <memory>
#include <iostream>
#include "helpers/helpers.h"

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
        Edge(int a, int b, W weight = 1) noexcept : node_a(a), node_b(b), weight_(weight) {}
        Edge(const Edge &Source) = default;
        ~Edge()
        {
#ifdef _DEBUG_TEST
            std::cout << "Edge - nodes : " << node_a << " and " << node_b << ", weight " << weight_ << " is removing\n";
#endif
        }
        // template <Arithmetic W>
        // friend std::ostream &operator<<(std::ostream &os, Edge<W> &edge);
    };

    /**
     * @brief Defaul list initializer
     * 
     * @tparam W Should be arithmetic
     */
    template <Arithmetic W>
    using Edges = std::initializer_list<Edge<W>>;

} // namespace HOSC

#endif // __EDGE_H__