#pragma once
#include <lemon/lgf_reader.h>
#include "core/pmf_r.h"
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
		pmfR.set_flowMap(digraph, pf, newFlowMap);
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
		pmfR.set_flowMap(digraph, pf, flowMap);
		pmfR.modify_flow(S, T, new_digraph, new_cap, flowMap, newFlowMap);
		EXPECT_DOUBLE_EQ(newFlowMap[4][0], 1);
		EXPECT_DOUBLE_EQ(newFlowMap[4][3], 2);
		EXPECT_DOUBLE_EQ(newFlowMap[0][3], 1);
	}


}
