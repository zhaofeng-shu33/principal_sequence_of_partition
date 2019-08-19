#pragma once
/**
*   Example from Literature "Minimum Average Cost Clustering"
**/
#include <lemon/list_graph.h>
#include <lemon/preflow.h>
#include "core/graph/graph.h"
#include "set/set_stl.h"
namespace submodular {
 
    /**
    *   compute the solution to \min_{P} h_{\gamma}(P)
    */
    class DilworthTruncation {
    public:
        typedef lemon::ListDigraph Digraph;
		typedef typename Digraph::ArcMap<double> ArcMap;
		typedef typename lemon::FilterNodes<Digraph> SubDigraph;
		typedef typename lemon::PreflowDefaultTraits<SubDigraph, ArcMap> PreflowSubgraphTraits;
		typedef typename lemon::Preflow<Digraph, ArcMap, PreflowSubgraphTraits> Preflow;
	
        DilworthTruncation(double lambda, Digraph* g, ArcMap* edge_map);
        double get_min_value();
        stl::Partition& get_min_partition();
        double evaluate(stl::Partition& partition);
        void run();
    private:
		void minimize(std::vector<double>& xl);
        Digraph* _g;
        ArcMap* _edge_map;
        double min_value;
		stl::CSet Tl;
        stl::Partition _partition;
        double lambda_;
        int NodeSize;
    };

    /**
    * computing principal sequence of partition for given submodular function
    */
    class PSP {
    public:
        typedef lemon::ListDigraph Digraph;
        typedef typename Digraph::ArcMap<double> ArcMap;
        PSP(Digraph* g, ArcMap* edge_map);
        //! evaluate and find the finest partition with $\abs{\P} > \texttt{partition_num}$
        stl::Partition split(stl::Partition& Q, stl::Partition& P, int partition_num);
        
        //! |Q| < |P|
        void split(stl::Partition& Q, stl::Partition& P);

        stl::Partition run(int partition_num);
        
        void run();
        
        std::list<double>& get_critical_values();
        std::list<stl::Partition>& get_psp();
    private:
        //! evalute the submodular function at the given partition
        double evaluate(const stl::Partition& P);

        int NodeSize;
		lemon::Tolerance<double> _tolerance;
        std::list<double> critical_values;
        std::list<stl::Partition> psp;
		Digraph* _g;
		ArcMap* _edge_map;
    };
}
