#pragma once
#include "gtest/gtest.h"
#include "core/graph.h"
#include "core/dt.h"
#include "core/oracles/graph_cut.h"
#include "core/set_utils.h"
#if USE_EIGEN3
    #include "core/algorithms/sfm_fw.h"
#endif
#include "core/gaussian2Dcase.h"
#include "test_utility.h"
namespace demo {

TEST(Gaussian2D, GivenPoint) {
    double a[4][2] = { {3,3},{3,-3},{-3,3},{-3,-3} };
    Gaussian2DGraph g2g(4, 1.0, a);
    std::vector<double> x_pos = g2g.get_x_pos_list();
    std::vector<double> y_pos = g2g.get_y_pos_list();
    EXPECT_EQ(x_pos.size(), 4);
    EXPECT_DOUBLE_EQ(x_pos[0], 3);
    EXPECT_DOUBLE_EQ(x_pos[2], -3);

    EXPECT_EQ(y_pos.size(), 4);
    EXPECT_DOUBLE_EQ(y_pos[1], -3);
    EXPECT_DOUBLE_EQ(y_pos[2], 3);
    g2g.run();
    std::vector<double> gamma_list = g2g.get_gamma_list();
    std::vector<std::vector<submodular::Set>> psp_list = g2g.get_psp_list();
    EXPECT_EQ(psp_list[0].size(), 1);
    EXPECT_EQ(psp_list[1].size(), 0);
    EXPECT_EQ(psp_list[2].size(), 0);
    EXPECT_EQ(psp_list[3].size(), 4);

    std::vector<int> cat = g2g.get_category(2);
    std::cout << cat << std::endl;
    EXPECT_EQ(cat.size(), 4);
    EXPECT_EQ(cat[0], 0);
    EXPECT_EQ(cat[1], 1);
    EXPECT_EQ(cat[2], 2);
    EXPECT_EQ(cat[3], 3);

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
    Gaussian2DGraph g2g(8, 1.0, a);
    g2g.run_bruteForce();
    std::vector<double> gamma_list = g2g.get_gamma_list();
    std::vector<std::vector<submodular::Set>> psp_list = g2g.get_psp_list();
    g2g.run();
    std::vector<double> gamma_list_2 = g2g.get_gamma_list();
    std::vector<std::vector<submodular::Set>> psp_list_2 = g2g.get_psp_list();

    for (int i = 0; i < gamma_list.size(); i++) {
        if (psp_list[i].size() == 0)
            continue;
        EXPECT_DOUBLE_EQ(gamma_list[i], gamma_list_2[i]);
        EXPECT_EQ(psp_list[i], psp_list_2[i]);
    }

}

// This test is used to verify that MaxFlow algorithm works
TEST_F(Graph4PointTest, TwoCase) {
    submodular::PSP<double> psp_class(dgc, &g, &edge_map);
    psp_class.run();
    std::vector<double> gamma_list = psp_class.Get_critical_values();
    std::vector<std::vector<submodular::Set>> psp_list = psp_class.Get_psp();
    EXPECT_DOUBLE_EQ(gamma_list[0],1+2/3.0);    
    EXPECT_EQ(psp_list[0].size(), 1);
    EXPECT_EQ(psp_list[1].size(), 0);
    EXPECT_EQ(psp_list[2].size(), 0);
    EXPECT_EQ(psp_list[3].size(), 4);
}

#if USE_EIGEN3
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

    // This test is used to verify the FWRobust algorithm returns the right solution to SFM problem
    // When two min solution exists, FWRobust not necessarily returns the finest partition
    TEST_F(Graph4PointTest, ReturnTrue) {
        submodular::DilworthTruncation<double> dt(dgc, 5/3.0+0.1);
        dt.Run(true);//BruteForce
        double min_value = dt.Get_min_value();
        std::vector<submodular::Set> P_apostrophe = dt.Get_min_partition();
        EXPECT_EQ(P_apostrophe.size(), 4);
        dt.Run(); //MaxFlow
        EXPECT_DOUBLE_EQ(dt.Get_min_value(), min_value);
        EXPECT_EQ(dt.Get_min_partition().size(), 4);
    }
#endif
}
