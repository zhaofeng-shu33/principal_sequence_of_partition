#include <gtest/gtest.h>
#include "test/utility.h"
#include "core/digraph_utility.h"
namespace demo {
	TEST_F(Graph4PointTest, Contract) {
		stl::CSet S(std::string("0111"));
		lemon::digraph_contract(g, edge_map, S, 2);
		EXPECT_EQ(lemon::countNodes(g), 2);
		EXPECT_EQ(lemon::countArcs(g), 1);
		lemon::ListDigraph::ArcIt a(g);
		EXPECT_DOUBLE_EQ(edge_map[a], 2.5);
	}
}
namespace lemon {
	TEST(DigraphUtility, COPY){
		ListDigraph G1;
		ArcMap A1(G1);
		Node n0 = G1.addNode();
		Node n1 = G1.addNode();
		Node n2 = G1.addNode();
		Arc a01 = G1.addArc(n0, n1);
		Arc a02 = G1.addArc(n0, n2);
		A1[a01] = 3;
		A1[a02] = 4;
		G1.erase(n1);
		ListDigraph G2;
		ArcMap A2(G2);
		digraph_copy(G1, A1, G2, A2);
		EXPECT_EQ(countNodes(G2), 2);
		EXPECT_EQ(countArcs(G2), 1);
		ListDigraph::ArcIt a(G2);
		EXPECT_DOUBLE_EQ(A2[a], 4);
	}
}