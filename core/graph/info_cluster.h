#pragma once
#include <lemon/list_graph.h>
#include "core/graph/graph.h"
#include "core/dt.h"

namespace submodular {
    class InfoCluster {
    public:
		typedef lemon::ListDigraph Digraph;
		typedef Digraph::ArcMap<double> ArcMap;
        InfoCluster(){}
        InfoCluster(const std::vector<std::tuple<std::size_t, std::size_t, double>>& elt, int np) {
            num_points = np;
			g = new Digraph();
			edge_map = new ArcMap(*g);
			make_dgraph(np, elt, *g, *edge_map);
        }
		virtual  ~InfoCluster() {
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
            PSP psp_class(g, edge_map);
			stl::Partition p = psp_class.run(pn);
            return p;
        }
        void run() {
            PSP psp_class(g, edge_map);
            psp_class.run(false);
            gamma_list = psp_class.get_critical_values();
            psp_list = psp_class.get_psp();
        }
        void run_bruteForce() {
            PSP psp_class(g, edge_map);
            psp_class.run(true);
            gamma_list = psp_class.get_critical_values();
            psp_list = psp_class.get_psp();
        }
        std::list<double>& get_gamma_list() {
            return gamma_list;
        }
        std::list<double> get_critical_values() {
            return gamma_list;
        }
        std::vector<int> get_partitions() {
            std::vector<int> partitions;
            for (std::list<stl::Partition>::iterator it = psp_list.begin(); it != psp_list.end(); it++) {
				partitions.push_back(it->Cardinality());
            }
            return partitions;
        }

        std::list<stl::Partition>& get_psp_list() {
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
		Digraph* g;
		ArcMap* edge_map;
        int num_points;
    private:
        std::list<double> gamma_list;
        
        std::list<stl::Partition> psp_list;
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
