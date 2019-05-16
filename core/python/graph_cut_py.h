#pragma once
#include <boost/python.hpp>

#include "core/graph/graph.h"
#include "core/graph/info_cluster.h"
#include <lemon/tolerance.h>
namespace submodular {
    class PyGraph : public InfoCluster {
    public:
        using EdgeListTuple = std::vector<std::tuple<std::size_t, std::size_t, double>>;

        PyGraph(int np, boost::python::list py_list)
        {
            num_points = np;
            EdgeListTuple edge_list;
            for (int i = 0; i < boost::python::len(py_list); i++) {
                boost::python::tuple pt = boost::python::extract<boost::python::tuple>(py_list[i]);
                int source_node_id = boost::python::extract<int>(pt[0]);
                int target_node_id = boost::python::extract<int>(pt[1]);
                double capacity = boost::python::extract<double>(pt[2]);
                edge_list.push_back(std::make_tuple(source_node_id, target_node_id, capacity));
            }
			g = new Digraph();
			edge_map = new ArcMap(*g);
			make_dgraph(np, edge_list, *g, *edge_map);
        }
    };
}
namespace parametric {
    class PyGraphPDT : public PDT {
    public:
        PyGraphPDT(int np, boost::python::list py_list) : num_points(np), arcMap(g), PDT(g, arcMap)
        {
            g.reserveNode(np);
            for (int i = 0; i < np; i++)
                g.addNode();
            for (int i = 0; i < boost::python::len(py_list); i++) {
                boost::python::tuple pt = boost::python::extract<boost::python::tuple>(py_list[i]);
                int source_node_id = boost::python::extract<int>(pt[0]);
                int target_node_id = boost::python::extract<int>(pt[1]);
                double capacity = boost::python::extract<double>(pt[2]);
				if (capacity < _tolerance.epsilon())
					continue;
                lemon::ListDigraph::Node s = g.nodeFromId(source_node_id);
                lemon::ListDigraph::Node t = g.nodeFromId(target_node_id);
                lemon::ListDigraph::Arc a1 = g.addArc(s, t);
                lemon::ListDigraph::Arc a2 = g.addArc(t, s);
                arcMap[a1] = capacity;
                arcMap[a2] = capacity;
            }
        }
        PyGraphPDT(const PyGraphPDT& ppdt): arcMap(g), PDT(g, arcMap) {
            num_points = ppdt.num_points;

            for (int i = 0; i < num_points; i++)
                g.addNode();
            for (lemon::ListDigraph::ArcIt a(ppdt.g); a != lemon::INVALID; ++a) {
                lemon::ListDigraph::Node s = ppdt.g.nodeFromId(ppdt.g.id(ppdt.g.source(a)));
                lemon::ListDigraph::Node t = ppdt.g.nodeFromId(ppdt.g.id(ppdt.g.target(a)));
                lemon::ListDigraph::Arc a1 = g.addArc(s, t);
                arcMap[a1] = ppdt.arcMap[a];
            }
        }
        std::vector<double> get_critical_values() {
            std::vector<double> gLv(Lambda_list.size());
            std::copy(Lambda_list.begin(), Lambda_list.end(), gLv.begin());
            gLv.pop_back(); // the last value is infinity
            return gLv;
        }
        std::vector<int> get_partitions() {
            std::vector<int> partitions;
            for (Partition& p: partition_list) {
                    partitions.push_back(p.Cardinality());
            }
            return partitions;
        }
        //! get the smallest partition whose size >= k, label each data point with an integer
        std::vector<int> get_category(int k) {
            for (Partition& i : partition_list) {
                if (i.Cardinality() >= k) {
                    return to_category(i);
                }
            }
            return std::vector<int>();
        }
    private:
        lemon::ListDigraph g;
        lemon::ListDigraph::ArcMap<double> arcMap;
		lemon::Tolerance<double> _tolerance;
        int num_points;
        //! form conversion
        std::vector<int> to_category(Partition& partition) {
            std::vector<int> cat(num_points, 0);
            int t = 0;
            for ( const stl::CSet& j : partition) {
                for (int i : j.GetMembers()) {
                    cat[i] = t;
                }
                t++;
            }
            return cat;
        }
    };
}