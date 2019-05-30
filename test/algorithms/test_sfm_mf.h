#pragma once
#include <gtest/gtest.h>
#include "core/sfm_mf.h"
#include "test/utility.h"
#include "core/graph/gaussian2Dcase.h"
namespace demo {

TEST_F(Graph4PointTest, MFCompare) {
    std::vector<double> xl({ -1 - 2 / 3.0,-2 / 3.0,-1 / 6.0 });
    submodular::MF solver2;
    solver2.Minimize(xl, 5 / 3.0, &g, &edge_map);
    submodular::BruteForce solver1;
    solver1.Minimize(xl, 1 + 2 / 3.0, &g, &edge_map);
    EXPECT_DOUBLE_EQ(solver2.GetMinimumValue(), solver1.GetMinimumValue());
}

TEST_F(Graph4PointTest, MFDT) {
    submodular::DilworthTruncation dt(5 / 3.0 + 0.1, &g, &edge_map);
    dt.Run(true);//BruteForce
    double min_value = dt.Get_min_value();
    stl::Partition P_apostrophe = dt.Get_min_partition();
    EXPECT_EQ(P_apostrophe.Cardinality(), 4);
    dt.Run(false);
    EXPECT_DOUBLE_EQ(dt.Get_min_value(), min_value);
    EXPECT_EQ(dt.Get_min_partition().Cardinality(), 4);
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
	dt.Run(false);
	double min_value = dt.Get_min_value();
	stl::Partition P_apostrophe = dt.Get_min_partition();
	EXPECT_DOUBLE_EQ(min_value, -0.2);
	stl::Partition p;
	p.AddElement(stl::CSet(std::string("0011")));
	p.AddElement(stl::CSet(std::string("1100")));
	EXPECT_EQ(p, P_apostrophe);

}
}