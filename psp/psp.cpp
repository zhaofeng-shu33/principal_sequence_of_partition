#include "psp/psp.h"
#include "psp/dt.h"
#include "psp/pmf.h"
#include "psp/psp_i.h"
#include "psp/pmf_r.h"

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
    PSP::~PSP() {
        delete edge_map;
        delete g;
    }
    std::vector<int> PSP::get_labels(int pn) {
        stl::Partition p = get_partition(pn);
        return to_category(p);
    }
    stl::Partition PSP::get_partition(int pn) {
        submodular::DT psp_class(g, edge_map);
        stl::Partition p = psp_class.run(pn);
        return p;
    }
    void PSP::run() {
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
    std::list<double>& PSP::get_gamma_list() {
        return gamma_list;
    }
    std::list<double> PSP::get_critical_values() {
        return gamma_list;
    }
    std::vector<double> PSP::get_critical_value_vector() {
        std::vector<double> gamma_vector(gamma_list.size());
        std::copy(gamma_list.begin(), gamma_list.end(), gamma_vector.begin());
        return gamma_vector;
    }
    std::vector<int> PSP::get_partitions() {
        std::vector<int> partitions;
        for (std::list<stl::Partition>::iterator it = psp_list.begin(); it != psp_list.end(); it++) {
            partitions.push_back(it->Cardinality());
        }
        return partitions;
    }

    std::list<stl::Partition>& PSP::get_psp() {
        return psp_list;
    }
    //! get the smallest partition whose size >= k
    stl::Partition& PSP::get_smallest_partition(int k) {
        for (stl::Partition& i : psp_list) {
            if (i.Cardinality() >= k)
                return i;
        }
    }
    //! get the smallest partition whose size >= k, label each data point with an integer
    std::vector<int> PSP::get_category(int k) {
        for (stl::Partition& i : psp_list) {
            if (i.Cardinality() >= k) {
                return to_category(i);
            }
        }
        return std::vector<int>();
    }
    std::vector<int> PSP::to_category(stl::Partition& partition) {
        std::vector<int> cat(num_points, 0);
        int t = 0;
        for (const stl::CSet& j : partition) {
            for (int i : j.GetMembers()) {
                cat[i] = t;
            }
            t++;
        }
        return cat;
    }
}