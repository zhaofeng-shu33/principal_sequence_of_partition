#include <gtest/gtest.h>
#include "core/agglomerative.h"
#include "test/test_extreme.h"
#include "core/io_utility.h"
namespace submodular{
TEST(Agglomerative, HyperGraphicalModel2) {
    HyperGraphicalModel2<double> hgm2;
    std::vector<Set> finest = Set::MakeFine(6);
    MNBFunction mnb(finest, 0, &hgm2);
    FWRobust<double> solver2;
    solver2.Minimize(mnb);
    std::vector<double> x_data = solver2.get_x_data();
    std::cout << x_data;
}
}