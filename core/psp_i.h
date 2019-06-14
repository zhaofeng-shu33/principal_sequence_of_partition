#pragma once
#include <lemon/list_graph.h>
#include "set/set_stl.h"
namespace psp {
	class DilworthTruncation {
	public:
		typedef lemon::FilterNodes<lemon::ListDigraph> Digraph;
		typedef typename lemon::ListDigraph::ArcMap<double> ArcMap;
		typedef Digraph::Node Node;
		DilworthTruncation(double lambda, Digraph* g, ArcMap* edge_map);
		double get_min_value();
		stl::Partition& get_min_partition();
		double evaluate(stl::Partition& partition);
		void run();
	private:
		void minimize(std::vector<double>& xl);
		Digraph* _g;
		ArcMap* _edge_map;
		double min_value;
		stl::CSet Tl;
		stl::Partition _partition;
		std::vector<Node> enabled_nodes;
		double lambda_;
		int node_size;
	};
	class DilworthTruncation_Improved {
	public:
		typedef lemon::ListDigraph Digraph;
		typedef typename lemon::ListDigraph::ArcMap<double> ArcMap;
		typedef Digraph::Node Node;
		DilworthTruncation_Improved(double lambda, Digraph* g, ArcMap* edge_map);
		double get_min_value();
		stl::Partition& get_min_partition();
		double evaluate(stl::Partition& partition);
		void run();
	private:
		void minimize(int i);
		Digraph* _g;
		ArcMap* _edge_map;
		double min_value;
		stl::CSet Tl;
		std::vector<double> xl;
		stl::Partition _partition;
		double lambda_;
		int node_size;
	};
	/**
	* improved version of principal sequence of partition
	*/
	class PSP {
	public:
		typedef lemon::ListDigraph Digraph;
		typedef Digraph::Node Node;
		typedef Digraph::Arc Arc;
		typedef typename Digraph::ArcMap<double> ArcMap;
		//! contract node set S to a single node i and remove duplicate arcs, requiring i in S
		void contract(const stl::CSet& S, int i);
		PSP(Digraph* g, ArcMap* edge_map);
		std::list<double>& get_critical_values();
		std::list<stl::Partition>& get_psp();
		void run();
	private:
		void split(int i);
		int node_size;
		std::vector<stl::CSet> K;
		Digraph tree;
		ArcMap tree_edge_map;
		Digraph* _g;
		ArcMap* _edge_map;
		std::list<double> critical_values;
		std::list<stl::Partition> psp_list;
		lemon::Tolerance<double> _tolerance;
	};
}