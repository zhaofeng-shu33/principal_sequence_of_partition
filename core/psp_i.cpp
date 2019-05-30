#include <lemon/adaptors.h>
#include "core/psp_i.h"
#include "core/dt.h"
#include "core/sfm_mf.h"

namespace psp {
	void DilworthTruncation::minimize(std::vector<double>& xl, double lambda_) {
		int graph_size = xl.size();

		for (int i = 0; i <= graph_size; i++) {
			_g->enable(_g->nodeFromId(i));
		}
		for (int i = graph_size + 1; i < lemon::countNodes(*_g); i++) {
			_g->disable(_g->nodeFromId(i));
		}
		std::vector<double> sink_node_cost_map;
		sink_node_cost_map.resize(graph_size);
		for (Digraph::InArcIt a(*_g, _g->nodeFromId(graph_size)); a != lemon::INVALID; ++a) {
			int node_id = _g->id(_g->source(a));
			sink_node_cost_map[node_id] = (*_edge_map)[a];
		}
		_g->disable(_g->nodeFromId(graph_size));

		// construct source node and sink node (inplace)
		Digraph::Node source_node = _g->addNode();
		Digraph::Node sink_node = _g->addNode();

		// construct edge cost map related with source_node and sink_node
		double const_difference = lambda_;
		for (int i = 0; i < graph_size; i++) {
			Digraph::Arc arc;
			if (xl[i] < 0) {
				arc = _g->addArc(source_node, _g->nodeFromId(i));
				(*_edge_map)[arc] = -xl[i];
			}
			double sink_cost = std::max<double>(0, xl[i]) + sink_node_cost_map[i];
			if (sink_cost > 0) {
				arc = _g->addArc(_g->nodeFromId(i), sink_node);
				(*_edge_map)[arc] = sink_cost;
			}
			const_difference += std::max<double>(0, xl[i]);
		}
		_g->enable(source_node);
		_g->enable(sink_node);
		lemon::Preflow<Digraph, ArcMap> pf(*_g, *_edge_map, source_node, sink_node);
		pf.run();
		min_value = pf.flowValue() - const_difference;
		stl::CSet X;
		for (int v = 0; v < graph_size; ++v) {
			if (!pf.minCut(_g->nodeFromId(v)))
				X.AddElement(v);
		}
		// house keeping, map is handled automatically

		_g->erase(source_node);
		_g->erase(sink_node);
		Tl = X;
	}

	DilworthTruncation::DilworthTruncation(double lambda, Digraph* g, ArcMap* edge_map) :
		lambda_(lambda), min_value(0), _g(g), _edge_map(edge_map)
	{
		node_size = lemon::countNodes(*_g);
	}
	double DilworthTruncation::get_min_value() {
		return min_value;
	}
	stl::Partition& DilworthTruncation::get_min_partition() {
		return _partition;
	}

	void DilworthTruncation::run() {
		min_value = 0;
		_partition.clear();
		std::vector<double> xl;
		double alpha_l = 0;
		for (int i = 0; i < node_size; i++) {
			minimize(xl, lambda_);
			alpha_l = min_value;
			Tl.AddElement(i);
			_partition = _partition.expand(Tl);
			xl.push_back(alpha_l);
		}
		min_value = 0;
		for (auto it = xl.begin(); it != xl.end(); it++) {
			min_value += *it;
		}
	}

	double DilworthTruncation::evaluate(stl::Partition & partition) {
		double result = submodular::get_partition_value(*_g, *_edge_map, partition);
		return result - lambda_ * partition.Cardinality();
	}


	PSP::PSP(Digraph* g, ArcMap* edge_map): _g(g), _edge_map(edge_map), tree_edge_map(tree){
		node_size = lemon::countNodes(*g);
	}
	void PSP::run() {
		Node root = tree.addNode();
		K.push_back(stl::CSet::MakeDense(node_size));
		for (int i = 0; i < node_size; i++) {
			Node leaf = tree.addNode();
			tree.addArc(root, leaf);
			stl::CSet s;
			s.AddElement(i);
			K.push_back(s);
		}
		split(0);
	}
	void PSP::split(int i) {
		stl::CSet& Q = K[i];
		lemon::ListDigraph::NodeMap<bool> node_filter(*_g);
		int num_of_children = 0;
		for (Digraph::OutArcIt a(tree, tree.nodeFromId(i)); a != lemon::INVALID; ++a) {
			stl::CSet& s = K[tree.id(tree.target(a))];
			int j = *s.begin();
			node_filter[_g->nodeFromId(j)] = true;
			num_of_children ++;
		}
		lemon::FilterNodes<Digraph> subgraph(*_g, node_filter);
		double weight_total = 0;
		for (lemon::FilterNodes<Digraph>::ArcIt a(subgraph); a != lemon::INVALID; ++a)
			weight_total += (*_edge_map)[a];
		double gamma_apostrophe = weight_total / (num_of_children - 1.0);
		DilworthTruncation dt(gamma_apostrophe, &subgraph, _edge_map);
		dt.run();
		double min_value = dt.get_min_value();
		if (min_value > -1 * gamma_apostrophe - _tolerance.epsilon()) {
			for (Digraph::OutArcIt a(tree, tree.nodeFromId(i)); a != lemon::INVALID; ++a) {
				tree_edge_map[a] = gamma_apostrophe;
			}
		}
		else {
			stl::Partition P_apostrophe = dt.get_min_partition();
		}
	}
}