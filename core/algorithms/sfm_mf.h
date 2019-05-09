// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//     http://www.apache.org/licenses/LICENSE-2.0
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// =============================================================================
#pragma once
#include <core/graph.h>
#include <core/graph/maxflow.h>
#include <lemon/list_graph.h>
#ifdef _DEBUG
#include <lemon/lgf_writer.h>
#include <fstream>
#endif
#include "preflow/mf_base.h"
#include <lemon/adaptors.h>
#include <boost/core/swap.hpp>
namespace submodular{

template <typename ValueType>
class MF: public SFMAlgorithm<ValueType> {
public:
    using value_type = typename ValueTraits<ValueType>::value_type;
    void Minimize(SubmodularOracle<ValueType>& F){}
#ifdef USE_LEMON
    void Minimize(SubmodularOracle<ValueType>* sf, std::vector<value_type>& xl, value_type lambda_, lemon::ListGraph* _g, lemon::ListGraph::EdgeMap<ValueType>* _edge_map) {
        this->reporter_.SetNames(GetName(), sf->GetName());
        //construct lemon graph
        lemon::ListDigraph g;
        std::vector<lemon::ListDigraph::Node> node_list;
        int graph_size = xl.size();
        //sink node id is graph_size, source node id is graph_size+1
        g.reserveNode(graph_size + 2);
        g.reserveArc(graph_size*(graph_size + 1) / 2 + 2 * graph_size);
        for (int i = 0; i < graph_size + 2; i++) {
            node_list.push_back(g.addNode());
        }
        // construct edge cost map
        typedef typename lemon::ListDigraph::template ArcMap<value_type> ArcMap;
        ArcMap edge_cost_map(g);
        lemon::ListDigraph::Node source_node = node_list[graph_size + 1];
        lemon::ListDigraph::Node sink_node = node_list[graph_size];
        value_type const_difference = lambda_;
        for (int i = 0; i < graph_size; i++) {
			
            lemon::ListDigraph::Arc arc = g.addArc(source_node, node_list[i]);
            edge_cost_map[arc] = std::max<value_type>(0, -xl[i]);

            arc = g.addArc(node_list[i], sink_node);			
			edge_cost_map[arc] = std::max<value_type>(0, xl[i]); // sink_node_id = graph_size

            const_difference += std::max<value_type>(0, xl[i]);
        }
		lemon::ListGraph::NodeMap<bool> node_filter(*_g);
		for (int i = 0; i <= graph_size; i++) {
			node_filter[_g->nodeFromId(i)] = true;
		}
		for (int i = graph_size + 1; i <= _g->maxNodeId(); i++) {
			node_filter[_g->nodeFromId(i)] = false;
		}
		lemon::FilterNodes<lemon::ListGraph> subgraph(*_g, node_filter);
		for (lemon::FilterNodes<lemon::ListGraph>::IncEdgeIt e(subgraph, subgraph.nodeFromId(graph_size)); e != lemon::INVALID; ++e) {
			int t_id = subgraph.id(subgraph.u(e));
			lemon::ListDigraph::OutArcIt arc(g, g.nodeFromId(t_id));
			edge_cost_map[arc] += (*_edge_map)[e];
		}
		for (lemon::FilterNodes<lemon::ListGraph>::EdgeIt e(subgraph); e != lemon::INVALID; ++e) {
			int s_id = subgraph.id(subgraph.u(e));
			int t_id = subgraph.id(subgraph.v(e));
			if (s_id > t_id)
				boost::swap(s_id, t_id);
			if (t_id == graph_size)
				continue;
			lemon::ListDigraph::Arc arc = g.addArc(node_list[s_id], node_list[t_id]);
			edge_cost_map[arc] = (*_edge_map)[e];
		}
        lemon::Preflow_Relabel<lemon::ListDigraph, ArcMap> pf(g, edge_cost_map, source_node, sink_node);
        pf.run();
        value_type minimum_value = pf.flowValue() - const_difference;

        Set X = Set::MakeEmpty(graph_size);
        for (int v = 0; v < graph_size; ++v) {
            if (!pf.minCut(node_list[v]))
                X.AddElement(v);
        }
#ifdef _DEBUG
        value_type minimum_value_2 = sf->Call(X.Extend(1)) - lambda_;
        for (int i : X.GetMembers())
            minimum_value_2 -= xl[i];
        if (std::abs(minimum_value - minimum_value_2) > 1e-5) {
            std::cout << "maxflow value differs: " << minimum_value << " != " << minimum_value_2 << std::endl;
            // dump the graph in graph_dump.txt(current directory) with lemon graph format
            std::ofstream fout("graph_dump.txt");
            lemon::digraphWriter(g, fout).
                arcMap("capacity", edge_cost_map)
                .node("source", source_node)
                .node("target", sink_node).
                run();
            fout.close();
            exit(0);
        }
#endif
        this->SetResults(minimum_value, X);
    }
#else
    void Minimize(SubmodularOracle<ValueType>* sf, std::vector<value_type>& xl, value_type lambda_){
        this->reporter_.SetNames(GetName(), sf->GetName());
        //construct s-t graph
        int graph_size = xl.size();
        //sink node id is graph_size, source node id is graph_size+1
        MaxflowGraph<ValueType> g;
        int s = graph_size + 1, t = graph_size;
        for (std::size_t i = 0; i <= graph_size + 1; ++i) {
            g.AddNode(i);
        }
        typename MaxflowGraph<ValueType>::Node_s ss = g.GetNodeById(s), tt = g.GetNodeById(t);
        value_type const_difference = lambda_;
        for (int v = 0; v < graph_size; ++v) {
            typename MaxflowGraph<ValueType>::Node_s vv = g.GetNodeById(v);
            if(xl[v]<0)
                g.AddSVArcPair(vv, ss, -xl[v], 0);
            else {
                g.AddVTArcPair(tt, vv, xl[v], 0);
                const_difference += xl[v];
            }
            g.AddVTArcPair(tt, vv, sf->GetArcCap(t, v), 0);
            for(int w = v+1; w < graph_size; w++)
                g.AddArcPair(g.GetNodeById(w), vv, sf->GetArcCap(w, v), 0);
        }        
        g.MakeGraph(ss,tt);
        g.FindMinCut();
        
        Set X = Set::MakeEmpty(graph_size);
        for (int v = 0; v < graph_size; ++v) {
            if(g.WhatSegment(v)==TermType::SINK)
                X.AddElement(v);
        }
        value_type minimum_value = g.GetMaxFlowValue() - const_difference;
#ifdef _DEBUG
        value_type minimum_value_2 = sf->Call(X.Extend(1)) - lambda_;
        for (int i : X.GetMembers())
            minimum_value_2 -= xl[i];
        if (std::abs(minimum_value - minimum_value_2) > 1e-5) {
            std::cout << "maxflow value differs: " << minimum_value << " != " << minimum_value_2 << std::endl;
            exit(0);
        }
#endif
        this->SetResults(minimum_value, X);
    }
#endif
    std::string GetName() { return "maximal flow"; }
	private:
		lemon::Tolerance<ValueType> _tolerance;
};

}
