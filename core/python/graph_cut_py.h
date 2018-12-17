#pragma once
#include <boost/python.hpp>

#include "core/oracles/modular.h"
#include "core/graph.h"
#include "core/oracles/graph_cut.h"
#include "core/graph/info_cluster.h"

namespace submodular {
    class PyGraph : public InfoCluster {
    public:
        using EdgeListFloat = std::vector<std::tuple<std::size_t, std::size_t, double>>;

        PyGraph(int np, boost::python::list py_list)
        {
            num_points = np;
            EdgeListFloat edge_list;
            for (int i = 0; i < boost::python::len(py_list); i++) {
                boost::python::tuple pt = boost::python::extract<boost::python::tuple>(py_list[i]);
                int source_node_id = boost::python::extract<int>(pt[0]);
                int target_node_id = boost::python::extract<int>(pt[1]);
                double capacity = boost::python::extract<double>(pt[2]);
                edge_list.push_back(std::make_tuple(source_node_id, target_node_id, capacity));
            }
            sg = submodular::make_dgraph(num_points, edge_list);
        }
    };
}