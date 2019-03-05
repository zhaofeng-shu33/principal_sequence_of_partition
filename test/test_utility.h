#pragma once
#include <vector>
#include <tuple>
#include <gtest/gtest.h>
namespace demo {
    typedef std::vector<std::tuple<std::size_t, std::size_t, double>> EdgeListFloat;
    void construct_edge_list_float_4(EdgeListFloat& edges) {
        double edge_1_value = 1.0;
        double edge_d_1_value = 0.5;
        edges.push_back(std::make_tuple(0, 1, edge_1_value));
        edges.push_back(std::make_tuple(1, 2, edge_1_value));
        edges.push_back(std::make_tuple(2, 3, edge_1_value));
        edges.push_back(std::make_tuple(0, 3, edge_1_value));
        edges.push_back(std::make_tuple(0, 2, edge_d_1_value));
        edges.push_back(std::make_tuple(1, 3, edge_d_1_value));
    }
}