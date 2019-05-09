#pragma once
#include <vector>
#include <tuple>
#include <lemon/list_graph.h>
#include "core/graph.h"
#include "core/gaussian2Dcase.h"
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
        submodular::DirectedGraphCutOracle<double>* dgc;
		lemon::ListGraph g;
		lemon::ListGraph::EdgeMap<double> edge_map;
        Graph4PointTest(): edge_map(g){}
        virtual void SetUp() {
            construct_edge_list_tuple_4(edge_list_tuple_1);

            submodular::SimpleGraph<double> sg = submodular::make_dgraph(4, edge_list_tuple_1);
            submodular::make_graph(4,  edge_list_tuple_1,  g, edge_map);
            dgc = new submodular::DirectedGraphCutOracle<double>(sg);

        }
    };
}

