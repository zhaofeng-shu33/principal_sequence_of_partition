#include <gtest/gtest.h>
#include "agglomerative/agglomerative.h"
#include "test/test_extreme.h"
namespace submodular{
TEST(Agglomerative, HyperGraphicalModel2) {
    HyperGraphicalModel2<double> hgm2;
    SampleFunction<double> sff;
    DilworthTruncation<double> dt(&sff, 2);
    dt.Run(true);
    // optimal value is 16
    // optimal partition is { 0 }, { 1 },        

    EXPECT_EQ(dt.Get_min_value(), 16);
    std::vector<Set> p = dt.Get_min_partition();
    EXPECT_EQ(p.size(), 2);
    EXPECT_EQ(p[0], Set::FromIndices(2, { 0 }));
    EXPECT_EQ(p[1], Set::FromIndices(2, { 1 }));
}
}