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

    class PMF_R {
    public:
        using Set = stl::CSet;
        typedef lemon::Preflow<lemon::ListDigraph, ArcMap> Preflow;
        typedef std::map<int, std::map<int, double>> FlowMap;
        typedef Preflow::Elevator Elevator;
        PMF_R(lemon::ListDigraph* g, ArcMap* arcMap, std::size_t j, std::vector<pair>& y_lambda);
		PMF_R();
        void run();
        std::list<Set> get_set_list() { return set_list; }
        std::list<double> get_lambda_list() { return lambda_list; }
        void reset_j(std::size_t j);
        void reset_y_lambda(std::vector<pair> parameter_list) { 
            _y_lambda = parameter_list; 
            sink_capacity.resize(_y_lambda.size());
        }
        double compute_lambda(const std::vector<pair>& parameter_list, const double target_value);
		void set_source_node_id(int new_id);
		void set_sink_node_id(int new_id);
		void set_tilde_G_size(int new_size);
		double contract(const Set& S, const Set& T, lemon::ListDigraph& G, ArcMap& arcMap);
		void set_flowMap(const lemon::ListDigraph& G, const Preflow& p, FlowMap& flowMap);
		void get_preflow_flowMap(const lemon::ListDigraph& G, const FlowMap& flowMapDic, Preflow::FlowMap& flowMap);
    private:    
        void update_dig(double lambda);
        void slice(Set& T_l, Set& T_r, const FlowMap& arcMap, double lambda_1, double lambda_3);
        //! modify flow given current dig_aM
        void modify_flow(const lemon::ListDigraph& G, const ArcMap& capMap, const FlowMap& flowMap, FlowMap& newFlowMap);
        inline Set get_min_cut_sink_side(Preflow& pf);
        double compute_lambda_eq_const(Set& S, Set& T);
		inline void addArc(int u, int v, double w, lemon::ListDigraph& G, ArcMap& arcMap);
		//! convert Preflow flowMap to double dic flowMap
    private:    
        lemon::ListDigraph* g_ptr;
        ArcMap* aM;
        std::vector<pair> _y_lambda;
        std::vector<double> sink_capacity;
        std::list<double> lambda_list;
        std::list<Set> set_list;
        lemon::ListDigraph::Node source_node;
        lemon::ListDigraph::Node sink_node;
        int tilde_G_size;
		int source_node_id;
		int sink_node_id;
        lemon::Tolerance<double> tolerance;
        std::size_t _j;
	public:
		lemon::ListDigraph dig; //directed graph
		ArcMap dig_aM; //directed graph arcMap
    };
}