#pragma once
#include <random>
#include <chrono>
#include <iostream>
#include <fstream>
#include "psp/graph.h"
#include "psp/dt.h"
#include "psp/psp.h"
#include "psp/io_utility.h"
#include "psp/pmf.h"
namespace demo {
    typedef std::vector<std::tuple<std::size_t, std::size_t, double>> EdgeListTuple;
    class Gaussian2DGraphBase {
    public:
        Gaussian2DGraphBase(int np, double gamma = 1, double a[][2] = NULL);
        EdgeListTuple get_edge_list_tuple();
    protected:
        EdgeListTuple edge_list_tuple;
        std::vector<double> x_pos;
        std::vector<double> y_pos;
    private:
        // {src, dst, capacity}
        std::default_random_engine generator;
        std::normal_distribution<double> distribution;
        lemon::Tolerance<double> _tolerance;
        double _gamma;
        int data_1[4][2] = { {3,3},{3,-3},{-3,-3},{-3,3} };
        //! use Gaussian similarity function $exp(-||p_1 - p_2||^2/2) $
        double compute_similarity(double x_1, double y_1, double x_2, double y_2);
    };

    class Gaussian2DGraph : public Gaussian2DGraphBase, public submodular::PSP {
    public:
        Gaussian2DGraph(int np, double gamma = 1, double a[][2] = NULL);
        std::vector<double> get_x_pos_list();
        std::vector<double> get_y_pos_list();
    };

}

