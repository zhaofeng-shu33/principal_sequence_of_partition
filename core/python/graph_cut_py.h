#pragma once
#include "core/oracles/modular.h"
#include "core/graph.h"
#include "core/oracles/graph_cut.h"
#include <boost/python.hpp>

namespace submodular {
    class PyGraph {
    public:
        using EdgeListFloat = std::vector<std::tuple<std::size_t, std::size_t, double>>;

        PyGraph(int np, boost::python::list py_list, double gamma = 1) :
            _gamma(gamma)
        {
            EdgeListFloat edge_list;
            num_points = np;
            for (int i = 0; i < boost::python::len(py_list); i++) {
                boost::python::tuple pt = boost::python::extract<boost::python::tuple>(py_list[i]);
                int source_node_id = boost::python::extract<int>(pt[0]);
                int target_node_id = boost::python::extract<int>(pt[1]);
                double capacity = boost::python::extract<double>(pt[2]);
                edge_list.push_back(std::make_tuple(source_node_id, target_node_id, capacity));
            }
            sg = submodular::make_dgraph(num_points, edge_list);
        }
        PyGraph(const PyGraph& pg) {
            sg = pg.sg;
            num_points = pg.num_points;
        }

        void run(bool bruteForce = false) {
            submodular::DirectedGraphCutOracle<double>* dgc = new submodular::DirectedGraphCutOracle<double>(sg);
            submodular::PSP<double> psp_class(dgc);
            psp_class.run(bruteForce);
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
            std::vector<int> partations;
            for (int i = 0; i < psp_list.size(); i++) {
                if (psp_list[i].size() > 0) {
                    partations.push_back(psp_list[i].size());
                }
            }
            return partations;
        }

        std::vector<std::vector<submodular::Set>>& get_psp_list() {
            return psp_list;
        }
        //! get the smallest partition whose size >= k
        std::vector<submodular::Set>& get_smallest_partition(int k) {
            for (std::vector<submodular::Set>& i : psp_list) {
                if (i.size() >= k)
                    return i;
            }
        }
        //! get the smallest partition whose size >= k, label each data point with an integer
        std::vector<int> get_category(int k) {
            for (std::vector<submodular::Set>& i : psp_list) {
                if (i.size() >= k) {
                    return to_category(i);
                }
            }
            return std::vector<int>();
        }


    private:
        // {src, dst, capacity}
        submodular::SimpleGraph<double> sg;
        std::vector<double> gamma_list;
        int num_points;
        std::vector<std::vector<submodular::Set>> psp_list;
        double _gamma;
        //! form conversion
        std::vector<int> to_category(std::vector<submodular::Set>& partation) {
            std::vector<int> cat(num_points, 0);
            int t = 0;
            for (submodular::Set& j : partation) {
                for (int i : j.GetMembers()) {
                    cat[i] = t;
                }
                t++;
            }
            return cat;
        }
    };
}