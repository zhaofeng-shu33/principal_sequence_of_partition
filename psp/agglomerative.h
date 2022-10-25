#pragma once
#include <lemon/tolerance.h>
#include <lemon/list_graph.h>
#include <Eigen/Core>
#include "set/set_stl.h"
namespace psp {
    class Agglomerative_PSP {
    public:
        typedef lemon::ListDigraph Digraph;
        typedef Digraph::Node Node;
        typedef Digraph::Arc Arc;
        typedef typename Digraph::ArcMap<double> ArcMap;
        Agglomerative_PSP(Digraph* g, ArcMap* edge_map);
        std::list<double>& get_critical_values();
        std::list<stl::Partition>& get_psp();
        void run();
    private:
        void add_partition(const std::vector<std::vector<size_t>>& q);
        // void contract(const stl::CSet& S, int i);
        int node_size;
        std::vector<stl::CSet> K; // denotes the index of the set (the node in the tree)
        Digraph tree;
        ArcMap tree_edge_map;
        Digraph* _g;
        ArcMap* _edge_map;
        Eigen::MatrixXd M;
        std::list<double> critical_values;
        std::list<stl::Partition> psp_list;
        lemon::Tolerance<double> _tolerance;
    };
}