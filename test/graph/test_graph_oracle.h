#pragma once

#include "core/graph/generalized_cut.h"
#include "test/graph/test_graph_base.h"

namespace submodular {
    class PINModelTest : public testing::Test {
    protected:
        using EdgeListFloat = std::vector<std::tuple<std::size_t, std::size_t, float>>;

        // {src, dst, capacity}
        std::size_t n_1 = 3;
        std::size_t data_1[3][3] = {
        { 0, 1, 3 },
        { 0, 2, 1 },
        { 1, 3, 3 },
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
}