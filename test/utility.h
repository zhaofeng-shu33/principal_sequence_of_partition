#pragma once
// contains test fixture
#include <gtest/gtest.h>
#include <vector>
#include <tuple>
#include <lemon/list_graph.h>
#include "psp/graph.h"
#include "gaussian2Dcase.h"

namespace demo {
void construct_edge_list_tuple_4(EdgeListTuple& edges) {
    double edge_1_value = 1.0;
    double edge_d_1_value = 0.5;
    edges.push_back(std::make_tuple(0, 1, edge_1_value));
    edges.push_back(std::make_tuple(1, 2, edge_1_value));
    edges.push_back(std::make_tuple(2, 3, edge_1_value));
    edges.push_back(std::make_tuple(0, 3, edge_1_value));
    edges.push_back(std::make_tuple(0, 2, edge_d_1_value));
    edges.push_back(std::make_tuple(1, 3, edge_d_1_value));
}

class Graph4PointTest : public testing::Test {
protected:
    EdgeListTuple edge_list_tuple_1;
    lemon::ListDigraph g;
    lemon::ListDigraph::ArcMap<double> edge_map;
    Graph4PointTest(): edge_map(g){}
    void SetUp() override {
        construct_edge_list_tuple_4(edge_list_tuple_1);
        submodular::make_dgraph(4,  edge_list_tuple_1,  g, edge_map);
    }
};

class FourPointNotComplete : public testing::Test {
    protected:
        lemon::ListDigraph g;
        lemon::ListDigraph::ArcMap<double> arc_map;
        FourPointNotComplete(): arc_map(g){}
        void SetUp() override {
            lemon::ListDigraph::Node n1 = g.addNode();
            lemon::ListDigraph::Node n2 = g.addNode();
            lemon::ListDigraph::Node n3 = g.addNode();
            lemon::ListDigraph::Node n4 = g.addNode();
            lemon::ListDigraph::Arc a1 = g.addArc(n1, n2);
            lemon::ListDigraph::Arc a2 = g.addArc(n3, n4);
            arc_map[a1] = 1;
            arc_map[a2] = 1;                
        }
};

class ThreePointComplete : public testing::Test {
    protected:
        std::vector<std::tuple<std::size_t, std::size_t, double>> edges;
        std::vector<double> lambda_list;
        std::vector<parametric::Partition> partition_list;
        void SetUp() override {
            edges.push_back(std::make_tuple(0, 1, 1.0));
            edges.push_back(std::make_tuple(0, 2, 1.0));
            edges.push_back(std::make_tuple(1, 2, 5.0));
        }
        void TearDown() override {
            std::vector<double>::iterator it = lambda_list.begin();
            EXPECT_DOUBLE_EQ(*it, 2);
            it++;
            EXPECT_DOUBLE_EQ(*it, 5);
            EXPECT_EQ(lambda_list.size(), 2);
            EXPECT_EQ(partition_list.size(), 3);

            stl::Partition p = stl::Partition::makeDense(3);
            std::vector<parametric::Partition>::iterator it_p = partition_list.begin();
            EXPECT_EQ(*it_p, p);

            it_p++;
            p.clear();
            p.AddElement(stl::CSet(std::string("011")));
            p.AddElement(stl::CSet(std::string("100")));
            EXPECT_EQ(*it_p, p);

            p = stl::Partition::makeFine(3);
            it_p++;
            EXPECT_EQ(*it_p, p);            
        }
};

}

