#pragma once
#include <gtest/gtest.h>
#include <core/algorithms/sfm_mf.h>

#include "utility/gaussian2Dcase.h"
namespace demo {

TEST_F(Graph4PointTest, MFCompare) {
    std::vector<double> xl({ -1 - 2 / 3.0,-2 / 3.0,-1 / 6.0 });
    submodular::SampleFunctionPartial<double> F1(xl, dgc, 1 + 2 / 3.0);
    submodular::MF<double> solver2;
    solver2.Minimize(dgc, xl, 5 / 3.0);
    submodular::BruteForce<double> solver1;
    solver1.Minimize(F1);
    EXPECT_DOUBLE_EQ(solver2.GetMinimumValue(), solver1.GetMinimumValue());
    std::cout << solver2.GetReporter() << std::endl;
    std::cout << solver1.GetReporter() << std::endl;
}
}