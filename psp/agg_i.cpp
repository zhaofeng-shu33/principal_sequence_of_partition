#include <cmath>
#include <queue>
#include <lemon/adaptors.h>
#include <lemon/preflow.h>
#include "psp/agg_i.h"
#include <cassert>
#include <Eigen/Core>

namespace psp {
    SF::SF(Digraph* g, ArcMap* edge_map): _g(g), _edge_map(edge_map){}

    double SF::operator() (const stl::CSet &B) {

    }
    size_t SF::size() {

    }
    void SF::set_j(size_t new_j) {
        j = new_j;
    }

    AGG_I::AGG_I(Digraph* g, ArcMap* edge_map): _g(g), _edge_map(edge_map), tree_edge_map(tree){
        node_size = lemon::countNodes(*g);
    }

    Eigen::VectorXd min_norm_base(const SF &f, double fn_tol, double eps) {

    }

	Eigen::VectorXd min_norm_base(const SF &f) {
		return min_norm_base(f, 1E-10, 1E-15);
	}  

    void AGG_I::agglomerate() {
        size_t k = current_partition.Cardinality();
        SF f(_g, _edge_map);
        double gamma = -INFINITY;
        std::vector<Eigen::VectorXd> x(k - 1);
			for (size_t j = 1; j < k; j++) {
				f.set_j(j);
				//std::cout << "j : " << j << endl;
				x[j-1] = min_norm_base(f);
				//std::cout << x[j - 1];
				gamma = std::max(gamma, -(x[j-1].minCoeff()));
			}
        current_partition.clear();
    }
    void AGG_I::run() {
        Node root = tree.addNode();
        K.push_back(stl::CSet::MakeDense(node_size));
        for (int i = 0; i < node_size; i++) {
            Node leaf = tree.addNode();
            tree.addArc(root, leaf);
            stl::CSet s;
            s.AddElement(i);
            current_partition.AddElement(s);
            K.push_back(s);
        }
        while(current_partition.Cardinality() > 1) {
            agglomerate();
        }
        typedef std::pair<int, double> psp_pair;
        // take the smallest number from the queue
        std::vector<psp_pair> q; // decreasing
        Digraph::OutArcIt a(tree, root);
        q.push_back(std::make_pair(0, tree_edge_map[a]));
        while (!q.empty()) {
            stl::Partition p;
            for (const psp_pair& pair : q) {
                p.AddElement(K[pair.first]);
            }
            psp_list.push_back(p);

            psp_pair tmp = q.back();
            q.pop_back();
            if(tmp.second < INFINITY)
                critical_values.push_back(tmp.second);
            else {
                q.clear();
                break;
            }
            for (Digraph::OutArcIt a(tree, tree.nodeFromId(tmp.first)); a != lemon::INVALID; ++a) {
                Node child = tree.target(a);
                Digraph::OutArcIt a_child(tree, child);
                double inserted_lambda;
                if (a_child == lemon::INVALID){
                    inserted_lambda = INFINITY;
                }
                else
                    inserted_lambda = tree_edge_map[a_child];
                bool inserted = false;
                for (std::vector<psp_pair>::iterator it = q.begin(); it != q.end(); it++) {
                    if (it->second < inserted_lambda) {
                        q.insert(it, std::make_pair(tree.id(tree.target(a)), inserted_lambda));
                        inserted = true;
                        break;
                    }
                }
                if (inserted == false) {
                    q.push_back(std::make_pair(tree.id(tree.target(a)), inserted_lambda));
                }
            }
        }
    }


    void AGG_I::contract(const stl::CSet& S, int i) {
        // we don't use _g->contract function since it can not handle multiple arcs within two nodes
        std::map<int, double> capacity_map;
        for (Digraph::NodeIt n(*_g); n != lemon::INVALID; ++n)
            capacity_map[_g->id(n)] = 0;
        // iterate over all arcs
        for (Digraph::ArcIt a(*_g); a != lemon::INVALID; ++a) {
            int s_id = _g->id(_g->source(a));
            int t_id = _g->id(_g->target(a));
            if (S.HasElement(s_id) && !S.HasElement(t_id)) {
                capacity_map.at(t_id) += (*_edge_map)[a]; //out is positive
            }
            else if (!S.HasElement(s_id) && S.HasElement(t_id)) {
                capacity_map.at(s_id) += (*_edge_map)[a];
            }
            if (s_id == i || t_id == i) { // delete all arcs connected with Node(i)
                _g->erase(a);
            }
        }
        //delete S\{i} in _g
        for (int j : S) {
            Node J = _g->nodeFromId(j);
            if (_g->valid(J) && j != i)
                _g->erase(J);
        }
        Node _S = _g->nodeFromId(i);

        for (const std::pair<int, double>& val : capacity_map) {
            if (S.HasElement(val.first) || std::abs(val.second) < _tolerance.epsilon())
                continue;
            Arc a;
            if (val.first > i) {
                a = _g->addArc(_S, _g->nodeFromId(val.first));
            }
            else {
                a = _g->addArc(_g->nodeFromId(val.first), _S);
            }
            (*_edge_map)[a] = std::abs(val.second);
        }
    }
    std::list<double>& AGG_I::get_critical_values() {
        return critical_values;
    }

    std::list<stl::Partition>& AGG_I::get_psp() {
        return psp_list;
    }
}
