#pragma once
#include <vector>
#include <tuple>
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
}

