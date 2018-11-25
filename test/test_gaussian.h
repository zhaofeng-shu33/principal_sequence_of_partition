#pragma once
#include "gtest/gtest.h"
#include "core/graph.h"
#include "core/oracles/graph_cut.h"
#include "core/set_utils.h"
#include "core/algorithms/sfm_fw.h"
#include "utility/gaussian2Dcase.h"
namespace demo {

TEST(Gaussian2D, GivenPoint) {
    float a[4][2] = { {3,3},{3,-3},{-3,3},{-3,-3} };
    Gaussian2DGraph g2g(4,a);
    std::vector<float> x_pos = g2g.get_x_pos_list();
    std::vector<float> y_pos = g2g.get_y_pos_list();
    EXPECT_EQ(x_pos.size(), 4);
    EXPECT_FLOAT_EQ(x_pos[0], 3);
    EXPECT_FLOAT_EQ(x_pos[2], -3);

    EXPECT_EQ(y_pos.size(), 4);
    EXPECT_FLOAT_EQ(y_pos[1], -3);
    EXPECT_FLOAT_EQ(y_pos[2], 3);

}
class Graph4PointTest : public testing::Test {
protected:
    using EdgeListFloat = std::vector<std::tuple<std::size_t, std::size_t, float>>;

     float edge_1_value = 1.0;
    float edge_d_1_value = 0.5;

    EdgeListFloat edge_list_float_1;
    submodular::DirectedGraphCutOracle<float>* dgc;
    virtual void SetUp() {
        edge_list_float_1.push_back(std::make_tuple(0, 1, edge_1_value));
        edge_list_float_1.push_back(std::make_tuple(1, 2, edge_1_value));
        edge_list_float_1.push_back(std::make_tuple(2, 3, edge_1_value));
        edge_list_float_1.push_back(std::make_tuple(0, 3, edge_1_value));
        edge_list_float_1.push_back(std::make_tuple(0, 2, edge_d_1_value));
        edge_list_float_1.push_back(std::make_tuple(1, 3, edge_d_1_value));

        submodular::SimpleGraph<float> sg = submodular::make_dgraph(4, edge_list_float_1);

        dgc = new submodular::DirectedGraphCutOracle<float>(sg);

    }
};
TEST_F(Graph4PointTest, TwoCase) {
    submodular::PSP<float> psp_class(dgc);
    psp_class.run();
    std::vector<float> gamma_list = psp_class.Get_critical_values();
    std::vector<std::vector<submodular::Set>> psp_list = psp_class.Get_psp();
    EXPECT_EQ(psp_list[0].size(), 1);
    EXPECT_EQ(psp_list[3].size(), 4);
}
#ifdef USE_EIGEN3
TEST_F(Graph4PointTest, NotReturn) {
    std::vector<float> xl({ -1-2/3.0,-2/3.0,-1/6.0 });
    submodular::SampleFunctionPartial<float> F1(xl, dgc, 1+2/3.0);
    submodular::FWRobust<float> solver2;
    solver2.Minimize(F1);
}
#endif
}