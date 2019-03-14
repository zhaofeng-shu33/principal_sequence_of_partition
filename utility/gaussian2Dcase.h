#pragma once
#include <random>
#include <chrono>
#include <iostream>
#include <fstream>
#include "core/graph.h"
#include "core/oracles/graph_cut.h"
#include "core/dt.h"
#include "core/graph/info_cluster.h"
#include "core/io_utility.h"
#include "test/test_utility.h"
#include "core/pmf.h"
namespace demo {
    class Graph4PointTest : public testing::Test {
    protected:
        EdgeListTuple edge_list_tuple_1;
        submodular::DirectedGraphCutOracle<double>* dgc;
        virtual void SetUp() {
            construct_edge_list_tuple_4(edge_list_tuple_1);

            submodular::SimpleGraph<double> sg = submodular::make_dgraph(4, edge_list_tuple_1);

            dgc = new submodular::DirectedGraphCutOracle<double>(sg);

        }
    };
    class Gaussian2DGraph : public submodular::InfoCluster, public Gaussian2DGraphBase {
    public:
        Gaussian2DGraph(int np, double gamma = 1, double a[][2] = NULL) : Gaussian2DGraphBase(np, gamma, a)
        {
            this->num_points = np;
            sg = submodular::make_dgraph(num_points, edge_list_tuple);
        }
        std::vector<double>& get_x_pos_list() {
            return x_pos;
        }
        std::vector<double>& get_y_pos_list() {
            return y_pos;
        }
    };
    class Gaussian2DPDT : public parametric::PDT {
    public:
        Gaussian2DPDT(int np, double gamma = 1): num_points(np), arcMap(g), parametric::PDT(g, arcMap){
            demo::Gaussian2DGraphBase* gb = new demo::Gaussian2DGraphBase(np, gamma);
            demo::EdgeListTuple elt = gb->get_edge_list_tuple();
            //build the graph and arcMap
            g.reserveNode(np);
            for (int i = 0; i < np; i++)
                g.addNode();
            for (std::tuple<std::size_t, std::size_t, double>& edge_tuple : elt) {
                lemon::ListDigraph::Node s = g.nodeFromId(std::get<0>(edge_tuple));
                lemon::ListDigraph::Node t = g.nodeFromId(std::get<1>(edge_tuple));
                lemon::ListDigraph::Arc a1 = g.addArc(s, t);
                lemon::ListDigraph::Arc a2 = g.addArc(t, s);
                arcMap[a1] = std::get<2>(edge_tuple);
                arcMap[a2] = std::get<2>(edge_tuple);
            }
            delete gb;
        }
        parametric::Partition get_smallest_partition(int part) {
            if (part < 1 || part > num_points)
                throw std::range_error("invalid part number is given");
            std::list<parametric::Partition> Lp = get_partition_list();
            for (std::list<parametric::Partition>::iterator it = Lp.begin(); it != Lp.end(); it++) {
                if (it->Cardinality() >= part)
                    return *it;
            }
        }
    private:
        lemon::ListDigraph g;
        lemon::ListDigraph::ArcMap<double> arcMap;
        int num_points;
    };
}

