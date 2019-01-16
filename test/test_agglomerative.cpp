#include <sstream>

#include <gtest/gtest.h>

#include "core/agglomerative.h"
#include "test/test_extreme.h"
#include "core/io_utility.h"
#include "core/dt.h"
namespace submodular{
TEST(Agglomerative, HyperGraphicalModel2_Bottom) {
    HyperGraphicalModel2<double> hgm2;
    std::vector<Set> finest = Set::MakeFine(6);
    MNBFunction mnb(finest, 0, &hgm2);
    Set B = Set::FromIndices(6, { 1,2,3,4,5 }); // omit 0
    EXPECT_EQ(mnb.Call(B), -4);
    FWRobust<double> solver2;
    solver2.Minimize(mnb);
    std::vector<double> x_data = solver2.get_x_data();
    EXPECT_EQ(x_data.size(), 5);
    EXPECT_DOUBLE_EQ(x_data[0], -2);
    EXPECT_DOUBLE_EQ(x_data[1], -1);
    EXPECT_DOUBLE_EQ(x_data[2], -0.5);
    EXPECT_DOUBLE_EQ(x_data[3], -0.5);
    EXPECT_DOUBLE_EQ(x_data[4], 0);
}
TEST(Agglomerative, HyperGraphicalModel2_Middle) {
    HyperGraphicalModel2<double> hgm2;
    std::vector<Set> finest = Set::MakeFine(6);
    std::pair<double, std::vector<Set>> result, result_2, result_3;
    result = agglomerate(finest, &hgm2);
    EXPECT_EQ(result.first, 2);
    std::stringstream ss;
    ss << result.second;
    EXPECT_EQ(ss.str(), std::string("{{0, 1}{2}{3}{4}{5}}"));

    MNBFunction mnb(result.second, 0, &hgm2);
    Set B = Set::FromIndices(5, { 1,2,3,4}); // omit 0
    EXPECT_EQ(mnb.Call(B), -2);

    result_2 = agglomerate(result.second, &hgm2);
    EXPECT_EQ(result_2.first, 1);
    ss.str(std::string());
    ss << result_2.second;
    EXPECT_EQ(ss.str(), std::string("{{0, 1, 2}{3, 4}{5}}"));

    result_3 = agglomerate(result_2.second, &hgm2);
    EXPECT_EQ(result_3.first, 0);
    ss.str(std::string());
    ss << result_3.second;
    EXPECT_EQ(ss.str(), std::string("{{0, 1, 2, 3, 4, 5}}"));
}

TEST(Agglomerative, HyperGraphicalModel2_Top) {
    HyperGraphicalModel2<double>* hgm = new HyperGraphicalModel2<double>();
    PSP<double> psp_class(hgm);
    psp_class.agglomerative_run();
    std::vector<double> gamma_list = psp_class.Get_critical_values();
    std::vector<std::vector<Set>> psp_list = psp_class.Get_psp();
    EXPECT_EQ(gamma_list.size(), 6);
    // effective element of gamma_list is corresponding to non-empty element of psp_list
    EXPECT_EQ(gamma_list[0], 0);
    EXPECT_EQ(gamma_list[2], 1);
    EXPECT_EQ(gamma_list[4], 2);

    EXPECT_EQ(psp_list.size(), 6);

    EXPECT_EQ(psp_list[0].size(), 1);
    EXPECT_EQ(psp_list[0][0], Set::MakeDense(6));

    EXPECT_EQ(psp_list[1].size(), 0);

    EXPECT_EQ(psp_list[2].size(), 3);
    EXPECT_EQ(psp_list[2][0], Set::FromIndices(6, { 0, 1, 2 }));
    EXPECT_EQ(psp_list[2][1], Set::FromIndices(6, { 3, 4 }));
    EXPECT_EQ(psp_list[2][2], Set::FromIndices(6, { 5 }));

    EXPECT_EQ(psp_list[3].size(), 0);

    EXPECT_EQ(psp_list[4].size(), 5);
    EXPECT_EQ(psp_list[4][0], Set::FromIndices(6, { 0, 1 }));
    EXPECT_EQ(psp_list[4][1], Set::FromIndices(6, { 2 }));
    EXPECT_EQ(psp_list[4][2], Set::FromIndices(6, { 3 }));
    EXPECT_EQ(psp_list[4][3], Set::FromIndices(6, { 4 }));
    EXPECT_EQ(psp_list[4][4], Set::FromIndices(6, { 5 }));

    EXPECT_EQ(psp_list[5].size(), 6);
    EXPECT_EQ(psp_list[5][0], Set::FromIndices(6, { 0 }));
    EXPECT_EQ(psp_list[5][1], Set::FromIndices(6, { 1 }));
    EXPECT_EQ(psp_list[5][2], Set::FromIndices(6, { 2 }));
    EXPECT_EQ(psp_list[5][3], Set::FromIndices(6, { 3 }));
    EXPECT_EQ(psp_list[5][4], Set::FromIndices(6, { 4 }));
    EXPECT_EQ(psp_list[5][5], Set::FromIndices(6, { 5 }));

    delete hgm;
    // partition list is {{0, 1, 2, 3, 4, 5}}, {{{0, 1, 2}, {3, 4}, {5}}, {{0, 1}, {2}, {3}, {4}, {5}}, {{0}, {1}, {2}, {3}, {4}, {5}}}  
    // gamma list is {0, 1, 2}
}
}