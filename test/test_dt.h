#pragma once
#include <gtest/gtest.h>
#include "core/sfm_mf.h"
#include "core/psp_i.h"
#include "test/utility.h"
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
		dt.run(false);//BruteForce
		double min_value = dt.get_min_value();
		stl::Partition P_apostrophe = dt.get_min_partition();

		psp::DilworthTruncation_Improved dt2(5 / 3.0 + 0.1, &g, &edge_map);
		dt2.run();
		stl::Partition p = dt2.get_min_partition();
		double v = dt2.get_min_value();

		EXPECT_DOUBLE_EQ(v, min_value); 
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
		dt.run(false);
		double min_value = dt.get_min_value();
		stl::Partition P_apostrophe = dt.get_min_partition();

		psp::DilworthTruncation_Improved dt2(0.1, &g, &arc_map);
		dt2.run();
		stl::Partition p2 = dt2.get_min_partition();
		double v = dt2.get_min_value();

		EXPECT_DOUBLE_EQ(min_value, -0.2);
		stl::Partition p;
		p.AddElement(stl::CSet(std::string("0011")));
		p.AddElement(stl::CSet(std::string("1100")));
		EXPECT_EQ(p, P_apostrophe);

		EXPECT_DOUBLE_EQ(v, -0.2);
		EXPECT_EQ(p2, p);
	}

	TEST(ThreePoint, DT_I) {
		lemon::ListDigraph g;
		lemon::ListDigraph::Node n1 = g.addNode();
		lemon::ListDigraph::Node n2 = g.addNode();
		lemon::ListDigraph::Node n3 = g.addNode();

		lemon::ListDigraph::ArcMap<double> arc_map(g);
		lemon::ListDigraph::Arc a1 = g.addArc(n1, n2);
		lemon::ListDigraph::Arc a2 = g.addArc(n1, n3);
		lemon::ListDigraph::Arc a3 = g.addArc(n2, n3);
		arc_map[a1] = 1;
		arc_map[a2] = 1;
		arc_map[a3] = 5;
		psp::DilworthTruncation_Improved dt(3, &g, &arc_map);
		dt.run();
		stl::Partition p = dt.get_min_partition();
		stl::Partition p1;
		p1.AddElement(stl::CSet(std::string("100")));
		p1.AddElement(stl::CSet(std::string("011")));
		EXPECT_EQ(p, p1);
		double v = dt.get_min_value();
		EXPECT_DOUBLE_EQ(v, -4);
	}
	// verify the early stopping property of new dt algorithm
	TEST(ThreePoint, DT_I_Early_Stopping) {
		lemon::ListDigraph g;
		lemon::ListDigraph::Node n1 = g.addNode();
		lemon::ListDigraph::Node n2 = g.addNode();
		lemon::ListDigraph::Node n3 = g.addNode();

		lemon::ListDigraph::ArcMap<double> arc_map(g);
		lemon::ListDigraph::Arc a1 = g.addArc(n1, n2);
		lemon::ListDigraph::Arc a2 = g.addArc(n1, n3);
		lemon::ListDigraph::Arc a3 = g.addArc(n2, n3);
		arc_map[a1] = 1;
		arc_map[a2] = 1;
		arc_map[a3] = 1;
		psp::DilworthTruncation_Improved dt(1.5, &g, &arc_map);
		dt.run();
		stl::Partition p = dt.get_min_partition();
		stl::Partition p1 = stl::Partition::makeDense(3);
		EXPECT_EQ(p, p1);
		double v = dt.get_min_value();
		EXPECT_DOUBLE_EQ(v, -1.5);
	}
}