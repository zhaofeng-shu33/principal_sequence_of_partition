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
    protected:
        EdgeListFloat edge_list_float_1;
        std::vector<double> x_pos;
        std::vector<double> y_pos;
    private:
        // {src, dst, capacity}
        std::default_random_engine generator;
        std::normal_distribution<double> distribution;
        double _gamma;
        int data_1[4][2] = { {3,3},{3,-3},{-3,-3},{-3,3} };
        //! use Gaussian similarity function $exp(-||p_1 - p_2||^2/2) $
        double compute_similarity(double x_1, double y_1, double x_2, double y_2) {
            return exp(-1.0 * _gamma* pow(x_1 - x_2, 2) - _gamma * pow(y_1 - y_2, 2));
        }
    };

}