#include <lemon/adaptors.h>
#include "core/psp_i.h"
#include "core/dt.h"
#include "core/sfm_mf.h"

namespace psp {
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
		submodular::SFMAlgorithm* solver2 = new submodular::MF;
		for (int i = 0; i < node_size; i++) {
			solver2->Minimize(xl, lambda_, _g, _edge_map);
			alpha_l = solver2->GetMinimumValue();
			stl::CSet Tl = solver2->GetMinimizer();
			Tl.AddElement(i);
			_partition = _partition.expand(Tl);
			xl.push_back(alpha_l);
		}
		for (auto it = xl.begin(); it != xl.end(); it++) {
			min_value += *it;
		}
		delete solver2;
	}

	double DilworthTruncation::evaluate(stl::Partition & partition) {
		double result = get_partition_value(*_g, *_edge_map, partition);
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
		stl::CSet Q = K[i];
		lemon::ListDigraph::NodeMap<bool> node_filter(*_g);
		int num_of_children = 0;
		for (Digraph::OutArcIt a(tree, tree.nodeFromId(i)); a != lemon::INVALID; a++) {
			stl::CSet s = K[tree.id(tree.target(a))];
			int j = *s.begin();
			node_filter[_g->nodeFromId(j)] = true;
			num_of_children ++;
		}
		lemon::FilterNodes<Digraph> subgraph(*_g, node_filter);
		double weight_total = 0;
		for (lemon::FilterNodes<Digraph>::ArcIt a(subgraph); a != lemon::INVALID; ++a)
			weight_total += (*_edge_map)[a];
		double gamma_apostrophe = weight_total / (num_of_children - 1);
		DilworthTruncation dt(gamma_apostrophe, &subgraph, _edge_map);
	}
}