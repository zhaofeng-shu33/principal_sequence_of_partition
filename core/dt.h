#pragma once
/**
*   Example from Literature "Minimum Average Cost Clustering"
**/
#include <lemon/list_graph.h>
#include "config.h"
#if USE_LEMON
#include "core/sfm_mf.h"
#include "core/graph/graph.h"
#include "set/set_stl.h"
#endif
namespace submodular {
 
    /**
    *   compute the solution to \min_{P} h_{\gamma}(P)
    */
    class DilworthTruncation {
    public:
		typedef lemon::ListDigraph Digraph;
		typedef typename Digraph::ArcMap<double> ArcMap;
        
        DilworthTruncation(double lambda, Digraph* g, ArcMap* edge_map);
        double Get_min_value();
        stl::Partition& Get_min_partition();
        double evaluate(stl::Partition& partition);
        void Run(bool bruteForce = false);
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
        PSP(Digraph* g, ArcMap* edge_map );
        //! evaluate and find the finest partition with $\abs{\P} > \texttt{partition_num}$
		stl::Partition split(stl::Partition& Q, stl::Partition& P, int partition_num, bool bruteForce = false);
        
        //! |Q| < |P|
        void split(stl::Partition& Q, stl::Partition& P, bool bruteForce = false);

		stl::Partition run(int partition_num, bool bruteForce = false);
        
        void run(bool bruteForce = false);
        
        std::vector<double>& Get_critical_values();
        std::vector<stl::Partition>& Get_psp();
    private:
        //! evalute the submodular function at the given partition
        double evaluate(const stl::Partition& P);

        int NodeSize;
        std::vector<double> critical_values;
        std::vector<stl::Partition> psp;
		Digraph* _g;
		ArcMap* _edge_map;
    };
}
