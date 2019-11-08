#pragma once
#include <gtest/gtest.h>
#include "utility.h"
namespace demo {
// This test is used to verify that MaxFlow algorithm works
TEST_F(Graph4PointTestGraph, DT) {
    submodular::DT psp_class(&g, &arc_map);
    psp_class.run();
    std::list<double> gamma_list = psp_class.get_critical_values();
    std::list<stl::Partition> psp_list = psp_class.get_psp();
    std::list<stl::Partition>::iterator it = psp_list.begin();
    EXPECT_DOUBLE_EQ(*gamma_list.begin(), 5 / 3.0);

    EXPECT_EQ(psp_list.size(), 2);
    EXPECT_EQ(it->Cardinality(), 1);
    it++;
    EXPECT_EQ(it->Cardinality(), 4);
}

TEST_F(Graph4PointTestGraph, MFDT) {
    submodular::DilworthTruncation dt(5 / 3.0 + 0.1, &g, &arc_map);
    dt.run();
    double min_value = dt.get_min_value();
    stl::Partition P_apostrophe = dt.get_min_partition();

    stl::Partition p = stl::Partition::makeFine(4);

    EXPECT_DOUBLE_EQ(-6.2/3, min_value); 
    EXPECT_EQ(P_apostrophe, p); // {0, 1, 2, 3}
}

TEST_F(FourPointNotCompleteGraph, MFDT) {
    submodular::DilworthTruncation dt(0.1, &g, &arc_map);
    dt.run();
    double min_value = dt.get_min_value();
    stl::Partition P_apostrophe = dt.get_min_partition();

    EXPECT_DOUBLE_EQ(min_value, -0.2);
    stl::Partition p;
    p.AddElement(stl::CSet(std::string("0011")));
    p.AddElement(stl::CSet(std::string("1100")));
    EXPECT_EQ(p, P_apostrophe);
}
}