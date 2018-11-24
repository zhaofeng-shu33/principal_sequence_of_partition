#pragma once
#include "gtest/gtest.h"
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

}