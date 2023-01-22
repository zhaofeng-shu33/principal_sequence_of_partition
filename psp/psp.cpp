#include "psp/psp.h"
#include "psp/dt.h"
#include "psp/pmf.h"
#include "psp/psp_i.h"
#include "psp/pmf_r.h"
#ifdef AGG
#include "psp/agglomerative.h"
#endif
namespace psp {
    PSP::PSP(const std::vector<std::tuple<std::size_t, std::size_t, double>>& elt, int np) {
        num_points = np;
        g = new Digraph();
        edge_map = new ArcMap(*g);
        submodular::make_dgraph(np, elt, *g, *edge_map);
    }
    PSP::PSP(std::vector<int> s_list, std::vector<int> t_list, std::vector<double> weight_list, int np) {
        num_points = np;
        g = new Digraph();
        edge_map = new ArcMap(*g);
        std::vector<std::tuple<std::size_t, std::size_t, double>> elt;
        for (int i = 0; i < s_list.size(); i++) {
            elt.push_back(std::make_tuple(s_list[i], t_list[i], weight_list[i]));
        }
        submodular::make_dgraph(num_points, elt, *g, *edge_map);
    }
    PSP::PSP(Digraph* _g, ArcMap* _edge_map) : g(_g), edge_map(_edge_map), num_points(0){}
    PSP::~PSP() {
        // do not free the memory if external graph structure and map are used
        if (num_points > 0) {
            delete edge_map;
            delete g;
        }
    }
    void PSP::run(std::string method) {
        if (method == "dt")
            run_dt();
        else if (method == "pdt")
            run_pdt();
        else if (method == "pdt_r")
            run_pdt_r();
        else if (method == "psp_i")
            run_psp_i();
#ifdef AGG
        else if (method == "agg_psp")
            run_agg_psp();
#endif
        else
            throw std::logic_error(method + " not allowed");
    }
    void PSP::run_dt() {
        submodular::DT psp_class(g, edge_map);
        psp_class.run();
        gamma_list = psp_class.get_critical_values();
        psp_list = psp_class.get_psp();
    }
    void PSP::run_pdt() {
        parametric::PDT pdt(g, edge_map);
        pdt.run();
        gamma_list = pdt.get_critical_values();
        psp_list = pdt.get_psp();
    }
    void PSP::run_pdt_r() {
        parametric::PDT_R pdt(g, edge_map);
        pdt.run();
        gamma_list = pdt.get_critical_values();
        psp_list = pdt.get_psp();
    }
    void PSP::run_psp_i() {
        PSP_I psp_class(g, edge_map);
        psp_class.run();
        gamma_list = psp_class.get_critical_values();
        psp_list = psp_class.get_psp();
    }
#ifdef AGG    
    void PSP::run_agg_psp() {
        Agglomerative_PSP psp_class(g, edge_map);
        psp_class.run();
        gamma_list = psp_class.get_critical_values();
        psp_list = psp_class.get_psp();
    }
#endif
    std::vector<double> PSP::get_critical_values() {
        std::vector<double> gamma_vector(gamma_list.size());
        std::copy(gamma_list.begin(), gamma_list.end(), gamma_vector.begin());
        return gamma_vector;
    }

    std::vector<Partition> PSP::get_partitions() {
        std::vector<Partition> partitions;
        for (std::list<stl::Partition>::iterator it = psp_list.begin(); it != psp_list.end(); it++) {
            partitions.push_back(*it);
        }
        return partitions;
    }
}