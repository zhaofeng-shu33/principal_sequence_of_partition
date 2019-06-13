#pragma once
#include <lemon/list_graph.h>
#include "core/graph/graph.h"

namespace submodular {
    class InfoCluster {
    public:
		typedef lemon::ListDigraph Digraph;
		typedef Digraph::ArcMap<double> ArcMap;
		InfoCluster(const std::vector<std::tuple<std::size_t, std::size_t, double>>& elt, int np);
		InfoCluster(std::vector<int> s_list, std::vector<int> t_list, std::vector<double> weight_list, int np);
		virtual  ~InfoCluster();
        //! get the partition which has at least pn clusters
        //! rerun the total algorithm, use with caution.
		std::vector<int> get_labels(int pn);
		stl::Partition get_partition(int pn);
		void run();
		void run_bruteForce();
		void run_pdt();
		void run_psp_i();
		std::list<double>& get_gamma_list();
		std::list<double> get_critical_values();
		std::vector<double> get_critical_value_vector();
		std::vector<int> get_partitions();
		std::list<stl::Partition>& get_psp();
        //! get the smallest partition whose size >= k
		stl::Partition& get_smallest_partition(int k);
        //! get the smallest partition whose size >= k, label each data point with an integer
		std::vector<int> get_category(int k);
    protected:
		Digraph* g;
		ArcMap* edge_map;
        int num_points;
    private:
        std::list<double> gamma_list;
        
        std::list<stl::Partition> psp_list;
        //! form conversion
		std::vector<int> to_category(stl::Partition& partition);
    };
}
