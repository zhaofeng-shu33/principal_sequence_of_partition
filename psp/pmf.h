#pragma once
#include <tuple>
#include <numeric>
#include <iostream>
#include <lemon/list_graph.h>
#include <unordered_set>
#include <lemon/preflow.h>
#include "set/set_stl.h"

typedef std::pair<double, double> pair;
namespace parametric{
    typedef lemon::ListDigraph::ArcMap<double> ArcMap;

    typedef stl::Partition Partition;

    class PMF {
    public:
        using Set = stl::CSet;
        typedef lemon::Preflow<lemon::ListDigraph, ArcMap> Preflow;
        typedef typename lemon::FilterNodes<lemon::ListDigraph> SubDigraph;
        typedef Preflow::FlowMap FlowMap;
        typedef Preflow::Elevator Elevator;
        PMF(lemon::ListDigraph* g, ArcMap* arcMap, std::size_t j, std::vector<pair>& y_lambda);
        void run();
        std::list<Set> get_set_list();
        std::list<double> get_lambda_list();
        void reset_j(std::size_t j);
        void reset_y_lambda(std::vector<pair> parameter_list);
        double compute_lambda(const std::vector<pair>& parameter_list, const double target_value);
        void set_node_filter(bool value);
    private:
        void update_dig(double lambda);
        void slice(Set& S, Set& T, const FlowMap& arcMap, double lambda_1, double lambda_3);
        //! modify flow given current dig_aM
        void modify_flow(const FlowMap& flowMap, FlowMap& newFlowMap);
        inline Set get_min_cut_sink_side(Preflow& pf);
        double compute_lambda_eq_const(Set& S, Set& T);
    private:    
        lemon::ListDigraph* g_ptr;
        SubDigraph sub_digraph;
        lemon::ListDigraph::NodeMap<bool> node_filter;
        ArcMap* aM;
        lemon::ListDigraph dig; //directed graph
        ArcMap dig_aM; //directed graph arcMap
        std::vector<pair> _y_lambda;
        std::vector<double> sink_capacity;
        std::list<double> lambda_list;
        std::list<Set> set_list;
        lemon::ListDigraph::Node source_node;
        lemon::ListDigraph::Node sink_node;
        int tilde_G_size;
        lemon::Tolerance<double> tolerance;
        std::size_t _j;
    };

    class PDT {
    public:
        typedef lemon::ListDigraph::ArcMap<double> ArcMap;
        PDT(lemon::ListDigraph* g, ArcMap* arcMap);
        void run();
        std::list<double> get_critical_values();
        std::list<Partition> get_psp();
    protected:
        std::list<double> Lambda_list;
        std::list<Partition> partition_list;
    private:
        std::vector<pair> _y_lambda;
        lemon::ListDigraph* _g;
        ArcMap* _arcMap;
        PMF pmf;
    };


}
