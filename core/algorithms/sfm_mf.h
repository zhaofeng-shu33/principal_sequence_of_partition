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
        //value_type minimum_value = sf->Call(X.Extend(1)) - lambda_;
        //for (int i : X.GetMembers())
        //    minimum_value -= xl[i];
        this->SetResults(minimum_value, X);
    }
    std::string GetName() { return "maximal flow"; }
};

}
