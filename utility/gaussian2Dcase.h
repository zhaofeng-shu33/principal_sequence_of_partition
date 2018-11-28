#pragma once
#include <random>
#include <chrono>
#include <iostream>
#include <fstream>
#include "core/graph.h"
#include "core/oracles/graph_cut.h"
#include "core/dt.h"
namespace demo {
    template <typename ValueType>
    class Gaussian2DGraph {
    public:
        using value_type = ValueType;
        using EdgeListFloat = std::vector<std::tuple<std::size_t, std::size_t, value_type>>;
        Gaussian2DGraph(int np, value_type a[][2] = NULL) :
            distribution(0, 1),
            num_points(np)
        {
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
            for (int i = 0; i < num_points; i++)
                for (int j = i + 1; j < num_points; j++) {
                    edge_list_float_1.push_back(std::make_tuple(i, j, compute_similarity(x_pos[i], y_pos[i], x_pos[j], y_pos[j])));
                }
            sg = submodular::make_dgraph(num_points, edge_list_float_1);
        }
        void run(bool bruteForce = false) {
            submodular::DirectedGraphCutOracle<ValueType>* dgc = new submodular::DirectedGraphCutOracle<ValueType>(sg);
            submodular::PSP<ValueType> psp_class(dgc);
            psp_class.run(bruteForce);
            gamma_list = psp_class.Get_critical_values();
            psp_list = psp_class.Get_psp();
            delete dgc;
        }
        std::vector<value_type>& get_gamma_list() {
            return gamma_list;
        }
        std::vector<std::vector<submodular::Set>>& get_psp_list() {
            return psp_list;
        }
        //! get the smallest partition whose size >= k
        std::vector<submodular::Set>& get_smallest_partition(int k) {
            for (std::vector<submodular::Set>& i : psp_list) {
                if (i.size() >= k)
                    return i;
            }
        }
        //! get the smallest partition whose size >= k, label each data point with an integer
        std::vector<int> get_category(int k) {            
            for (std::vector<submodular::Set>& i : psp_list) {
                if (i.size() >= k){
                    return to_category(i);
                }
            }
        }
        std::vector<value_type>& get_x_pos_list() {
            return x_pos;
        }
        std::vector<value_type>& get_y_pos_list() {
            return y_pos;
        }

    private:
        // {src, dst, capacity}
        int num_points;
        std::default_random_engine generator;
        std::normal_distribution<value_type> distribution;
        submodular::SimpleGraph<ValueType> sg;
        std::vector<value_type> x_pos;
        std::vector<value_type> y_pos;
        std::vector<value_type> gamma_list;
        std::vector<std::vector<submodular::Set>> psp_list;
        EdgeListFloat edge_list_float_1;
        int data_1[4][2] = { {3,3},{3,-3},{-3,-3},{-3,3} };
        //! use Gaussian similarity function $exp(-||p_1 - p_2||^2/2) $
        value_type compute_similarity(value_type x_1, value_type y_1, value_type x_2, value_type y_2) {
            return exp(-pow(x_1 - x_2, 2) / 2 - pow(y_1 - y_2, 2) / 2);
        }
        //! form conversion
        std::vector<int> to_category(std::vector<submodular::Set>& partation) {
            std::vector<int> cat(num_points, 0);
            int t = 0;
            for (submodular::Set& j : partation) {
                for (int i : j.GetMembers()) {
                    cat[i] = t;
                }
                t++;
            }
            return cat;
        }
    };

}
std::ostream& operator<<(std::ostream& os, std::vector<submodular::Set> partition) {
    os << '{';
    for (const submodular::Set& s : partition)
        os << s;
    os << '}';
    return os;
}
