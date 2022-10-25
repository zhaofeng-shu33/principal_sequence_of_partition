#pragma once
#include "psp/agglomerative.h"
#include "utility.h"
namespace demo {
TEST(Agglomerative_PSP, SIMPLE_FOUR_POINT) {
    std::vector<std::tuple<std::size_t, std::size_t, double>> elt;
    elt.push_back(std::make_tuple(0, 1, 1));
    elt.push_back(std::make_tuple(0, 2, 0.4));
    elt.push_back(std::make_tuple(1, 3, 0.5));
    elt.push_back(std::make_tuple(2, 3, 2));
    psp::PSP ic(elt, 4);
    ic.run("agg_psp");
    std::vector<double> cv = ic.get_critical_values();
    EXPECT_EQ(cv.size(), 3);
    std::vector<double>::iterator it = cv.begin();
    EXPECT_DOUBLE_EQ(*it, 0.9);
    it++;
    EXPECT_DOUBLE_EQ(*it, 1);
    it++;
    EXPECT_DOUBLE_EQ(*it, 2);

    std::vector<stl::Partition> psp_list = ic.get_partitions();
    EXPECT_EQ(psp_list.size(), 4);
    std::vector<stl::Partition>::iterator it_2 = psp_list.begin();
    EXPECT_EQ(*it_2, stl::Partition::makeDense(4));
    stl::Partition p2;
    p2.AddElement(stl::CSet(std::string("11")));
    p2.AddElement(stl::CSet(std::string("0011")));
    it_2++;
    EXPECT_EQ(*it_2, p2);
    stl::Partition p3;
    p3.AddElement(stl::CSet(std::string("1")));
    p3.AddElement(stl::CSet(std::string("01")));
    p3.AddElement(stl::CSet(std::string("0011")));
    it_2++;
    EXPECT_EQ(*it_2, p3);
    it_2++;
    EXPECT_EQ(*it_2, stl::Partition::makeFine(4));
}

}