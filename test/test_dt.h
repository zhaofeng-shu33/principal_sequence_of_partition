#pragma once
#include <gtest/gtest.h>
#include "core/psp_i.h"
#include "test/utility.h"
namespace demo {


    TEST_F(Graph4PointTest, MFDT) {
        submodular::DilworthTruncation dt(5 / 3.0 + 0.1, &g, &edge_map);
        dt.run();
        double min_value = dt.get_min_value();
        stl::Partition P_apostrophe = dt.get_min_partition();

        stl::Partition p = stl::Partition::makeFine(4);

        EXPECT_DOUBLE_EQ(-6.2/3, min_value); 
        EXPECT_EQ(P_apostrophe, p); // {0, 1, 2, 3}
    }

    TEST(FourPointNotComplete, MFDT) {
        lemon::ListDigraph g;
        lemon::ListDigraph::Node n1 = g.addNode();
        lemon::ListDigraph::Node n2 = g.addNode();
        lemon::ListDigraph::Node n3 = g.addNode();
        lemon::ListDigraph::Node n4 = g.addNode();

        lemon::ListDigraph::ArcMap<double> arc_map(g);
        lemon::ListDigraph::Arc a1 = g.addArc(n1, n2);
        lemon::ListDigraph::Arc a2 = g.addArc(n3, n4);
        arc_map[a1] = 1;
        arc_map[a2] = 1;

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