#pragma once

#include "gtest/gtest.h"
#include "core/graph/maxflow.h"

#include <vector>
#include <tuple>
#include <utility>

namespace submodular {
    //! n: number of nodes
    //! s: source node id
    //! t: sink node id
    //! edges: vector of (u,v,cap)
    template <typename ValueType>
    MaxflowGraph<ValueType> st_graph(std::size_t n, std::size_t s, std::size_t t,
        const std::vector<std::tuple<std::size_t, std::size_t, ValueType>>& edges)
    {
        MaxflowGraph<ValueType> graph;
        auto m = edges.size();
        graph.Reserve(n, m);
        for (std::size_t i = 0; i < n; ++i) {
            graph.AddNode(i);
        }
        auto source = graph.GetNodeById(s);
        auto sink = graph.GetNodeById(t);

        for (std::size_t edge_id = 0; edge_id < m; ++edge_id) {
            std::size_t src, dst;
            ValueType cap;
            std::tie(src, dst, cap) = edges[edge_id];
            auto head = graph.GetNodeById(dst);
            auto tail = graph.GetNodeById(src);

            if (src == s && dst != t) {
                graph.AddSVArcPair(head, tail, cap, 0);
            }
            else if (src != s && dst == t) {
                graph.AddVTArcPair(head, tail, cap, 0);
            }
            else if (src != s && dst != t) {
                graph.AddArcPair(head, tail, cap, 0);
            }
            else {
                graph.AddSTOffset(cap);
            }
        }
        graph.MakeGraph(source, sink);

        return graph;
    }


    class MaxflowGraphTest : public testing::Test {
    protected:
        using EdgeListInt = std::vector<std::tuple<std::size_t, std::size_t, int>>;
        using EdgeListDouble = std::vector<std::tuple<std::size_t, std::size_t, double>>;

        // {src, dst, capacity}
        // source = 0, sink = 6
        // maxflow value = 10 + 3
        // mincut = {0, 1, 3}
        static constexpr std::size_t n_1 = 7;
        static constexpr std::size_t s_1 = 0;
        static constexpr std::size_t t_1 = 6;
        static constexpr std::size_t data_1[9][3] = {
            { 0, 1, 3 },
        { 0, 2, 1 },
        { 1, 3, 3 },
        { 2, 3, 5 },
        { 2, 4, 4 },
        { 4, 5, 2 },
        { 3, 6, 2 },
        { 5, 6, 3 },
        { 0, 6, 10 } // st arc
        };

        EdgeListInt edge_list_int_1;
        EdgeListDouble edge_list_double_1;

        virtual void SetUp() {
            for (const auto& sdc : data_1) {
                edge_list_int_1.push_back(std::make_tuple(sdc[0], sdc[1], (int)sdc[2]));
                edge_list_double_1.push_back(std::make_tuple(sdc[0], sdc[1], (double)sdc[2]));
            }
        }
    };

    constexpr std::size_t MaxflowGraphTest::n_1;
    constexpr std::size_t MaxflowGraphTest::s_1;
    constexpr std::size_t MaxflowGraphTest::t_1;
    constexpr std::size_t MaxflowGraphTest::data_1[9][3];
}