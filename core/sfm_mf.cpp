// Copyright 2018 Kentaro Minami. All Rights Reserved.
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//     http://www.apache.org/licenses/LICENSE-2.0
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// =============================================================================

#ifndef SFM_SOLVER_H
#define SFM_SOLVER_H

#include "core/sfm_mf.h"

namespace submodular {


double SFMAlgorithm::GetMinimumValue() {
  return minimum_value_;
}


stl::CSet SFMAlgorithm::GetMinimizer() {
  return minimizer_;
}

void SFMAlgorithm::SetResults(double minimum_value, const Set& minimizer) {
    //minimum_value_ = minimum_value;
    //minimizer_ = minimizer;
	minimizer_ = minimizer;
	minimum_value_ = minimum_value;
    done_sfm_ = true;
}

void MF::Minimize(std::vector<double>& xl, double lambda_, Digraph* _g, ArcMap* _edge_map) {
	int graph_size = xl.size();

	lemon::ListDigraph::NodeMap<bool> node_filter(*_g);
	for (int i = 0; i <= graph_size; i++) {
		node_filter[_g->nodeFromId(i)] = true;
	}
	for (int i = graph_size + 1; i < lemon::countNodes(*_g); i++) {
		node_filter[_g->nodeFromId(i)] = false;
	}
	lemon::FilterNodes<Digraph> subgraph(*_g, node_filter);
	std::vector<double> sink_node_cost_map;
	sink_node_cost_map.resize(graph_size);
	for (lemon::FilterNodes<Digraph>::InArcIt a(subgraph, subgraph.nodeFromId(graph_size)); a != lemon::INVALID; ++a) {
		int node_id = subgraph.id(subgraph.source(a));
		sink_node_cost_map[node_id] = (*_edge_map)[a];
	}
	subgraph.disable(_g->nodeFromId(graph_size));

	// construct source node and sink node (inplace)
	Digraph::Node source_node = subgraph.addNode();
	Digraph::Node sink_node = subgraph.addNode();

	// construct edge cost map related with source_node and sink_node
	double const_difference = lambda_;
	for (int i = 0; i < graph_size; i++) {
		Digraph::Arc arc;
		if (xl[i] < 0) {
			arc = subgraph.addArc(source_node, subgraph.nodeFromId(i));
			(*_edge_map)[arc] = -xl[i];
		}
		double sink_cost = std::max<double>(0, xl[i]) + sink_node_cost_map[i];
		if (sink_cost > 0) {
			arc = subgraph.addArc(subgraph.nodeFromId(i), sink_node);
			(*_edge_map)[arc] = sink_cost;
		}
		const_difference += std::max<double>(0, xl[i]);
	}
	subgraph.enable(source_node);
	subgraph.enable(sink_node);
	lemon::Preflow<Digraph, ArcMap, PreflowSubgraphTraits> pf(subgraph, *_edge_map, source_node, sink_node);
	pf.run();
	double minimum_value = pf.flowValue() - const_difference;
	stl::CSet X;
	for (int v = 0; v < graph_size; ++v) {
		if (!pf.minCut(subgraph.nodeFromId(v)))
			X.AddElement(v);
	}
	// house keeping, map is handled automatically

#ifdef  _DEBUG	
	subgraph.disable(source_node);
	subgraph.disable(sink_node);
	stl::CSet _X;
	std::copy(X.begin(), X.end(), std::back_inserter(_X));
	_X.AddElement(graph_size);
	subgraph.enable(_g->nodeFromId(graph_size));
	double minimum_value_2 = -lambda_ + get_cut_value(subgraph, *_edge_map, _X);
	for (int i : X.GetMembers())
		minimum_value_2 -= xl[i];
	if (std::abs(minimum_value - minimum_value_2) > 1e-5) {
		std::cout << "maxflow value differs: " << minimum_value << " != " << minimum_value_2 << std::endl;
		// dump the graph in graph_dump.txt(current directory) with lemon graph format
		std::ofstream fout("graph_dump.txt");
		lemon::digraphWriter(subgraph, fout).
			arcMap("capacity", *_edge_map)
			.node("source", source_node)
			.node("target", sink_node).
			run();
		fout.close();
		exit(0);
	}
#endif
	subgraph.erase(source_node);
	subgraph.erase(sink_node);
	this->SetResults(minimum_value, X);
}

void BruteForce::Minimize(std::vector<double>& xl, double lambda_, Digraph* _g, ArcMap* _edge_map) {
	int n_ground = xl.size();
	stl::CSet minimizer;

	double min_value = std::numeric_limits<double>::max();
	stl::CSet fixed;
	fixed.AddElement(n_ground);
	for (unsigned long i = 0; i < (1 << n_ground); ++i) {
		stl::CSet X(n_ground, i);
		// divide it by 2 !		
		double new_value = get_cut_value(*_g, *_edge_map, X.Union(fixed));
		for (int j : X)
			new_value -= xl[j];

		if (new_value < min_value) {
			min_value = new_value;
			minimizer = X;
		}
	}
	min_value -= lambda_;

	stl::CSet minimizer_set;
	for (int i : minimizer)
		minimizer_set.AddElement(i);
	this->SetResults(min_value, minimizer_set);
}
}

#endif
