#include <gtest/gtest.h>
#include "core/pmf.h"
TEST(PMF, EquationSolve) {
    std::vector<pair> parameter_list;
    parameter_list.push_back(std::make_pair(2, 0));
    parameter_list.push_back(std::make_pair(0, 0));
    double lambda_1 = compute_lambda(parameter_list, -1.0);
    EXPECT_DOUBLE_EQ(lambda_1, 0.5);
    double lambda_2 = compute_lambda(parameter_list, -6.0);
    EXPECT_DOUBLE_EQ(lambda_2, 2);
    double lambda_3 = compute_lambda(parameter_list, 1);
    EXPECT_DOUBLE_EQ(lambda_3, -1);
    double lambda_4 = compute_lambda(parameter_list, 0);
    EXPECT_DOUBLE_EQ(lambda_4, 0);
    double lambda_5 = compute_lambda(parameter_list, -2);
    EXPECT_DOUBLE_EQ(lambda_5, 1);
}