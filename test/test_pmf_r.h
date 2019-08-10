#pragma once
#include <lemon/lgf_reader.h>
#include "core/pmf_r.h"
#include "test/utility.h"
namespace parametric {
	TEST(PMF_R, contract) {
		PMF_R pmfR;
		lemon::ListDigraph& digraph = pmfR.dig;
		ArcMap& cap = pmfR.dig_aM;
		std::string lgf_str = "@nodes\nlabel\n0\n1\n2\n3\n4\n@arcs\n\t\tlabel\tcapacity\n4\t0\t0\t1\n4\t2\t1\t2\n0\t1\t2\t3\n2\t0\t3\t5\n2\t3\t4\t6\n1\t2\t5\t4\n1\t3\t6\t7";
		std::stringstream ss;
		ss << lgf_str;
		lemon::digraphReader(digraph, ss)
			.arcMap("capacity", cap)
			.run();
		pmfR.set_source_node_id(4);
		pmfR.set_sink_node_id(3);
		pmfR.set_tilde_G_size(5);
		using Set = stl::CSet;
		Set S("00101"), T("01010");
		lemon::ListDigraph new_digraph;
		ArcMap new_cap(new_digraph);
		pmfR.contract(S, T, new_digraph, new_cap);
		EXPECT_EQ(lemon::countNodes(new_digraph), 3);
		EXPECT_EQ(lemon::countArcs(new_digraph), 4);
		PMF_R::FlowMap flowMap;
		pmfR.set_flowMap(new_digraph, new_cap, flowMap);
		EXPECT_DOUBLE_EQ(flowMap[4][0], 6);
		EXPECT_DOUBLE_EQ(flowMap[4][3], 6);
		EXPECT_DOUBLE_EQ(flowMap[3][4], 4);
		EXPECT_DOUBLE_EQ(flowMap[0][3], 3);
	}
	TEST(PMF_R, flowMap) {
		PMF_R pmfR;
		lemon::ListDigraph& digraph = pmfR.dig;
		ArcMap& cap = pmfR.dig_aM;
		std::string lgf_str = "@nodes\nlabel\n0\n1\n2\n3\n4\n@arcs\n\t\tlabel\tcapacity\n4\t0\t0\t1\n4\t2\t1\t2\n0\t1\t2\t3\n2\t0\t3\t5\n2\t3\t4\t6\n1\t2\t5\t4\n1\t3\t6\t7";
		std::stringstream ss;
		ss << lgf_str;
		lemon::digraphReader(digraph, ss)
			.arcMap("capacity", cap)
			.run();
		lemon::ListDigraph::Node source_node = digraph.nodeFromId(4);
		lemon::ListDigraph::Node sink_node = digraph.nodeFromId(3);
		PMF_R::Preflow pf(digraph, cap, source_node, sink_node);
		pf.run();
		PMF_R::FlowMap newFlowMap;
		pmfR.set_flowMap(digraph, pf.flowMap(), newFlowMap);
		EXPECT_TRUE(newFlowMap.size() > 0);
		PMF_R::Preflow::FlowMap backFlowMap(digraph);
		pmfR.get_preflow_flowMap(digraph, newFlowMap, backFlowMap);
		const PMF_R::Preflow::FlowMap& originalFlowMap = pf.flowMap();
		for (lemon::ListDigraph::ArcIt a(digraph); a != lemon::INVALID; ++a) {
			EXPECT_EQ(backFlowMap[a], originalFlowMap[a]);
		}
	}
	TEST(PMF_R, modifyFlow) {
		PMF_R pmfR;
		lemon::ListDigraph& digraph = pmfR.dig;
		ArcMap& cap = pmfR.dig_aM;
		std::string lgf_str = "@nodes\nlabel\n0\n1\n2\n3\n4\n@arcs\n\t\tlabel\tcapacity\n4\t0\t0\t1\n4\t2\t1\t2\n0\t1\t2\t3\n2\t0\t3\t5\n2\t3\t4\t6\n1\t2\t5\t4\n1\t3\t6\t7";
		std::stringstream ss;
		ss << lgf_str;
		lemon::digraphReader(digraph, ss)
			.arcMap("capacity", cap)
			.run();
		pmfR.set_source_node_id(4);
		pmfR.set_sink_node_id(3);
		pmfR.set_tilde_G_size(5);
		using Set = stl::CSet;
		Set S("00101"), T("01010");
		lemon::ListDigraph new_digraph;
		ArcMap new_cap(new_digraph);
		pmfR.contract(S, T, new_digraph, new_cap);
		lemon::ListDigraph::Node source_node = digraph.nodeFromId(4);
		lemon::ListDigraph::Node sink_node = digraph.nodeFromId(3);
		PMF_R::Preflow pf(digraph, cap, source_node, sink_node);
		pf.run();
		PMF_R::FlowMap flowMap, newFlowMap;
		pmfR.set_flowMap(digraph, pf.flowMap(), flowMap);
		pmfR.modify_flow(S, T, new_digraph, new_cap, flowMap, newFlowMap);
		EXPECT_DOUBLE_EQ(newFlowMap[4][0], 1);
		EXPECT_DOUBLE_EQ(newFlowMap[4][3], 2);
		EXPECT_DOUBLE_EQ(newFlowMap[0][3], 1);
		EXPECT_DOUBLE_EQ(newFlowMap[3][4], 0);
	}
	TEST(PMF_R, PMClass) {
		using Set = stl::CSet;
		std::vector<pair> parameter_list;
		parameter_list.push_back(std::make_pair(0, INFINITY));
		parameter_list.push_back(std::make_pair(0, INFINITY));
		parameter_list.push_back(std::make_pair(0, INFINITY));

		lemon::ListDigraph g;
		lemon::ListDigraph::Node n1 = g.addNode();
		lemon::ListDigraph::Node n2 = g.addNode();
		lemon::ListDigraph::Node n3 = g.addNode();

		lemon::ListDigraph::Arc a1 = g.addArc(n1, n2);
		lemon::ListDigraph::Arc a3 = g.addArc(n1, n3);
		lemon::ListDigraph::Arc a5 = g.addArc(n2, n3);

		lemon::ListDigraph::ArcMap<double> aM(g);
		aM[a1] = 1;
		aM[a3] = 5;
		aM[a5] = 1;
		PMF_R pmf(&g, &aM, 2, parameter_list);
		pmf.run();

		std::list<Set> sL = pmf.get_set_list();
		EXPECT_EQ(sL.size(), 3);
		std::list<Set>::iterator it = sL.begin();
		EXPECT_EQ(*it, Set(std::string("111"))); // { 0, 1, 2 }
		it++;
		EXPECT_EQ(*it, Set(std::string("101"))); // { 0, 2 }
		it++;
		EXPECT_EQ(*it, Set(std::string("001"))); // { 2 }

		std::list<double> lambda_list = pmf.get_lambda_list();
		EXPECT_EQ(lambda_list.size(), 2);
		std::list<double>::iterator it_2 = lambda_list.begin();
		EXPECT_DOUBLE_EQ(*it_2, 1);
		it_2++;
		EXPECT_DOUBLE_EQ(*it_2, 5);
	}
	TEST(PDT_R, RUN) {
		std::vector<std::tuple<std::size_t, std::size_t, double>> elt;
		elt.push_back(std::make_tuple(0, 1, 1));
		elt.push_back(std::make_tuple(0, 2, 5));
		elt.push_back(std::make_tuple(1, 2, 1));
		submodular::InfoCluster ic(elt, 3);
		ic.run_pdt_r();
		std::list<double> lambda_list = ic.get_critical_values();
		std::list<Partition> partition_list = ic.get_psp();
		EXPECT_EQ(lambda_list.size(), 2);
		EXPECT_EQ(partition_list.size(), 3);
		std::list<double>::iterator d = lambda_list.begin();
		EXPECT_DOUBLE_EQ(*d, 2);
		d++;
		EXPECT_DOUBLE_EQ(*d, 5);
		std::list<Partition>::iterator p = partition_list.begin();
		EXPECT_EQ(*p, Partition::makeDense(3));
		p++;
		Partition nP;
		nP.AddElement(stl::CSet("101"));
		nP.AddElement(stl::CSet("010"));
		EXPECT_EQ(*p, nP);
		p++;
		EXPECT_EQ(*p, Partition::makeFine(3));
	}
}
namespace demo {
	TEST_F(Graph4PointTest, PDT_R) {
		submodular::InfoCluster ic(edge_list_tuple_1, 4);
		ic.run_pdt_r();
		std::list<double> lambda_list = ic.get_critical_values();
		std::list<parametric::Partition> partition_list = ic.get_psp();
		EXPECT_EQ(lambda_list.size(), 1);
		std::list<double>::iterator lambda_it = lambda_list.begin();
		EXPECT_DOUBLE_EQ(*lambda_it, 1 + 2 / 3.0);

		EXPECT_EQ(partition_list.size(), 2);
		std::list<parametric::Partition>::iterator partition_it = partition_list.begin();

		EXPECT_EQ(*partition_it, parametric::Partition::makeDense(4));
		partition_it++;
		EXPECT_EQ(*partition_it, parametric::Partition::makeFine(4));

	}

