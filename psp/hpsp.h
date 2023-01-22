#pragma once
// #include "psp/psp_i.h"
#include <lemon/list_graph.h>
#include <lemon/tolerance.h>
#include <lemon/adaptors.h>
#include "set/set_stl.h"
namespace psp{
class HPSP {
public:
    typedef lemon::ListDigraph Digraph;
    typedef Digraph::Node Node;
    typedef Digraph::Arc Arc;
    typedef typename Digraph::ArcMap<double> ArcMap;
    HPSP(Digraph* g, ArcMap* edge_map);
    std::vector<double>& get_W();
    std::vector<int>& get_K();
    void run();
private:
    void split(int s);
    void contract(const stl::CSet& S, int i);
    std::vector<int> K; 
    std::vector<double> W;    
    Digraph* _g;
    ArcMap* _edge_map;
    lemon::ListDigraph::NodeMap<bool> node_filter;
    lemon::FilterNodes<Digraph> subgraph;
    lemon::Tolerance<double> _tolerance;
};
}