#pragma once
#include <gtest/gtest.h>
#include "psp/graph.h"
#include "psp/dt.h"
#include "gaussian2Dcase.h"
#include "utility.h"
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
    std::vector<stl::Partition> psp_list = g2g.get_partitions();
    EXPECT_EQ(psp_list.size(), 2);
    std::vector<stl::Partition>::iterator it = psp_list.begin();
    EXPECT_EQ(it->Cardinality(), 1);
    it++;
    EXPECT_EQ(it->Cardinality(), 4);
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
    g2g.run("pdt");
    std::vector<double> gamma_list = g2g.get_critical_values();
    std::vector<stl::Partition> psp_list = g2g.get_partitions();
    g2g.run("dt");
    std::vector<double> gamma_list_2 = g2g.get_critical_values();
    std::vector<stl::Partition> psp_list_2 = g2g.get_partitions();
    std::vector<double>::iterator it_2 = gamma_list_2.begin();
    for (std::vector<double>::iterator it = gamma_list.begin(); it != gamma_list.end(); it++) {
        lemon::Tolerance<double> _tolerance;
        EXPECT_FALSE(_tolerance.different(*it, *it_2));
        it_2++;
    }
    EXPECT_EQ(psp_list, psp_list_2);
}

// This test is used to verify that MaxFlow algorithm works
TEST_F(Graph4PointTest, TwoCase) {
    submodular::DT psp_class(&g, &edge_map);
    psp_class.run();
    std::list<double> gamma_list = psp_class.get_critical_values();
    std::list<stl::Partition> psp_list = psp_class.get_psp();
    std::list<stl::Partition>::iterator it = psp_list.begin();
    EXPECT_DOUBLE_EQ(*gamma_list.begin(),1+2/3.0);    

    EXPECT_EQ(psp_list.size(), 2);
    EXPECT_EQ(it->Cardinality(), 1);
    it++;
    EXPECT_EQ(it->Cardinality(), 4);
}

}
