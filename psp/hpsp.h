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
    std::list<double>& get_critical_values();
    std::list<stl::Partition>& get_psp();
    void run();
private:
    void split(int s);
    void psp_construct();
    void merge(std::list<int>& C, std::map<int, std::vector<int>>& D);
    void contract(const stl::CSet& S, int i);
    int node_size;
    std::vector<int> K; 
    std::vector<double> W;    
    Digraph* _g;
    ArcMap* _edge_map;
    lemon::ListDigraph::NodeMap<bool> node_filter;
    lemon::FilterNodes<Digraph> subgraph;
    lemon::Tolerance<double> _tolerance;
    std::list<double> critical_values;
    std::list<stl::Partition> psp_list;
};
}