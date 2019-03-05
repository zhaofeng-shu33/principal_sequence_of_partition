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
        EdgeListFloat edge_list_float_1;
        submodular::DirectedGraphCutOracle<double>* dgc;
        virtual void SetUp() {
            construct_edge_list_float_4(edge_list_float_1);

            submodular::SimpleGraph<double> sg = submodular::make_dgraph(4, edge_list_float_1);

            dgc = new submodular::DirectedGraphCutOracle<double>(sg);

        }
    };
    class Gaussian2DGraphBase {
    public:
        using EdgeListFloat = std::vector<std::tuple<std::size_t, std::size_t, double>>;
        Gaussian2DGraphBase(int np, double gamma = 1, double a[][2] = NULL) :
            distribution(0, 1),
            _gamma(gamma)
        {
            if (a == NULL) {
                // generator num_points 2D points ~ N(0,1) located at different positions
                unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
                generator = std::default_random_engine(seed);

                for (int j = 0; j < 4; j++) {
                    for (int i = 0; i < np / 4; i++) {
                        x_pos.push_back(distribution(generator) + data_1[j][0]);
                        y_pos.push_back(distribution(generator) + data_1[j][1]);
                    }
                }
            }
            else {
                for (int j = 0; j < 4; j++) {
                    for (int i = 0; i < np / 4; i++) {
                        x_pos.push_back(a[j][0]);
                        y_pos.push_back(a[j][1]);
                    }
                }
            }
#ifdef _DEBUG
            //dump x_pos, y_pos
            std::ofstream fout("x_y_pos.txt");
            for (int j = 0; j < x_pos.size(); j++) {
                fout << '{' << x_pos[j] << ',' << y_pos[j] << '}' << ',' << std::endl;
            }
            fout.close();
#endif
            for (int i = 0; i < np; i++)
                for (int j = i + 1; j < np; j++) {
                    edge_list_float_1.push_back(std::make_tuple(i, j, compute_similarity(x_pos[i], y_pos[i], x_pos[j], y_pos[j])));
                }
        }
            
    private:
        // {src, dst, capacity}
        std::default_random_engine generator;
        std::normal_distribution<double> distribution;
        std::vector<double> x_pos;
        std::vector<double> y_pos;
        EdgeListFloat edge_list_float_1;
        double _gamma;
        int data_1[4][2] = { {3,3},{3,-3},{-3,-3},{-3,3} };
        //! use Gaussian similarity function $exp(-||p_1 - p_2||^2/2) $
        double compute_similarity(double x_1, double y_1, double x_2, double y_2) {
            return exp(-1.0 * _gamma* pow(x_1 - x_2, 2) - _gamma * pow(y_1 - y_2, 2));
        }
    };
    class Gaussian2DGraph : public submodular::InfoCluster {
    public:
        using EdgeListFloat = std::vector<std::tuple<std::size_t, std::size_t, double>>;
        Gaussian2DGraph(int np, double gamma = 1, double a[][2] = NULL) :
            distribution(0, 1),
            _gamma(gamma)
        {
            this->num_points = np;
            if(a == NULL){
                // generator num_points 2D points ~ N(0,1) located at different positions
                unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
                generator = std::default_random_engine(seed);

                for (int j = 0; j < 4; j++) {
                    for (int i = 0; i < num_points / 4; i++) {
                        x_pos.push_back(distribution(generator) + data_1[j][0]);
                        y_pos.push_back(distribution(generator) + data_1[j][1]);
                    }
                }
            }
            else {
                for (int j = 0; j < 4; j++) {
                    for (int i = 0; i < num_points / 4; i++) {
                        x_pos.push_back(a[j][0]);
                        y_pos.push_back(a[j][1]);
                    }
                }
            }
#ifdef _DEBUG
            //dump x_pos, y_pos
            std::ofstream fout("x_y_pos.txt");
            for (int j = 0; j < x_pos.size(); j++) {
                fout << '{' << x_pos[j] << ',' << y_pos[j] << '}' << ',' << std::endl;
            }
            fout.close();
#endif
            for (int i = 0; i < num_points; i++)
                for (int j = i + 1; j < num_points; j++) {
                    edge_list_float_1.push_back(std::make_tuple(i, j, compute_similarity(x_pos[i], y_pos[i], x_pos[j], y_pos[j])));
                }
            sg = submodular::make_dgraph(num_points, edge_list_float_1);
        }
        std::vector<double>& get_x_pos_list() {
            return x_pos;
        }
        std::vector<double>& get_y_pos_list() {
            return y_pos;
        }
    private:
        // {src, dst, capacity}
        std::default_random_engine generator;
        std::normal_distribution<double> distribution;
        std::vector<double> x_pos;
        std::vector<double> y_pos;
        EdgeListFloat edge_list_float_1;
        double _gamma;
        int data_1[4][2] = { {3,3},{3,-3},{-3,-3},{-3,3} };
        //! use Gaussian similarity function $exp(-||p_1 - p_2||^2/2) $
        double compute_similarity(double x_1, double y_1, double x_2, double y_2) {
            return exp(-1.0 * _gamma* pow(x_1 - x_2, 2) - _gamma * pow(y_1 - y_2, 2));
        }
    };

}

