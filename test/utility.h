#pragma once
// contains test fixture
#include <gtest/gtest.h>
#include <vector>
#include <tuple>
#include <lemon/list_graph.h>
#include "psp/graph.h"
#include "gaussian2Dcase.h"

namespace demo {

class Graph4PointTest : public testing::Test {
protected:
    EdgeListTuple edges;
    std::vector<double> lambda_list;
    std::vector<parametric::Partition> partition_list;
    void SetUp() override {
        edges.push_back(std::make_tuple(0, 1, 1));
        edges.push_back(std::make_tuple(1, 2, 1));
        edges.push_back(std::make_tuple(2, 3, 1));
        edges.push_back(std::make_tuple(0, 3, 1));
        edges.push_back(std::make_tuple(0, 2, 0.5));
        edges.push_back(std::make_tuple(1, 3, 0.5));
    }
    void TearDown() override {
        EXPECT_EQ(lambda_list.size(), 1);
        std::vector<double>::iterator lambda_it = lambda_list.begin();
        EXPECT_DOUBLE_EQ(*lambda_it, 1 + 2 / 3.0);

        EXPECT_EQ(partition_list.size(), 2);
        std::vector<parametric::Partition>::iterator partition_it = partition_list.begin();
            
        EXPECT_EQ(*partition_it, parametric::Partition::makeDense(4));
        partition_it++;
        EXPECT_EQ(*partition_it, parametric::Partition::makeFine(4));
    }
};

class Graph4PointTestGraph : public testing::Test {
protected:
    lemon::ListDigraph g;
    lemon::ListDigraph::ArcMap<double> arc_map;
    Graph4PointTestGraph(): arc_map(g){}
    void SetUp() override {
        for (int i = 0; i < 4; i ++){
            g.addNode();
        }
        lemon::ListDigraph::Arc a1 = g.addArc(g.nodeFromId(0), g.nodeFromId(1));
        lemon::ListDigraph::Arc a2 = g.addArc(g.nodeFromId(1), g.nodeFromId(2));
        lemon::ListDigraph::Arc a3 = g.addArc(g.nodeFromId(2), g.nodeFromId(3));
        lemon::ListDigraph::Arc a4 = g.addArc(g.nodeFromId(0), g.nodeFromId(3));
        lemon::ListDigraph::Arc a5 = g.addArc(g.nodeFromId(0), g.nodeFromId(2));
        lemon::ListDigraph::Arc a6 = g.addArc(g.nodeFromId(1), g.nodeFromId(3));
        arc_map[a1] = 1;
        arc_map[a2] = 1;
        arc_map[a3] = 1;
        arc_map[a4] = 1;
        arc_map[a5] = 0.5;
        arc_map[a6] = 0.5;
    }
};

class FourPointNotCompleteGraph : public testing::Test {
    protected:
        lemon::ListDigraph g;
        lemon::ListDigraph::ArcMap<double> arc_map;
        FourPointNotCompleteGraph(): arc_map(g){}
        void SetUp() override {
            for (int i = 0; i < 4; i ++){
                g.addNode();
            }
            lemon::ListDigraph::Arc a1 = g.addArc(g.nodeFromId(0), g.nodeFromId(1));
            lemon::ListDigraph::Arc a2 = g.addArc(g.nodeFromId(2), g.nodeFromId(3));
            arc_map[a1] = 1;
            arc_map[a2] = 1;                
        }
};

class FourPointNotComplete : public testing::Test {
    protected:
        std::vector<std::tuple<std::size_t, std::size_t, double>> edges;
        std::vector<double> lambda_list;
        std::vector<parametric::Partition> partition_list;
        void SetUp() override {
            edges.push_back(std::make_tuple(0, 1, 1.0));
            edges.push_back(std::make_tuple(2, 3, 1.0));
        }
        void TearDown() override {
            std::vector<double>::iterator it = lambda_list.begin();
            EXPECT_DOUBLE_EQ(*it, 0);
            it++;
            EXPECT_DOUBLE_EQ(*it, 1);

            EXPECT_EQ(lambda_list.size(), 2);  
            EXPECT_EQ(partition_list.size(), 3);

            stl::Partition p = stl::Partition::makeDense(4);

            std::vector<parametric::Partition>::iterator it_p = partition_list.begin();
            EXPECT_EQ(*it_p, p);

            it_p++;
            p.clear();
            p.AddElement(stl::CSet(std::string("1100")));
            p.AddElement(stl::CSet(std::string("0011")));
            EXPECT_EQ(*it_p, p);

            p = stl::Partition::makeFine(4);
            it_p++;
            EXPECT_EQ(*it_p, p);
        }
};

class ThreePointComplete : public testing::Test {
    protected:
        std::vector<std::tuple<std::size_t, std::size_t, double>> edges;
        std::vector<double> lambda_list;
        std::vector<parametric::Partition> partition_list;
        void SetUp() override {
            edges.push_back(std::make_tuple(0, 1, 1.0));
            edges.push_back(std::make_tuple(0, 2, 1.0));
            edges.push_back(std::make_tuple(1, 2, 5.0));
        }
        void TearDown() override {
            std::vector<double>::iterator it = lambda_list.begin();
            EXPECT_DOUBLE_EQ(*it, 2);
            it++;
            EXPECT_DOUBLE_EQ(*it, 5);
            EXPECT_EQ(lambda_list.size(), 2);
            EXPECT_EQ(partition_list.size(), 3);

            stl::Partition p = stl::Partition::makeDense(3);
            std::vector<parametric::Partition>::iterator it_p = partition_list.begin();
            EXPECT_EQ(*it_p, p);

            it_p++;
            p.clear();
            p.AddElement(stl::CSet(std::string("011")));
            p.AddElement(stl::CSet(std::string("100")));
            EXPECT_EQ(*it_p, p);

            p = stl::Partition::makeFine(3);
            it_p++;
            EXPECT_EQ(*it_p, p);            
        }
};

}

