#pragma once
#include <lemon/list_graph.h>
#include "psp/set/set_stl.h"

namespace psp {
    typedef stl::Partition Partition;
    class PSP {
    public:
        typedef lemon::ListDigraph Digraph;
        typedef Digraph::ArcMap<double> ArcMap;
        /// \brief Constructor
        ///
        /// \param arc_triple_list arc triple list (i, j, w)
        /// represents the first node index, the second node index and the edge weight
        /// between the two nodes.
        /// \param number_of_node the number of node in the graph
        PSP(const std::vector<std::tuple<std::size_t, std::size_t, double>>& arc_triple_list, int number_of_node);
        PSP(std::vector<int> s_list, std::vector<int> t_list, std::vector<double> weight_list, int number_of_node);
        PSP(Digraph* _g, ArcMap* _edge_map);
        virtual  ~PSP();
        //! run principal sequence of partition algorithm
        //! default to use improved psp,
        //! which is empirically the fastest method
        void run(std::string method = "psp_i");
        //! get the critical value list
        std::vector<double> get_critical_values();
        //! get the partition list
        std::vector<Partition> get_partitions();
    protected:
        Digraph* g;
        ArcMap* edge_map;
        int num_points;
    private:
        std::list<double> gamma_list;        
        std::list<Partition> psp_list;
        void run_dt();
        void run_pdt();
        void run_pdt_r();
        void run_psp_i();
        void run_agg_psp();
    };
}
