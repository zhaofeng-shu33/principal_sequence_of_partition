#pragma once
#include <random>
#include <chrono>
#include <iostream>
#include <fstream>
#include "core/graph.h"
#include "core/oracles/graph_cut.h"
#include "core/dt.h"
#include "core/graph/info_cluster.h"
#include "core/io_utility.h"
#include "test/test_utility.h"

namespace demo {
    class Graph4PointTest : public testing::Test {
    protected:
        EdgeListTuple edge_list_tuple_1;
        submodular::DirectedGraphCutOracle<double>* dgc;
        virtual void SetUp() {
            construct_edge_list_tuple_4(edge_list_tuple_1);

            submodular::SimpleGraph<double> sg = submodular::make_dgraph(4, edge_list_tuple_1);

            dgc = new submodular::DirectedGraphCutOracle<double>(sg);

        }
    };
    class Gaussian2DGraph : public submodular::InfoCluster, public Gaussian2DGraphBase {
    public:
        Gaussian2DGraph(int np, double gamma = 1, double a[][2] = NULL) : Gaussian2DGraphBase(np, gamma, a)
        {
            this->num_points = np;
            sg = submodular::make_dgraph(num_points, edge_list_tuple);
        }
        std::vector<double>& get_x_pos_list() {
            return x_pos;
        }
        std::vector<double>& get_y_pos_list() {
            return y_pos;
        }
    };

}

