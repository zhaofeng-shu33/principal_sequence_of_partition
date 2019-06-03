#pragma once
#include "core/psp_i.h"
#include "test/utility.h"
namespace demo {
	TEST_F(Graph4PointTest, PSP_I_Contract) {
		psp::PSP instance(&g, &edge_map);
		stl::CSet S(std::string("0111"));
		instance.contract(S, 2);
		EXPECT_EQ(lemon::countNodes(g), 2);
		EXPECT_EQ(lemon::countArcs(g), 1);
		lemon::ListDigraph::ArcIt a(g);
		EXPECT_DOUBLE_EQ(edge_map[a], 2.5);
	}
	TEST(PSP_I, SIMPLE_FOUR_POINT) {
		lemon::ListDigraph g;
		lemon::ListDigraph::Node n1 = g.addNode();
		lemon::ListDigraph::Node n2 = g.addNode();
		lemon::ListDigraph::Node n3 = g.addNode();
		lemon::ListDigraph::Node n4 = g.addNode();
		lemon::ListDigraph::Arc a1 = g.addArc(n1, n2);
		lemon::ListDigraph::Arc a2 = g.addArc(n2, n4);
		lemon::ListDigraph::Arc a3 = g.addArc(n3, n4);
		lemon::ListDigraph::Arc a4 = g.addArc(n1, n3);
		lemon::ListDigraph::ArcMap<double> am(g);
		am[a1] = 1;
		am[a2] = 0.5;
		am[a3] = 2;
		am[a4] = 0.4;
		submodular::PSP instance_2(&g, &am);
		instance_2.run();
		psp::PSP instance(&g, &am);
		instance.run();
		std::list<double> cv = instance.get_critical_values();
		EXPECT_EQ(cv.size(), 3);
		std::list<double>::iterator it = cv.begin();
		EXPECT_DOUBLE_EQ(*it, 0.9);
		it++;
		EXPECT_DOUBLE_EQ(*it, 1);
		it++;
		EXPECT_DOUBLE_EQ(*it, 2);

		std::list<stl::Partition> psp_list = instance.get_psp();
		EXPECT_EQ(psp_list.size(), 4);
		std::list<stl::Partition>::iterator it_2 = psp_list.begin();
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