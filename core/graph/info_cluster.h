#pragma once
#include "core/oracles/modular.h"
#include "core/graph.h"
#include "core/oracles/graph_cut.h"

namespace submodular {
    class InfoCluster {
    public:
        InfoCluster(){}
        InfoCluster(const InfoCluster& pg) {
            sg = pg.sg;
            num_points = pg.num_points;
        }
        InfoCluster(const std::vector<std::tuple<std::size_t, std::size_t, double>>& elt, int np) {
            num_points = np;
            sg = submodular::make_dgraph(num_points, elt);
        }
        //! get the partition which has at least pn clusters
        std::vector<int> get_labels(int pn) {
            std::vector<Set> p = get_partition(pn);
            return to_category(p);
        }
        std::vector<Set> get_partition(int pn) {
            DirectedGraphCutOracle<double>* dgc = new DirectedGraphCutOracle<double>(sg);
            PSP<double> psp_class(dgc);
            std::vector<Set> p = psp_class.run(pn);
            delete dgc;
            return p;
        }
        void run(bool bruteForce = false) {
            DirectedGraphCutOracle<double>* dgc = new DirectedGraphCutOracle<double>(sg);
            PSP<double> psp_class(dgc);
            psp_class.run(bruteForce);
            gamma_list = psp_class.Get_critical_values();
            psp_list = psp_class.Get_psp();
            delete dgc;
        }
        void agglomerative_run() {
            DirectedGraphCutOracle<double>* dgc = new DirectedGraphCutOracle<double>(sg);
            PSP<double> psp_class(dgc);
            psp_class.agglomerative_run();
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
                if (psp_list[i].size() > 0) {
                    critical_value_list.push_back(gamma_list[i]);
                }
            }
            critical_value_list.pop_back(); // the last value is invalid
            return critical_value_list;
        }
        std::vector<int> get_partitions() {
            std::vector<int> partitions;
            for (int i = 0; i < psp_list.size(); i++) {
                if (psp_list[i].size() > 0) {
                    partitions.push_back(psp_list[i].size());
                }
            }
            return partitions;
        }

        std::vector<std::vector<Set>>& get_psp_list() {
            return psp_list;
        }
        //! get the smallest partition whose size >= k
        std::vector<Set>& get_smallest_partition(int k) {
            for (std::vector<Set>& i : psp_list) {
                if (i.size() >= k)
                    return i;
            }
        }
        //! get the smallest partition whose size >= k, label each data point with an integer
        std::vector<int> get_category(int k) {
            for (std::vector<Set>& i : psp_list) {
                if (i.size() >= k) {
                    return to_category(i);
                }
            }
            return std::vector<int>();
        }
    protected:
        SimpleGraph<double> sg;
        int num_points;
    private:
        std::vector<double> gamma_list;
        
        std::vector<std::vector<Set>> psp_list;
        //! form conversion
        std::vector<int> to_category(std::vector<Set>& partition) {
            std::vector<int> cat(num_points, 0);
            int t = 0;
            for (Set& j : partition) {
                for (int i : j.GetMembers()) {
                    cat[i] = t;
                }
                t++;
            }
            return cat;
        }
    };
}