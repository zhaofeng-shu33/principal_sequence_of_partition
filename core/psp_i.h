#pragma once
#include <lemon/list_graph.h>
#include "set/set_stl.h"
namespace psp {
	class DilworthTruncation {
	public:
		typedef lemon::FilterNodes<lemon::ListDigraph> Digraph;
		typedef typename Digraph::ArcMap<double> ArcMap;

		DilworthTruncation(double lambda, Digraph* g, ArcMap* edge_map);
		double get_min_value();
		stl::Partition& get_min_partition();
		double evaluate(stl::Partition& partition);
		void run();
	private:
		Digraph* _g;
		ArcMap* _edge_map;
		double min_value;
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
		typedef typename Digraph::ArcMap<double> ArcMap;
		PSP(Digraph* g, ArcMap* edge_map);
		void run();
	private:
		void split(int i);
		int node_size;
		std::vector<stl::CSet> K;
		Digraph tree;
		ArcMap tree_edge_map;
		Digraph* _g;
		ArcMap* _edge_map;
	};
}