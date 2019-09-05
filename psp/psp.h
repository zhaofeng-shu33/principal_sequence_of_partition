#pragma once
#include <lemon/list_graph.h>
#include "set/set_stl.h"

namespace psp {
    typedef stl::Partition Partition;
    class PSP {
    public:
        typedef lemon::ListDigraph Digraph;
        typedef Digraph::ArcMap<double> ArcMap;
        PSP(const std::vector<std::tuple<std::size_t, std::size_t, double>>& elt, int np);
        PSP(std::vector<int> s_list, std::vector<int> t_list, std::vector<double> weight_list, int np);
        virtual  ~PSP();
        //! get the partition which has at least pn clusters
        //! rerun the total algorithm, use with caution.
        std::vector<int> get_labels(int pn);
        Partition get_partition(int pn);
        void run();
        void run_pdt();
        void run_pdt_r();
        void run_psp_i();
        std::list<double>& get_gamma_list();
        std::list<double> get_critical_values();
        std::vector<double> get_critical_value_vector();
        std::vector<int> get_partitions();
        std::list<Partition>& get_psp();
        //! get the smallest partition whose size >= k
        Partition& get_smallest_partition(int k);
        //! get the smallest partition whose size >= k, label each data point with an integer
        std::vector<int> get_category(int k);
    protected:
        Digraph* g;
        ArcMap* edge_map;
        int num_points;
    private:
        std::list<double> gamma_list;
        
        std::list<Partition> psp_list;
        //! form conversion
        std::vector<int> to_category(Partition& partition);
    };
}
