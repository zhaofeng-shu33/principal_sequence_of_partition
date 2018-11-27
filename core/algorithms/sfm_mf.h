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
namespace submodular{

template <typename ValueType>
class MF: public SFMAlgorithm<ValueType> {
public:
    using value_type = typename ValueTraits<ValueType>::value_type;
    void Minimize(SubmodularOracle<ValueType>& F){}
    void Minimize(SubmodularOracle<ValueType>* sf, std::vector<value_type> xl, value_type lambda_){
        //construct s-t graph
        int graph_size = xl.size();
        //sink node id is graph_size-1, source node id is graph_size
        MaxflowGraph<ValueType> g;
        int s = graph_size, t = graph_size-1;
        for (std::size_t i = 0; i <= graph_size; ++i) {
            g.AddNode(i);
        }
        for (int v = 0; v < graph_size-1; ++v) {
            
            graph.AddSVArcPair(s, v, std::max(0, -xl[v]),0);
            graph.AddVTArcPair(v, t, std::max(0, xl[v]), 0);
            for(int w = v+1; w < graph_size-1; w++)
                graph.AddArcPair(w, v, sf->GetArcCap(w, v), 0);
        }        
        g.MakeGraph(s,t);
        g.FindMinCut();
        
        Set X = Set::MakeEmpty(graph_size-1);
        for (int v = 0; v < graph_size-1; ++v) {
            if(g.WhatSegment(v)==TermType::SINK)
                X.AddElement(v);
        }
        minimum_value = g.GetMaxFlowValue();
        this->SetResults(minimum_value, X);
    }
    std::string GetName() { return "maximal flow"; }
}    
}