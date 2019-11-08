#pragma once
// contains test fixture
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
        virtual void SetUp() {
            construct_edge_list_tuple_4(edge_list_tuple_1);

            submodular::make_dgraph(4,  edge_list_tuple_1,  g, edge_map);            

        }
    };

    class FourPointNotComplete : public testing::Test {
        protected:
            lemon::ListDigraph g;
            lemon::ListDigraph::ArcMap<double> arc_map;
            FourPointNotComplete(): arc_map(g){}
            virtual void SetUp() {
                lemon::ListDigraph::Node n1 = g.addNode();
                lemon::ListDigraph::Node n2 = g.addNode();
                lemon::ListDigraph::Node n3 = g.addNode();
                lemon::ListDigraph::Node n4 = g.addNode();
                lemon::ListDigraph::Arc a1 = g.addArc(n1, n2);
                lemon::ListDigraph::Arc a2 = g.addArc(n3, n4);
                arc_map[a1] = 1;
                arc_map[a2] = 1;                
            }
    }
}

