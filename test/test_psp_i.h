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
}