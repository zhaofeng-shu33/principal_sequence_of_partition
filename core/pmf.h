#pragma once
#include <tuple>
#include <numeric>
#include <iostream>

#include <lemon/list_graph.h>
#include "core/parametric_preflow.h"
#include <unordered_set>
#include "set/set_stl.h"
typedef std::pair<double, double> pair;
double compute_lambda(const std::vector<pair>& parameter_list, const double target_value);
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
    void reset_y_lambda(std::vector<pair> parameter_list) { _y_lambda = parameter_list; }
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
    Partition expand(stl::CSet& A)  {
        Partition p;
        stl::CSet A_extend = A;
        for (auto it = begin(); it != end(); it++) {
            if (it->Intersection(A).IsEmpty()) {
                p.AddElement(*it);
            }
            else {
                A_extend = A_extend.Union(*it);
            }
        }
        p.AddElement(A_extend);
        return p;
    }
};
class PDT {
public:
    typedef lemon::ListDigraph::ArcMap<double> ArcMap;
    PDT(lemon::ListDigraph& g, ArcMap& arcMap) {}
    void run() {}
private:
    std::vector<pair> _y_lambda;
    std::list<double> Lambda_list;
    std::list<Partition> partition_list;
};
}