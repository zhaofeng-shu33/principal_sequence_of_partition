#pragma once
#include <tuple>
#include <numeric>
#include <iostream>

#include <lemon/list_graph.h>
#include "core/parametric_preflow.h"
#include <unordered_set>
#include "set/set_stl.h"
typedef std::pair<double, double> pair;
namespace parametric{
class PMF {
public:
    using Set = stl::CSet;
    typedef lemon::ListDigraph::ArcMap<double> ArcMap;
    PMF(lemon::ListDigraph& g, ArcMap& arcMap, std::size_t j, std::vector<pair>& y_lambda);
    void run();
    void insert(double lambda);
    void insert_set(Set s);
    std::list<Set> get_set_list() { return set_list; }
    std::list<double> get_lambda_list() { return lambda_list; }
    void reset_j(std::size_t j);
    void reset_y_lambda(std::vector<pair> parameter_list) { 
        _y_lambda = parameter_list; 
        sink_capacity.resize(_y_lambda.size());
    }
    double compute_lambda(const std::vector<pair>& parameter_list, const double target_value);
private:    
    void update_dig(double lambda);
    void slice(Set& S, Set& T);
    inline Set get_min_cut_source_side();
    double compute_lambda_eq_const(Set& S, Set& T);
    lemon::ListDigraph* g_ptr;
    ArcMap* aM;
    lemon::ListDigraph dig;
    ArcMap dig_aM;
    std::vector<pair> _y_lambda;
    std::vector<double> sink_capacity;
    std::list<double> lambda_list;
    std::list<Set> set_list;
    lemon::ListDigraph::Node source_node;
    lemon::ListDigraph::Node sink_node;
    int tilde_G_size;
    lemon::Preflow<lemon::ListDigraph, ArcMap> pf;
    std::size_t _j;
};
class Partition : public stl::Set<stl::CSet> {
public:
    static Partition makeFine(int size);
    static Partition makeDense(int size);
    Partition(){}
    Partition expand(const stl::CSet& A);
};
class PDT {
public:
    typedef lemon::ListDigraph::ArcMap<double> ArcMap;
    PDT(lemon::ListDigraph& g, ArcMap& arcMap);
    PDT(const PDT& another_pdt);
    void run();
    std::list<double> get_lambda_list(){ return Lambda_list; }
    std::list<Partition> get_partition_list() {
        return partition_list;
    }
protected:
    std::list<double> Lambda_list;
    std::list<Partition> partition_list;
private:
    std::vector<pair> _y_lambda;
    lemon::ListDigraph* _g;
    ArcMap* _arcMap;
    PMF pmf;
};
PDT* make_pdt(std::size_t num_nodes, std::vector<std::tuple<std::size_t, std::size_t, double>>& edges);

//! compute the graph cut with S as source side
template<typename DiGraph, typename ValueType = double>
ValueType compute_cut(const DiGraph& g, const typename DiGraph::template ArcMap<ValueType>& map, stl::CSet& S) {
    ValueType target_value = 0;
    stl::CSet S_Complement = S.Complement(g.maxNodeId() + 1);
    for (size_t i : S_Complement) {
        for (lemon::ListDigraph::InArcIt arc(g, g.nodeFromId(i)); arc != lemon::INVALID; ++arc) {
            if (S.HasElement(g.id(g.source(arc))))
                target_value += map[arc];
        }
    }
    return target_value;
}
}