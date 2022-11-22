#pragma once
#include <lemon/list_graph.h>
#include <lemon/tolerance.h>
#include "set/set_stl.h"
namespace psp {
    class SF {
	public:
        typedef lemon::ListDigraph Digraph;
        typedef typename Digraph::ArcMap<double> ArcMap;


        SF(Digraph* g, ArcMap* edge_map);
		/**
		The value of the submodular function evaluated on a set.
		@param B A subvector of elements of V.
		@return The value of the submodular function at B.
		*/
		double operator() (const stl::CSet &B);
		/**
		@return The size of the ground set.
		*/
		size_t size();
        
        void set_j(size_t new_j);

		private:
            Digraph* _g;
            ArcMap* _edge_map;
            size_t j;
	};
    /**
    * improved version of principal sequence of partition
    */
    class AGG_I {
    public:
        typedef lemon::ListDigraph Digraph;
        typedef Digraph::Node Node;
        typedef Digraph::Arc Arc;
        typedef typename Digraph::ArcMap<double> ArcMap;
        AGG_I(Digraph* g, ArcMap* edge_map);
        std::list<double>& get_critical_values();
        std::list<stl::Partition>& get_psp();
        void run();
    private:
        void contract(const stl::CSet& S, int i);
        void agglomerate();
        int node_size;
        std::vector<stl::CSet> K; // denotes the index of the set (the node in the tree)
        Digraph tree;
        ArcMap tree_edge_map;
        Digraph* _g;
        ArcMap* _edge_map;
        std::list<double> critical_values;
        stl::Partition current_partition;
        std::list<stl::Partition> psp_list;
        lemon::Tolerance<double> _tolerance;
    };
}