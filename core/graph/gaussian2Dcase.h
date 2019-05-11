#pragma once
#include <random>
#include <chrono>
#include <iostream>
#include <fstream>
#include "core/graph/graph.h"
#include "core/dt.h"
#include "core/graph/info_cluster.h"
#include "core/io_utility.h"
#include "core/pmf.h"
namespace demo {
    typedef std::vector<std::tuple<std::size_t, std::size_t, double>> EdgeListTuple;
    class Gaussian2DGraphBase {
    public:
        Gaussian2DGraphBase(int np, double gamma = 1, double a[][2] = NULL) :
            distribution(0, 1),
            _gamma(gamma)
        {
            if (a == NULL) {
                // generator num_points 2D points ~ N(0,1) located at different positions
                unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
                generator = std::default_random_engine(seed);

                for (int j = 0; j < 4; j++) {
                    for (int i = 0; i < np / 4; i++) {
                        x_pos.push_back(distribution(generator) + data_1[j][0]);
                        y_pos.push_back(distribution(generator) + data_1[j][1]);
                    }
                }
            }
            else {
                for (int j = 0; j < np; j++) {
                    x_pos.push_back(a[j][0]);
                    y_pos.push_back(a[j][1]);
                }
            }
            for (int i = 0; i < np; i++)
                for (int j = i + 1; j < np; j++) {
                    edge_list_tuple.push_back(std::make_tuple(i, j, compute_similarity(x_pos[i], y_pos[i], x_pos[j], y_pos[j])));
                }
        }
        EdgeListTuple get_edge_list_tuple() { return edge_list_tuple; }
    protected:
        EdgeListTuple edge_list_tuple;
        std::vector<double> x_pos;
        std::vector<double> y_pos;
    private:
        // {src, dst, capacity}
        std::default_random_engine generator;
        std::normal_distribution<double> distribution;
        double _gamma;
        int data_1[4][2] = { {3,3},{3,-3},{-3,-3},{-3,3} };
        //! use Gaussian similarity function $exp(-||p_1 - p_2||^2/2) $
        double compute_similarity(double x_1, double y_1, double x_2, double y_2) {
            return exp(-1.0 * _gamma* pow(x_1 - x_2, 2) - _gamma * pow(y_1 - y_2, 2));
        }
    };

    class Gaussian2DGraph : public Gaussian2DGraphBase, public submodular::InfoCluster {
    public:
        Gaussian2DGraph(int np, double gamma = 1, double a[][2] = NULL): 
			Gaussian2DGraphBase(np, gamma, a),
			InfoCluster(edge_list_tuple, np)
        {
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

