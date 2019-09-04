#pragma once
#include <vector>
#include <tuple>
#include <utility>

#include <gtest/gtest.h>
#include <lemon/list_graph.h>

#include "set/set_stl.h"
#include "psp/graph.h"

namespace submodular {
    class SIMPLE_GRAPH : public testing::Test {
    protected:
        lemon::ListGraph g;
        lemon::ListGraph::EdgeMap<int> edge_map;
        SIMPLE_GRAPH(): edge_map(g){}
        virtual void SetUp() {
            std::vector<std::tuple<std::size_t, std::size_t, int>> edges;
            edges.push_back(std::make_tuple(0, 1, 1));
            edges.push_back(std::make_tuple(0, 2, 1));
            edges.push_back(std::make_tuple(1, 2, 5));
            make_graph(3, edges, g, edge_map);
        }
    };

    class SIMPLE_DGRAPH : public testing::Test {
    protected:
        lemon::ListDigraph g;
        lemon::ListDigraph::ArcMap<int> arc_map;
        SIMPLE_DGRAPH() : arc_map(g) {}
        virtual void SetUp() {
            std::vector<std::tuple<std::size_t, std::size_t, int>> arcs;
            arcs.push_back(std::make_tuple(0, 1, 1));
            arcs.push_back(std::make_tuple(0, 2, 1));
            arcs.push_back(std::make_tuple(1, 2, 5));
            make_dgraph(3, arcs, g, arc_map);
        }
    };
    TEST_F(SIMPLE_GRAPH, make_graph){

        EXPECT_EQ(g.maxNodeId(), 2);
        EXPECT_EQ(g.maxEdgeId(), 2);
        int edge_weight_count = 0;
        for (lemon::ListGraph::EdgeIt e(g); e != lemon::INVALID; ++e)
            edge_weight_count += edge_map[e];
        EXPECT_EQ(edge_weight_count, 7);    
    }

    TEST_F(SIMPLE_GRAPH, get_cut_value){
        stl::CSet _s;
        EXPECT_EQ(get_cut_value(g, edge_map, _s), 0);
        _s.AddElement(0);
        EXPECT_EQ(get_cut_value(g, edge_map, _s), 2);
        _s.AddElement(2);
        EXPECT_EQ(get_cut_value(g, edge_map, _s), 6);
        _s.AddElement(1);
        EXPECT_EQ(get_cut_value(g, edge_map, _s), 0);
    }

    TEST_F(SIMPLE_GRAPH, get_partition_value) {    
        EXPECT_EQ(get_partition_value(g, edge_map, stl::Partition::makeFine(3)), 7);
        EXPECT_EQ(get_partition_value(g, edge_map, stl::Partition::makeDense(3)), 0);
        stl::Partition _p;
        _p.AddElement(stl::CSet("100"));
        _p.AddElement(stl::CSet("011"));
        EXPECT_EQ(get_partition_value(g, edge_map, _p), 2);
    }

    TEST_F(SIMPLE_DGRAPH, make_dgraph) {
        EXPECT_EQ(g.maxNodeId(), 2);
        EXPECT_EQ(g.maxArcId(), 2);
        int arc_weight_count = 0;
        for (lemon::ListDigraph::ArcIt a(g); a != lemon::INVALID; ++a)
            arc_weight_count += arc_map[a];
        EXPECT_EQ(arc_weight_count, 7);
    }

    TEST_F(SIMPLE_DGRAPH, get_cut_value) {
        stl::CSet _s;
        EXPECT_EQ(get_cut_value(g, arc_map, _s), 0);
        _s.AddElement(2);
        EXPECT_EQ(get_cut_value(g, arc_map, _s), 6);
        _s.AddElement(0);
        EXPECT_EQ(get_cut_value(g, arc_map, _s), 5);
        _s.AddElement(1);
        EXPECT_EQ(get_cut_value(g, arc_map, _s), 0);
    }

    TEST_F(SIMPLE_DGRAPH, get_partition_value) {
        EXPECT_EQ(get_partition_value(g, arc_map, stl::Partition::makeFine(3)), 7);
        EXPECT_EQ(get_partition_value(g, arc_map, stl::Partition::makeDense(3)), 0);
        stl::Partition _p;
        _p.AddElement(stl::CSet("100"));
        _p.AddElement(stl::CSet("011"));
        EXPECT_EQ(get_partition_value(g, arc_map, _p), 2);
    }
}