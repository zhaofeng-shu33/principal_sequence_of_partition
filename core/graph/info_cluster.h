#pragma once
#include "core/oracles/modular.h"
#include "core/graph.h"
#include "core/oracles/graph_cut.h"
#include <lemon/list_graph.h>
namespace submodular {
    class InfoCluster {
    public:
        InfoCluster(){}
        InfoCluster(const std::vector<std::tuple<std::size_t, std::size_t, double>>& elt, int np) {
            num_points = np;
            sg = make_dgraph(num_points, elt);
			g = new lemon::ListGraph();
			edge_map = new lemon::ListGraph::EdgeMap<double>(*g);
			make_graph(np, elt, *g, *edge_map);
        }
		~InfoCluster() {
			delete edge_map;
			delete g;
		}
        //! get the partition which has at least pn clusters
        //! rerun the total algorithm, use with caution.
        std::vector<int> get_labels(int pn) {
            stl::Partition p = get_partition(pn);
            return to_category(p);
        }
		stl::Partition get_partition(int pn) {
            DirectedGraphCutOracle<double>* dgc = new DirectedGraphCutOracle<double>(sg);
            PSP<double> psp_class(dgc, g, edge_map);
			stl::Partition p = psp_class.run(pn);
            delete dgc;
            return p;
        }
        void run() {
            DirectedGraphCutOracle<double>* dgc = new DirectedGraphCutOracle<double>(sg);
            PSP<double> psp_class(dgc, g, edge_map);
            psp_class.run(false);
            gamma_list = psp_class.Get_critical_values();
            psp_list = psp_class.Get_psp();
            delete dgc;
        }
        void run_bruteForce() {
            DirectedGraphCutOracle<double>* dgc = new DirectedGraphCutOracle<double>(sg);
            PSP<double> psp_class(dgc, g, edge_map);
            psp_class.run(true);
            gamma_list = psp_class.Get_critical_values();
            psp_list = psp_class.Get_psp();
            delete dgc;
        }
        std::vector<double>& get_gamma_list() {
            return gamma_list;
        }
        std::vector<double> get_critical_values() {
            std::vector<double> critical_value_list;
            for (int i = 0; i < psp_list.size(); i++) {
                if (psp_list[i].Cardinality() > 0) {
                    critical_value_list.push_back(gamma_list[i]);
                }
            }
            critical_value_list.pop_back(); // the last value is invalid
            return critical_value_list;
        }
        std::vector<int> get_partitions() {
            std::vector<int> partitions;
            for (int i = 0; i < psp_list.size(); i++) {
                if (psp_list[i].Cardinality() > 0) {
                    partitions.push_back(psp_list[i].Cardinality());
                }
            }
            return partitions;
        }

        std::vector<stl::Partition>& get_psp_list() {
            return psp_list;
        }
        //! get the smallest partition whose size >= k
		stl::Partition& get_smallest_partition(int k) {
            for (stl::Partition& i : psp_list) {
                if (i.Cardinality() >= k)
                    return i;
            }
        }
        //! get the smallest partition whose size >= k, label each data point with an integer
        std::vector<int> get_category(int k) {
            for (stl::Partition& i : psp_list) {
                if (i.Cardinality() >= k) {
                    return to_category(i);
                }
            }
            return std::vector<int>();
        }
    protected:
        SimpleGraph<double> sg;
		lemon::ListGraph* g;
		lemon::ListGraph::EdgeMap<double>* edge_map;
        int num_points;
    private:
        std::vector<double> gamma_list;
        
        std::vector<stl::Partition> psp_list;
        //! form conversion
        std::vector<int> to_category(stl::Partition& partition) {
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
    };
}
