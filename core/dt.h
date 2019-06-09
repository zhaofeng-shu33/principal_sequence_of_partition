#pragma once
/**
*   Example from Literature "Minimum Average Cost Clustering"
**/
#include <lemon/list_graph.h>
#include "core/sfm_mf.h"
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

        DilworthTruncation(double lambda, Digraph* g, ArcMap* edge_map);
        double get_min_value();
        stl::Partition& get_min_partition();
        double evaluate(stl::Partition& partition);
        void run(bool bruteForce = false);
    private:
		Digraph* _g;
		ArcMap* _edge_map;
        double min_value;
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
		stl::Partition split(stl::Partition& Q, stl::Partition& P, int partition_num, bool bruteForce = false);
        
        //! |Q| < |P|
        void split(stl::Partition& Q, stl::Partition& P, bool bruteForce = false);

		stl::Partition run(int partition_num, bool bruteForce = false);
        
        void run(bool bruteForce = false);
        
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
