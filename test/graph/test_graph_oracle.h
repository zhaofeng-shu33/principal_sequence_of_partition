#pragma once

#include "core/graph/generalized_cut.h"
#include "test/graph/test_graph_base.h"
#include "core/graph.h"
#include "core/oracles/graph_cut.h"
#include "core/dt.h"
namespace submodular {

    class PINModelTest : public testing::Test {
    protected:
        using EdgeListFloat = std::vector<std::tuple<std::size_t, std::size_t, float>>;

        // {src, dst, capacity}
        std::size_t n_1 = 3;
        std::size_t data_1[3][3] = {
        { 0, 1, 1 },
        { 0, 2, 5 },
        { 1, 2, 1 },
        };

        EdgeListFloat edge_list_float_1;

        virtual void SetUp() {
            for (const auto& sdc : data_1) {
                edge_list_float_1.push_back(std::make_tuple(sdc[0], sdc[1], (float)sdc[2]));
            }
        }
    };
    TEST_F(MaxflowGraphTest, GraphOracleBase) {
        MaxflowGraph<int> graph_int_1 = st_graph(n_1, s_1, t_1, edge_list_int_1);
        GeneralizedCutOracle<int> gc;
        gc.SetGraph(graph_int_1);
        EXPECT_EQ(gc.GetN(), MaxflowGraphTest::n_1 - 2);
        EXPECT_EQ(gc.GetNGround() , MaxflowGraphTest::n_1);
        std::vector<element_type> members = gc.GetMembers();
        // members = {1, 2, 3, 4, 5}
        EXPECT_EQ(members.size(), MaxflowGraphTest::n_1 - 2);
    }
    TEST_F(MaxflowGraphTest, GraphOracleCall) {
        MaxflowGraph<int> graph_int_1 = st_graph(n_1, s_1, t_1, edge_list_int_1);
        GeneralizedCutOracle<int> gc;
        gc.SetGraph(graph_int_1);
        int offset = 10;
        EXPECT_EQ(gc.Call(Set()), 4 + offset);
        EXPECT_EQ(gc.Call(Set::FromIndices(2, { 1 })), 4 + offset);
        EXPECT_EQ(gc.Call(Set::FromIndices(3, { 2 })), 12 + offset);
        EXPECT_EQ(gc.Call(Set::FromIndices(3, { 1, 2 })), 12 + offset);
        EXPECT_EQ(gc.Call(Set::FromIndices(4, { 1, 2, 3 })), 6 + offset);
        EXPECT_EQ(gc.Call(Set::FromIndices(4, { 1, 3 })), 3 + offset);
        EXPECT_EQ(gc.Call(Set::FromIndices(5, { 1, 2, 3, 4 })), 4 + offset);    
    }
    TEST_F(PINModelTest, DGraphBase) {
        SimpleGraph<float> sg = make_dgraph(n_1, edge_list_float_1);
        // Test Node Access
        SimpleGraph<float>::Node_s ns[3] = { sg.GetNode(0), sg.GetNode(1), sg.GetNode(2) };
        EXPECT_EQ(ns[0]->name, 0);
        EXPECT_EQ(ns[0]->excess, 0);
        // Test Node Iterator
        SimpleGraph<float>::node_range nr = sg.NodeRange();
        for (const auto&  it : nr) {
            continue;
        }
        // Test Arc Access
        SimpleGraph<float>::Arc_s as[3] = { sg.GetArc(1, 0), sg.GetArc(2, 1), sg.GetArc(2, 0)};
        EXPECT_EQ(as[2]->capacity, 5);
        EXPECT_EQ(as[2]->tail_node.lock()->name, 0);
        EXPECT_EQ(as[2]->head_node.lock()->name, 2);
        // Test Arc Iterator
        SimpleGraph<float>::arc_range ar = sg.OutArcRange(0);
        int sum = 0;
        for (const auto& it : ar) {
            sum += it->capacity;
        }
        EXPECT_EQ(sum, 6);
    }
    TEST_F(PINModelTest, DGraphCut) {
        SimpleGraph<float> sg = make_dgraph(n_1, edge_list_float_1);
        DirectedGraphCutOracle<float> dgc(sg);
        EXPECT_EQ(dgc.Call(Set()), 0);// Empty set is zero
        EXPECT_EQ(dgc.Call(Set::FromIndices(3, { 0 })), 0);
        EXPECT_EQ(dgc.Call(Set::FromIndices(3, { 1 })), 1);
        EXPECT_EQ(dgc.Call(Set::FromIndices(3, { 2 })), 6);
        EXPECT_EQ(dgc.Call(Set::FromIndices(3, { 0, 1})), 0);
        EXPECT_EQ(dgc.Call(Set::FromIndices(3, { 0, 2 })), 1);
        EXPECT_EQ(dgc.Call(Set::FromIndices(3, { 1, 2 })), 6);
        EXPECT_EQ(dgc.Call(Set::MakeDense(3)), 0);
    }
    TEST_F(PINModelTest, DGraphPSP) {
        SimpleGraph<float> sg = make_dgraph(n_1, edge_list_float_1);
        DirectedGraphCutOracle<float>* dgc = new DirectedGraphCutOracle<float>(sg);

        PSP<float> psp_class(dgc);
        psp_class.run();
        std::vector<float> gamma_list = psp_class.Get_critical_values();
        std::vector<std::vector<Set>> psp_list = psp_class.Get_psp();
        EXPECT_EQ(gamma_list.size(), 3);
        EXPECT_EQ(gamma_list[0], 2);
        EXPECT_EQ(gamma_list[1], 5);

        EXPECT_EQ(psp_list.size(), 3);

        EXPECT_EQ(psp_list[0].size(), 1);
        EXPECT_EQ(psp_list[0][0], Set::MakeDense(3));

        EXPECT_EQ(psp_list[1].size(), 2);
        EXPECT_EQ(psp_list[1][0], Set::FromIndices(3, { 1 }));
        EXPECT_EQ(psp_list[1][1], Set::FromIndices(3, { 0, 2 }));

        EXPECT_EQ(psp_list[2].size(), 3);
        EXPECT_EQ(psp_list[2][0], Set::FromIndices(3, { 0 }));
        EXPECT_EQ(psp_list[2][1], Set::FromIndices(3, { 1 }));
        EXPECT_EQ(psp_list[2][2], Set::FromIndices(3, { 2 }));

        delete dgc;
        // partition list is {{{0, 1, 2}}, {{0, 2}, {1}}, {{0}, {1}, {2}}}  
        // gamma list is {2, 5}       
    }
}