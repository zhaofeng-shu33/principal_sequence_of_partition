#include <random>
#include <chrono>
#include <iostream>
#include <fstream>
#include "core/graph.h"
#include "core/oracles/graph_cut.h"
#include "core/dt.h"
namespace demo {
    class Gaussian2DGraph {
    public:
        using EdgeListFloat = std::vector<std::tuple<std::size_t, std::size_t, float>>;
        Gaussian2DGraph(int np) :
            distribution(0, 1),
            num_points(np)
        {
            // generator num_points 2D points ~ N(0,1) located at different positions
            unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
            generator = std::default_random_engine(seed);
            std::vector<float> x_pos;
            std::vector<float> y_pos;
            for (int j = 0; j < 4; j++) {
                for (int i = 0; i < num_points / 4; i++) {
                    x_pos.push_back(distribution(generator) + data_1[j][0]);
                    y_pos.push_back(distribution(generator) + data_1[j][1]);
                }
            }
            // print the coordinates to the file
            std::ofstream fout("coordinates.txt");
            for (int i = 0; i < num_points; i++) {
                fout << x_pos[i] << '\t' << y_pos[i] << std::endl;
            }
            for (int i = 0; i < num_points; i++)
                for (int j = i + 1; j < num_points; j++) {
                    edge_list_float_1.push_back(std::make_tuple(i, j, compute_similarity(x_pos[i], y_pos[i], x_pos[j], y_pos[j])));
                }
            sg = submodular::make_dgraph(num_points, edge_list_float_1);
        }
        void run() {
            submodular::DirectedGraphCutOracle<float>* dgc = new submodular::DirectedGraphCutOracle<float>(sg);
            submodular::PSP<float> psp_class(dgc);
            psp_class.run();
            gamma_list = psp_class.Get_critical_values();
            psp_list = psp_class.Get_psp();
        }
        std::vector<float>& get_gamma_list() {
            return gamma_list;
        }
        std::vector<std::vector<submodular::Set>>& get_psp_list() {
            return psp_list;
        }
    private:
        // {src, dst, capacity}
        int num_points;
        std::default_random_engine generator;
        std::normal_distribution<float> distribution;
        submodular::SimpleGraph<float> sg;
        std::vector<float> gamma_list;
        std::vector<std::vector<submodular::Set>> psp_list;
        EdgeListFloat edge_list_float_1;
        int data_1[4][2] = { {3,3},{3,-3},{-3,-3},{-3,3} };
        //! use Gaussian similarity function $exp(-||p_1 - p_2||^2/2) $
        float compute_similarity(float x_1, float y_1, float x_2, float y_2) {
            return exp(-pow(x_1 - x_2, 2) / 2 - pow(y_1 - y_2, 2) / 2);
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