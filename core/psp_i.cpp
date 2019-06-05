#include <queue>
#include <lemon/adaptors.h>
#include "core/psp_i.h"
#include "core/dt.h"
#include "core/sfm_mf.h"
#include <cassert>
namespace psp {
	void DilworthTruncation::minimize(std::vector<double>& xl) {
		int graph_size = xl.size();

		for (int i = 0; i <= graph_size; i++) {
			_g->enable(enabled_nodes[i]);
		}
		for (int i = graph_size + 1; i < node_size; i++) {
			_g->disable(enabled_nodes[i]);
		}
		std::map<int, double> sink_node_cost_map;
		for (Digraph::InArcIt a(*_g, enabled_nodes[graph_size]); a != lemon::INVALID; ++a) {
			int node_id = _g->id(_g->source(a));
			sink_node_cost_map[node_id] = (*_edge_map)[a];
		}
#ifdef  _DEBUG	
		for (Digraph::OutArcIt a(*_g, enabled_nodes[graph_size]); a != lemon::INVALID; ++a) {
			std::cout << _g->id(_g->target(a)) << std::endl;
			throw std::logic_error("no outarc allowed");
		}
#endif
		_g->disable(enabled_nodes[graph_size]);

		// construct source node and sink node (inplace)
		Digraph::Node source_node = _g->addNode();
		Digraph::Node sink_node = _g->addNode();

		// construct edge cost map related with source_node and sink_node
		double const_difference = lambda_;
		for (int i = 0; i < graph_size; i++) {
			Digraph::Arc arc;
			if (xl[i] < 0) {
				arc = _g->addArc(source_node, enabled_nodes[i]);
				(*_edge_map)[arc] = -xl[i];
			}
			double sink_cost = std::max<double>(0, xl[i]) + sink_node_cost_map[_g->id(enabled_nodes[i])];
			if (sink_cost > 0) {
				arc = _g->addArc(enabled_nodes[i], sink_node);
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
			if (!pf.minCut(enabled_nodes[v]))
				X.AddElement(_g->id(enabled_nodes[v]));
		}
		// house keeping, map is handled automatically
#ifdef  _DEBUG	
		stl::CSet _X;
		for (int v = 0; v < graph_size; ++v) {
			if (!pf.minCut(enabled_nodes[v]))
				_X.AddElement(v);
		}
		_g->disable(source_node);
		_g->disable(sink_node);
		_g->enable(enabled_nodes[graph_size]);
		double minimum_value_2 = -lambda_;
		for (int i : _X.GetMembers())
			minimum_value_2 -= xl[i];
		_X.clear();
		std::copy(X.begin(), X.end(), std::back_inserter(_X));
		_X.AddElement(_g->id(enabled_nodes[graph_size]));
		minimum_value_2 += submodular::get_cut_value(*_g, *_edge_map, _X);
		assert(std::abs(min_value - minimum_value_2) < 1e-5);
#endif
		_g->erase(source_node);
		_g->erase(sink_node);
		Tl = X;
	}

	DilworthTruncation::DilworthTruncation(double lambda, Digraph* g, ArcMap* edge_map) :
		lambda_(lambda), min_value(0), _g(g), _edge_map(edge_map)
	{
		for (Digraph::NodeIt n(*_g); n != lemon::INVALID; ++n)
			enabled_nodes.push_back(n);
		std::reverse(enabled_nodes.begin(), enabled_nodes.end());
		node_size = enabled_nodes.size();
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
			minimize(xl);
			alpha_l = min_value;
			Tl.AddElement(_g->id(enabled_nodes[i]));
			_partition = _partition.expand(Tl);
			xl.push_back(alpha_l);
		}
		min_value = 0;
		for (auto it = xl.begin(); it != xl.end(); it++) {
			min_value += *it;
		}
#ifdef _DEBUG 
		for (Node n : enabled_nodes) {
			_g->enable(n);
		}
		double min_value_check = evaluate(_partition);
		if (std::abs(min_value - min_value_check) > 1e-4) {
			std::cout << "num of nodes " << lemon::countNodes(*_g) << std::endl;
			for (Digraph::ArcIt a(*_g); a != lemon::INVALID; ++a) {
				std::cout << _g->id(_g->source(a)) << ' ' << _g->id(_g->target(a)) << std::endl;
			}
			for (Digraph::ArcIt a(*_g); a != lemon::INVALID; ++a)
				std::cout << (*_edge_map)[a] << std::endl;
			std::stringstream ss;
			ss << "min_value_check error: " << min_value << ' ' << min_value_check;
			throw std::logic_error(ss.str());
		}
#endif
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
		typedef std::pair<int, double> psp_pair;
		// take the smallest number from the queue
		std::vector<psp_pair> q; // decreasing
		Digraph::OutArcIt a(tree, root);
		q.push_back(std::make_pair(0, tree_edge_map[a]));
		while (!q.empty()) {
			stl::Partition p;
			for (const psp_pair& pair : q) {
				p.AddElement(K[pair.first]);
			}
			psp_list.push_back(p);

			psp_pair tmp = q.back();
			q.pop_back();
			if(tmp.second < INFINITY)
				critical_values.push_back(tmp.second);
			else {
				q.clear();
				break;
			}
			for (Digraph::OutArcIt a(tree, tree.nodeFromId(tmp.first)); a != lemon::INVALID; ++a) {
				Node child = tree.target(a);
				Digraph::OutArcIt a_child(tree, child);
				double inserted_lambda;
				if (a_child == lemon::INVALID){
					inserted_lambda = INFINITY;
				}
				else
					inserted_lambda = tree_edge_map[a_child];
				bool inserted = false;
				for (std::vector<psp_pair>::iterator it = q.begin(); it != q.end(); it++) {
					if (it->second < inserted_lambda) {
						q.insert(it, std::make_pair(tree.id(tree.target(a)), inserted_lambda));
						inserted = true;
						break;
					}
				}
				if (inserted == false) {
					q.push_back(std::make_pair(tree.id(tree.target(a)), inserted_lambda));
				}
			}
		}
	}
	void PSP::contract(const stl::CSet& S, int i) {
		// we don't use _g->contract function since it can not handle multiple arcs within two nodes
		std::map<int, double> capacity_map;
		for (Digraph::NodeIt n(*_g); n != lemon::INVALID; ++n)
			capacity_map[_g->id(n)] = 0;
		// iterate over all arcs
		for (Digraph::ArcIt a(*_g); a != lemon::INVALID; ++a) {
			int s_id = _g->id(_g->source(a));
			int t_id = _g->id(_g->target(a));
			if (S.HasElement(s_id) && !S.HasElement(t_id)) {
				capacity_map.at(t_id) += (*_edge_map)[a]; //out is positive
			}
			else if (!S.HasElement(s_id) && S.HasElement(t_id)) {
				capacity_map.at(s_id) -= (*_edge_map)[a];
			}
			if (s_id == i || t_id == i) { // delete all arcs connected with Node(i)
				_g->erase(a);
			}
		}
		//delete S\{i} in _g
		for (int j : S) {
			Node J = _g->nodeFromId(j);
			if (_g->valid(J) && j != i)
				_g->erase(J);
		}
		Node _S = _g->nodeFromId(i);
				
		for (const std::pair<int, double>& val : capacity_map) {
			if (S.HasElement(val.first) || std::abs(val.second) < _tolerance.epsilon())
				continue;
			Arc a;
			if (val.first > i) {
				a = _g->addArc(_S, _g->nodeFromId(val.first));
			}
			else {
				a = _g->addArc(_g->nodeFromId(val.first), _S);
			}
			(*_edge_map)[a] = std::abs(val.second);
		}
	}
	void PSP::split(int i) {
		lemon::ListDigraph::NodeMap<bool> node_filter(*_g);
		int num_of_children = 0;
		Node root = tree.nodeFromId(i);
		for (Digraph::OutArcIt a(tree, root); a != lemon::INVALID; ++a) {
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
		stl::Partition P_apostrophe;
		double min_value;
		if (num_of_children == 2) {
			goto add_lambda;
		}
		else{
			DilworthTruncation dt(gamma_apostrophe, &subgraph, _edge_map);
			dt.run();
			min_value = dt.get_min_value();
			P_apostrophe = dt.get_min_partition();
		}
		if (!(P_apostrophe.Cardinality() != 1 && P_apostrophe.Cardinality() != num_of_children && min_value < -1 * gamma_apostrophe - _tolerance.epsilon())) {
			add_lambda:
				for (Digraph::OutArcIt a(tree, root); a != lemon::INVALID; ++a) {
					tree_edge_map[a] = gamma_apostrophe;
				}
		}
		else {			
			for (const stl::CSet& S : P_apostrophe) {
				if (S.Cardinality() == 1)
					continue;
				stl::CSet S_apostrophe = S;
				int s = K.size();
				Node _S = tree.addNode();
				assert(tree.id(_S) == s);
				std::queue<Arc> a_q;
				for (Digraph::OutArcIt a(tree, root); a != lemon::INVALID; ++a) {
					int j = tree.id(tree.target(a));
					stl::CSet& K_j = K[j];
					if (K_j.Intersection(S).Cardinality() > 0) {
						a_q.push(a);
						S_apostrophe = S_apostrophe.Union(K_j);
					}
				}
				while (!a_q.empty()) {
					Arc a = a_q.front();
					tree.changeSource(a, _S);
					a_q.pop();
				}
				tree.addArc(root, _S);
				K.push_back(S_apostrophe);
				split(s);
				// contract the graph
				contract(S, *S.begin());
			}
			split(i);
		}
	}
	std::list<double>& PSP::get_critical_values() {
		return critical_values;
	}

	std::list<stl::Partition>& PSP::get_psp() {
		return psp_list;
	}
}