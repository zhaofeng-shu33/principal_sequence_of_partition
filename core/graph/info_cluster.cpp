#include "core/graph/info_cluster.h"
#include "core/dt.h"
#include "core/pmf.h"
#include "core/psp_i.h"
#include "core/pmf_r.h"

namespace submodular {
    InfoCluster::InfoCluster(const std::vector<std::tuple<std::size_t, std::size_t, double>>& elt, int np) {
        num_points = np;
        g = new Digraph();
        edge_map = new ArcMap(*g);
        make_dgraph(np, elt, *g, *edge_map);
    }
    InfoCluster::InfoCluster(std::vector<int> s_list, std::vector<int> t_list, std::vector<double> weight_list, int np) {
        num_points = np;
        g = new Digraph();
        edge_map = new ArcMap(*g);
        std::vector<std::tuple<std::size_t, std::size_t, double>> elt;
        for (int i = 0; i < s_list.size(); i++) {
            elt.push_back(std::make_tuple(s_list[i], t_list[i], weight_list[i]));
        }
        make_dgraph(num_points, elt, *g, *edge_map);
    }
    InfoCluster::~InfoCluster() {
        delete edge_map;
        delete g;
    }
    std::vector<int> InfoCluster::get_labels(int pn) {
        stl::Partition p = get_partition(pn);
        return to_category(p);
    }
    stl::Partition InfoCluster::get_partition(int pn) {
        PSP psp_class(g, edge_map);
        stl::Partition p = psp_class.run(pn);
        return p;
    }
    void InfoCluster::run() {
        PSP psp_class(g, edge_map);
        psp_class.run();
        gamma_list = psp_class.get_critical_values();
        psp_list = psp_class.get_psp();
    }
    void InfoCluster::run_pdt() {
        parametric::PDT pdt(g, edge_map);
        pdt.run();
        gamma_list = pdt.get_critical_values();
        psp_list = pdt.get_psp();
    }
    void InfoCluster::run_pdt_r() {
        parametric::PDT_R pdt(g, edge_map);
        pdt.run();
        gamma_list = pdt.get_critical_values();
        psp_list = pdt.get_psp();
    }
    void InfoCluster::run_psp_i() {
        psp::PSP psp_class(g, edge_map);
        psp_class.run();
        gamma_list = psp_class.get_critical_values();
        psp_list = psp_class.get_psp();
    }
    std::list<double>& InfoCluster::get_gamma_list() {
        return gamma_list;
    }
    std::list<double> InfoCluster::get_critical_values() {
        return gamma_list;
    }
    std::vector<double> InfoCluster::get_critical_value_vector() {
        std::vector<double> gamma_vector(gamma_list.size());
        std::copy(gamma_list.begin(), gamma_list.end(), gamma_vector.begin());
        return gamma_vector;
    }
    std::vector<int> InfoCluster::get_partitions() {
        std::vector<int> partitions;
        for (std::list<stl::Partition>::iterator it = psp_list.begin(); it != psp_list.end(); it++) {
            partitions.push_back(it->Cardinality());
        }
        return partitions;
    }

    std::list<stl::Partition>& InfoCluster::get_psp() {
        return psp_list;
    }
    //! get the smallest partition whose size >= k
    stl::Partition& InfoCluster::get_smallest_partition(int k) {
        for (stl::Partition& i : psp_list) {
            if (i.Cardinality() >= k)
                return i;
        }
    }
    //! get the smallest partition whose size >= k, label each data point with an integer
    std::vector<int> InfoCluster::get_category(int k) {
        for (stl::Partition& i : psp_list) {
            if (i.Cardinality() >= k) {
                return to_category(i);
            }
        }
        return std::vector<int>();
    }
    std::vector<int> InfoCluster::to_category(stl::Partition& partition) {
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