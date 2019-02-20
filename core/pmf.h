#pragma once
#include <tuple>
#include <numeric>
#include <iostream>

#include <lemon/list_graph.h>
#include "core/parametric_preflow.h"
//#include "core/set_utils.h"
#include "set/set_stl.h"
using stl::Set;
typedef std::pair<double, double> pair;
double compute_lambda(const std::vector<pair>& parameter_list, const double target_value);
namespace submodular{
class PMF {
public:
    typedef lemon::ListDigraph::ArcMap<double> ArcMap;
    PMF(lemon::ListDigraph& g, ArcMap& arcMap, std::vector<pair>& y_lambda):
        g_ptr(&g), aM(&arcMap),
        _y_lambda(y_lambda),
        dig_aM(dig),
        pf(dig, dig_aM, lemon::INVALID, lemon::INVALID){}
    void run();
    void insert(double lambda);
    void insert_set(Set s);
    std::list<Set> get_set_list() { return set_list; }
    std::list<double> get_lambda_list() { return lambda_list; }
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
};
class PDT {

};
}