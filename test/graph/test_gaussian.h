#pragma once
#include "gtest/gtest.h"
#include "core/graph/graph.h"
#include "core/dt.h"
#include "core/graph/gaussian2Dcase.h"
#include "test/utility.h"
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
    std::vector<stl::Partition> psp_list = g2g.get_psp_list();
    EXPECT_EQ(psp_list[0].Cardinality(), 1);
    EXPECT_EQ(psp_list[1].Cardinality(), 0);
    EXPECT_EQ(psp_list[2].Cardinality(), 0);
    EXPECT_EQ(psp_list[3].Cardinality(), 4);

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
    std::vector<stl::Partition> psp_list = g2g.get_psp_list();
    g2g.run();
    std::vector<double> gamma_list_2 = g2g.get_gamma_list();
    std::vector<stl::Partition> psp_list_2 = g2g.get_psp_list();

    for (int i = 0; i < gamma_list.size(); i++) {
        if (psp_list[i].Cardinality() == 0)
            continue;
        EXPECT_DOUBLE_EQ(gamma_list[i], gamma_list_2[i]);
        EXPECT_EQ(psp_list[i], psp_list_2[i]);
    }

}

// This test is used to verify that MaxFlow algorithm works
TEST_F(Graph4PointTest, TwoCase) {
    submodular::PSP<double> psp_class(&g, &edge_map);
    psp_class.run();
    std::vector<double> gamma_list = psp_class.Get_critical_values();
    std::vector<stl::Partition> psp_list = psp_class.Get_psp();
    EXPECT_DOUBLE_EQ(gamma_list[0],1+2/3.0);    
    EXPECT_EQ(psp_list[0].Cardinality(), 1);
    EXPECT_EQ(psp_list[1].Cardinality(), 0);
    EXPECT_EQ(psp_list[2].Cardinality(), 0);
    EXPECT_EQ(psp_list[3].Cardinality(), 4);
}

}
