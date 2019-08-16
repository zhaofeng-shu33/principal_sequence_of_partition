#pragma once
#include <tuple>
#include <numeric>
#include <iostream>

#include <lemon/list_graph.h>
#include <unordered_set>
#include "set/set_stl.h"
#include "preflow/mf_base.h"
typedef std::pair<double, double> pair;
namespace parametric{
	typedef lemon::ListDigraph::ArcMap<double> ArcMap;

	typedef stl::Partition Partition;
    class PMF_R {

    public:
        using Set = stl::CSet;
        typedef lemon::Preflow_Relabel<lemon::ListDigraph, ArcMap> Preflow;
		typedef lemon::Preflow_Relabel<lemon::ReverseDigraph<lemon::ListDigraph>, ArcMap> Preflow_Reverse;
        typedef std::map<int, std::map<int, double>> FlowMap;
        typedef Preflow::Elevator Elevator;
		typedef Preflow_Reverse::Elevator Elevator_Reverse;
		struct ThreadArgumentPack {
			lemon::ListDigraph* newDig;
			ArcMap* newArcMap;
			FlowMap* flowMap;
			Set* S;
			Set* T;
			Set* T_apostrophe;
			double* new_flow_value;
			Elevator* ele;
			Elevator* ele_out = NULL;
			Elevator_Reverse* ele_reverse;
			Elevator_Reverse* ele_reverse_out = NULL;
			FlowMap* newFlowMap;
			ThreadArgumentPack(lemon::ListDigraph& newDig1, ArcMap& newArcMap1, FlowMap& flowMap1, Set& S1, Set& T1, Set& T_apostrophe_1, double& new_flow_value_1, 
				FlowMap& newflowMap1, Elevator* ele1, Elevator_Reverse* ele_reverse_1) :
				newDig(&newDig1), newArcMap(&newArcMap1), flowMap(&flowMap1),
				S(&S1), T(&T1), T_apostrophe(&T_apostrophe_1),
				new_flow_value(&new_flow_value_1), newFlowMap(&newflowMap1), 
				ele(ele1), ele_reverse(ele_reverse_1){}
		};
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
		void contract(const Set& S, const Set& T, lemon::ListDigraph& G, ArcMap& arcMap);
		//! convert Preflow flowMap to double dic flowMap
		void set_flowMap(const lemon::ListDigraph& G, const Preflow::FlowMap& pfm, FlowMap& flowMap);
		void get_preflow_flowMap(const lemon::ListDigraph& G, const FlowMap& flowMapDic, Preflow::FlowMap& flowMap);
		//! modify flow given current dig_aM
		void modify_flow(const Set& S, const Set& T, const lemon::ListDigraph& G, const ArcMap& capMap, const FlowMap& flowMap, FlowMap& newFlowMap);
		void executePreflow(ThreadArgumentPack& TAP);
		void executePreflow_reverse(ThreadArgumentPack& TAP);
    private:    
        void update_dig(double lambda);
        void slice(Set& T_l, Set& T_r, FlowMap& leftArcMap, FlowMap& rightArcMap, double lambda_1, double lambda_3, Elevator* left_ele, Elevator_Reverse* right_ele, bool is_contract=true);
        inline Set get_min_cut_sink_side(const lemon::ListDigraph& digraph, Preflow& pf);
		inline Set get_min_cut_sink_side_reverse(const lemon::ReverseDigraph<lemon::ListDigraph>& digraph, Preflow_Reverse& pf);
        double compute_lambda_eq_const(Set& S, Set& T);
		inline void addArc(int u, int v, double w, lemon::ListDigraph& G, ArcMap& arcMap);
		inline void addFlowArc(int u, int v, double w, FlowMap& flowMap);	
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

	class PDT_R {
	public:
		typedef lemon::ListDigraph::ArcMap<double> ArcMap;
		PDT_R(lemon::ListDigraph* g, ArcMap* arcMap);
		void run();
		std::list<double> get_critical_values() { return Lambda_list; }
		std::list<Partition> get_psp() {
			return partition_list;
		}
	protected:
		std::list<double> Lambda_list;
		std::list<Partition> partition_list;
	private:
		std::vector<pair> _y_lambda;
		lemon::ListDigraph* _g;
		ArcMap* _arcMap;
		PMF_R pmf;
	};
}