	TEST(GivenPoint8, PDT_R) {
		double a[8][2] = { {3.1, 3.2},
						   {4.0, 4.0 },
						   {1.1, -2.2},
						   {3.9, -2.0},
						   {-3.9, -2.0},
						   {-2.2, -3.5},
						   {-3.9, 2.4},
						   {-3.1, 2.6}
		};
		Gaussian2DGraphBase g2g(8, 0.1, a);
		EdgeListTuple elt = g2g.get_edge_list_tuple();
		submodular::InfoCluster ic(elt, 8);
		ic.run_pdt_r();
		std::list<double> lambda_list = ic.get_critical_values();
		std::list<parametric::Partition> partition_list = ic.get_psp();


		ic.run();
		std::list<double> lambda_list_2 = ic.get_gamma_list();
		std::list<stl::Partition> partition_list_2 = ic.get_psp();

		EXPECT_EQ(lambda_list.size(), lambda_list_2.size());
		lemon::Tolerance<double> Tol;

		std::list<double>::iterator it = lambda_list.begin();
		;
		for (std::list<double>::iterator it_2 = lambda_list_2.begin(); it_2 != lambda_list_2.end(); it_2++) {
			EXPECT_NEAR(*it, *it_2, Tol.epsilon());
			it++;
		}
		EXPECT_EQ(partition_list, partition_list_2);
	}
	TEST(ThreePointComplete, PDT_R) {
		std::vector<std::tuple<std::size_t, std::size_t, double>> edges;
		edges.push_back(std::make_tuple(0, 1, 1.0));
		edges.push_back(std::make_tuple(0, 2, 1.0));
		edges.push_back(std::make_tuple(1, 2, 5.0));
		submodular::InfoCluster ic(edges, 3);
		ic.run_pdt_r();
		std::list<double> lambda_list = ic.get_critical_values();
		std::list<parametric::Partition> partition_list = ic.get_psp();
		std::list<double>::iterator it = lambda_list.begin();
		EXPECT_DOUBLE_EQ(*it, 2);
		it++;
		EXPECT_DOUBLE_EQ(*it, 5);

		EXPECT_EQ(lambda_list.size(), 2);
		EXPECT_EQ(partition_list.size(), 3);

		stl::Partition p = stl::Partition::makeDense(3);

		std::list<parametric::Partition>::iterator it_p = partition_list.begin();
		EXPECT_EQ(*it_p, p);

		it_p++;
		p.clear();
		p.AddElement(stl::CSet(std::string("011")));
		p.AddElement(stl::CSet(std::string("100")));
		EXPECT_EQ(*it_p, p);

		p = stl::Partition::makeFine(3);
		it_p++;
		EXPECT_EQ(*it_p, p);

	}

