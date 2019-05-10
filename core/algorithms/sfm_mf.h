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
#include <lemon/list_graph.h>
#ifdef _DEBUG
#include <lemon/lgf_writer.h>
#include <fstream>
#endif
#include "preflow/mf_base.h"
#include <lemon/adaptors.h>
#include <boost/core/swap.hpp>
namespace submodular{
	stl::CSet convert_set(const Set& _s) {
		stl::CSet s;
		for (int i : _s.GetMembers())
			s.AddElement(i);
		return s;
	}
template <typename ValueType>
class MF: public SFMAlgorithm<ValueType> {
public:
    using value_type = typename ValueTraits<ValueType>::value_type;
	typedef lemon::ListDigraph Digraph;
	typedef typename Digraph::ArcMap<ValueType> ArcMap;
	typedef typename lemon::FilterNodes<Digraph> SubDigraph;
	typedef typename lemon::Preflow_RelabelDefaultTraits<SubDigraph, ArcMap> PreflowSubgraphTraits;
    void Minimize(SubmodularOracle<ValueType>& F){}
#ifdef USE_LEMON
    void Minimize(std::vector<value_type>& xl, value_type lambda_, Digraph* _g, ArcMap* _edge_map) {
        this->reporter_.SetNames(GetName(), "graph maximal flow");
		int graph_size = xl.size();

		lemon::ListDigraph::NodeMap<bool> node_filter(*_g);
		for (int i = 0; i <= graph_size; i++) {
			node_filter[_g->nodeFromId(i)] = true;
		}		
		for (int i = graph_size + 1; i < lemon::countNodes(*_g); i++) {
			node_filter[_g->nodeFromId(i)] = false;
		}
		lemon::FilterNodes<Digraph> subgraph(*_g, node_filter);
		std::vector<value_type> sink_node_cost_map;
		sink_node_cost_map.resize(graph_size);
		for (lemon::FilterNodes<Digraph>::InArcIt a(subgraph, subgraph.nodeFromId(graph_size)); a != lemon::INVALID; ++a) {
			int node_id = subgraph.id(subgraph.source(a));
			sink_node_cost_map[node_id] = (*_edge_map)[a];
		}
		subgraph.disable(_g->nodeFromId(graph_size));

        // construct source node and sink node (inplace)
		Digraph::Node source_node = subgraph.addNode();
		Digraph::Node sink_node = subgraph.addNode();

        // construct edge cost map related with source_node and sink_node
        value_type const_difference = lambda_;
        for (int i = 0; i < graph_size; i++) {
			Digraph::Arc arc;
			if (xl[i] < 0) {
				arc = subgraph.addArc(source_node, subgraph.nodeFromId(i));
				(*_edge_map)[arc] = -xl[i];
			}
			value_type sink_cost = std::max<value_type>(0, xl[i]) + sink_node_cost_map[i];
			if(sink_cost > 0){
				arc = subgraph.addArc(subgraph.nodeFromId(i), sink_node);
				(*_edge_map)[arc] = sink_cost;
			}
            const_difference += std::max<value_type>(0, xl[i]);
        }
		subgraph.enable(source_node);
		subgraph.enable(sink_node);
        lemon::Preflow_Relabel<Digraph, ArcMap, PreflowSubgraphTraits> pf(subgraph, *_edge_map, source_node, sink_node);
        pf.run();
        value_type minimum_value = pf.flowValue() - const_difference;
        Set X = Set::MakeEmpty(graph_size);
        for (int v = 0; v < graph_size; ++v) {
            if (!pf.minCut(subgraph.nodeFromId(v)))
                X.AddElement(v);
        }
		// house keeping, map is handled automatically

#ifdef  _DEBUG	
		subgraph.disable(source_node);
		subgraph.disable(sink_node);
		stl::CSet _X = convert_set(X);
		_X.AddElement(graph_size);
		subgraph.enable(_g->nodeFromId(graph_size));
		value_type minimum_value_2 = -lambda_ + get_cut_value(subgraph, *_edge_map, _X);
        for (int i : X.GetMembers())
            minimum_value_2 -= xl[i];
        if (std::abs(minimum_value - minimum_value_2) > 1e-5) {
            std::cout << "maxflow value differs: " << minimum_value << " != " << minimum_value_2 << std::endl;
            // dump the graph in graph_dump.txt(current directory) with lemon graph format
            std::ofstream fout("graph_dump.txt");
            lemon::digraphWriter(subgraph, fout).
                arcMap("capacity",*_edge_map)
                .node("source", source_node)
                .node("target", sink_node).
                run();
            fout.close();
            exit(0);
        }
#endif
		subgraph.erase(source_node);
		subgraph.erase(sink_node);
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
