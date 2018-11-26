#pragma once
#include "gtest/gtest.h"
#include "core/graph.h"
#include "core/dt.h"
#include "core/oracles/graph_cut.h"
#include "core/set_utils.h"
#include "core/algorithms/sfm_fw.h"
#include "utility/gaussian2Dcase.h"
namespace demo {

TEST(Gaussian2D, GivenPoint) {
    double a[4][2] = { {3,3},{3,-3},{-3,3},{-3,-3} };
    Gaussian2DGraph<double> g2g(4,a);
    std::vector<double> x_pos = g2g.get_x_pos_list();
    std::vector<double> y_pos = g2g.get_y_pos_list();
    EXPECT_EQ(x_pos.size(), 4);
    EXPECT_FLOAT_EQ(x_pos[0], 3);
    EXPECT_FLOAT_EQ(x_pos[2], -3);

    EXPECT_EQ(y_pos.size(), 4);
    EXPECT_FLOAT_EQ(y_pos[1], -3);
    EXPECT_FLOAT_EQ(y_pos[2], 3);
    g2g.run();
    std::vector<double> gamma_list = g2g.get_gamma_list();
    std::vector<std::vector<submodular::Set>> psp_list = g2g.get_psp_list();
    EXPECT_EQ(psp_list[0].size(), 1);
    EXPECT_EQ(psp_list[1].size(), 0);
    EXPECT_EQ(psp_list[2].size(), 0);
    EXPECT_EQ(psp_list[3].size(), 4);
}
TEST(Gaussian2D, GivenPoint8) {
    double a[8][2] = { {3.1, 3.2},
                       {4.0, 4.0 },
                       {1.1, -2.2},
                       {3.9, -2.0},
                       {-3.9, -2.0},
                       {-2.2, -3.5},
                       {-3.9, 2.4},
                       {-3.1, 2.6}
                     };
    Gaussian2DGraph<double> g2g(8, a);
    g2g.run(true);
    std::vector<double> gamma_list = g2g.get_gamma_list();
    std::vector<std::vector<submodular::Set>> psp_list = g2g.get_psp_list();
    for (int i = 0; i < gamma_list.size(); i++) {
        if (psp_list[i].size() == 0)
            continue;
        std::cout << "critical value: " << gamma_list[i] << std::endl; // the last critical value is not set
        std::cout << psp_list[i] << std::endl;
    }
    g2g.run(false);
    EXPECT_EQ(psp_list[0].size(), 1);
    EXPECT_EQ(psp_list[7].size(), 8);
}
class Graph4PointTest : public testing::Test {
protected:
    using EdgeListFloat = std::vector<std::tuple<std::size_t, std::size_t, double>>;

     float edge_1_value = 1.0;
    float edge_d_1_value = 0.5;

    EdgeListFloat edge_list_float_1;
    submodular::DirectedGraphCutOracle<double>* dgc;
    virtual void SetUp() {
        edge_list_float_1.push_back(std::make_tuple(0, 1, edge_1_value));
        edge_list_float_1.push_back(std::make_tuple(1, 2, edge_1_value));
        edge_list_float_1.push_back(std::make_tuple(2, 3, edge_1_value));
        edge_list_float_1.push_back(std::make_tuple(0, 3, edge_1_value));
        edge_list_float_1.push_back(std::make_tuple(0, 2, edge_d_1_value));
        edge_list_float_1.push_back(std::make_tuple(1, 3, edge_d_1_value));

        submodular::SimpleGraph<double> sg = submodular::make_dgraph(4, edge_list_float_1);

        dgc = new submodular::DirectedGraphCutOracle<double>(sg);

    }
};
// This test is used to verify that FWRobust algorithm works
TEST_F(Graph4PointTest, TwoCase) {
    submodular::PSP<double> psp_class(dgc);
    psp_class.run();
    std::vector<double> gamma_list = psp_class.Get_critical_values();
    std::vector<std::vector<submodular::Set>> psp_list = psp_class.Get_psp();
    EXPECT_DOUBLE_EQ(gamma_list[0],1+2/3.0);    
    EXPECT_EQ(psp_list[0].size(), 1);
    EXPECT_EQ(psp_list[1].size(), 0);
    EXPECT_EQ(psp_list[2].size(), 0);
    EXPECT_EQ(psp_list[3].size(), 4);
}
#ifdef USE_EIGEN3
// This test is used to verify the FWRobust algorithm can return in finite times
TEST_F(Graph4PointTest, NotReturn) {
    std::vector<double> xl({ -1-2/3.0,-2/3.0,-1/6.0 });
    submodular::SampleFunctionPartial<double> F1(xl, dgc, 1+2/3.0);
    submodular::FWRobust<double> solver2;
    solver2.Minimize(F1);
    submodular::BruteForce<double> solver1;
    solver1.Minimize(F1);
    EXPECT_DOUBLE_EQ(solver2.GetMinimumValue(), solver1.GetMinimumValue());
    std::cout << solver2.GetReporter() << std::endl;
    std::cout << solver1.GetReporter() << std::endl;
}
#endif
// This test is used to verify the FWRobust algorithm returns the right solution to SFM problem
// When two min solution exists, FWRobust not necessarily returns the finest partition
TEST_F(Graph4PointTest, ReturnTrue) {
    submodular::DilworthTruncation<double> dt(dgc, 5/3.0+0.1);
    dt.Run(true);//BruteForce
    double min_value = dt.Get_min_value();
    std::vector<submodular::Set> P_apostrophe = dt.Get_min_partition();
    EXPECT_EQ(P_apostrophe.size(), 4);
    dt.Run(); //FWRobust
    EXPECT_DOUBLE_EQ(dt.Get_min_value(), min_value);
    EXPECT_EQ(dt.Get_min_partition().size(), 4);
}
}