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
	TEST(PSP_I, GivenPoint8) {
		double a[8][2] = { {3.1, 3.2},
						   {4.0, 4.0 },
						   {1.1, -2.2},
						   {3.9, -2.0},
						   {-3.9, -2.0},
						   {-2.2, -3.5},
						   {-3.9, 2.4},
						   {-3.1, 2.6}
		};
		Gaussian2DGraph g2g(8, 1.0, a);
		g2g.run();
		std::vector<double> gamma_list_2 = g2g.get_gamma_list();
		std::vector<stl::Partition> psp_list_2 = g2g.get_psp_list();

		demo::EdgeListTuple et = g2g.get_edge_list_tuple();
		lemon::ListDigraph g;
		lemon::ListDigraph::ArcMap<double> edge_map(g);
		submodular::make_dgraph(8, et, g, edge_map);

		psp::PSP instance(&g, &edge_map);
		instance.run();
		std::list<double> gamma_list = instance.get_critical_values();
		std::list<stl::Partition> psp_list = instance.get_psp();

		std::list<double>::iterator it = gamma_list.begin();
		std::list<stl::Partition>::iterator it_2 = psp_list.begin();

		for (int i = 0; i < gamma_list.size(); i++) {
			if (psp_list_2[i].Cardinality() == 0)
				continue;
			EXPECT_DOUBLE_EQ(*it, gamma_list_2[i]);
			EXPECT_EQ(*it_2, psp_list_2[i]);
			it++;
			it_2++;
		}

	}
}