	TEST(ThreePointNotComplete, PDT_R) {
		std::vector<std::tuple<std::size_t, std::size_t, double>> edges;
		edges.push_back(std::make_tuple(0, 1, 1.0));
		submodular::InfoCluster ic(edges, 3);
		ic.run_pdt_r();
		std::list<double> lambda_list = ic.get_critical_values();
		std::list<parametric::Partition> partition_list = ic.get_psp();
		std::list<double>::iterator it = lambda_list.begin();
		EXPECT_DOUBLE_EQ(*it, 0);
		it++;
		EXPECT_DOUBLE_EQ(*it, 1);

		EXPECT_EQ(lambda_list.size(), 2);
		EXPECT_EQ(partition_list.size(), 3);

		stl::Partition p = stl::Partition::makeDense(3);

		std::list<parametric::Partition>::iterator it_p = partition_list.begin();
		EXPECT_EQ(*it_p, p);

		it_p++;
		p.clear();
		p.AddElement(stl::CSet(std::string("110")));
		p.AddElement(stl::CSet(std::string("001")));
		EXPECT_EQ(*it_p, p);

		p = stl::Partition::makeFine(3);
		it_p++;
		EXPECT_EQ(*it_p, p);
	}

	TEST(FourPointNotComplete, PDT_R) {
		std::vector<std::tuple<std::size_t, std::size_t, double>> edges;
		edges.push_back(std::make_tuple(0, 1, 1.0));
		edges.push_back(std::make_tuple(2, 3, 1.0));
		submodular::InfoCluster ic(edges, 4);
		ic.run_pdt_r();
		std::list<double> lambda_list = ic.get_critical_values();
		std::list<parametric::Partition> partition_list = ic.get_psp();
		std::list<double>::iterator it = lambda_list.begin();
		EXPECT_DOUBLE_EQ(*it, 0);
		it++;
		EXPECT_DOUBLE_EQ(*it, 1);

		EXPECT_EQ(lambda_list.size(), 2);
		EXPECT_EQ(partition_list.size(), 3);

		stl::Partition p = stl::Partition::makeDense(4);

		std::list<parametric::Partition>::iterator it_p = partition_list.begin();
		EXPECT_EQ(*it_p, p);

		it_p++;
		p.clear();
		p.AddElement(stl::CSet(std::string("1100")));
		p.AddElement(stl::CSet(std::string("0011")));
		EXPECT_EQ(*it_p, p);

		p = stl::Partition::makeFine(4);
		it_p++;
		EXPECT_EQ(*it_p, p);
	}
}
