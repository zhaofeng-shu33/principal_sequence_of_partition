#pragma once
#include <gtest/gtest.h>
#include <cmath>
#include "psp/pmf.h"
#include "psp/psp.h"
#include "gaussian2Dcase.h"
#include "utility.h"
namespace parametric {
TEST(PMF, EquationSolve) {
    std::vector<pair> parameter_list;
    parameter_list.push_back(std::make_pair(2, 0));
    parameter_list.push_back(std::make_pair(0, 0));
    std::vector<pair> y_lambda;
    lemon::ListDigraph g;
    g.addNode();
    lemon::ListDigraph::ArcMap<double> aM(g);
    PMF pmf(&g, &aM, 1, y_lambda);
    double lambda_1 = pmf.compute_lambda(parameter_list, -0.5);
    EXPECT_DOUBLE_EQ(lambda_1, 0.5);
    double lambda_2 = pmf.compute_lambda(parameter_list, -3.0);
    EXPECT_DOUBLE_EQ(lambda_2, 2.5);
    EXPECT_THROW(pmf.compute_lambda(parameter_list, 1), std::range_error);
    // double lambda_4 = pmf.compute_lambda(parameter_list, 0);
    // EXPECT_DOUBLE_EQ(lambda_4, 0);
    double lambda_5 = pmf.compute_lambda(parameter_list, -1);
    EXPECT_DOUBLE_EQ(lambda_5, 1);
    parameter_list[0].first = 0;
    double lambda_6 = pmf.compute_lambda(parameter_list, -2); // slope = -2 for lambda > 0
    EXPECT_DOUBLE_EQ(lambda_6, 1);
    parameter_list[0].second = INFINITY;
    double lambda_7 = pmf.compute_lambda(parameter_list, 2);
    EXPECT_DOUBLE_EQ(lambda_7, -2);
    double lambda_8 = pmf.compute_lambda(parameter_list, -2);
    EXPECT_DOUBLE_EQ(lambda_8, 1);
    parameter_list[1].second = INFINITY;
    double lambda_9 = pmf.compute_lambda(parameter_list, 2);
    EXPECT_DOUBLE_EQ(lambda_9, -1);

    // test left value
    parameter_list.pop_back();
    parameter_list[0] = std::make_pair(1,-1);
    double lambda_11 = pmf.compute_lambda(parameter_list, -1);
    EXPECT_DOUBLE_EQ(lambda_11, 2);
}

TEST(PMF, PMClass) {
    using Set = stl::CSet;
    std::vector<pair> parameter_list;
    parameter_list.push_back(std::make_pair(0, INFINITY));
    parameter_list.push_back(std::make_pair(0, INFINITY));
    parameter_list.push_back(std::make_pair(0, INFINITY));

    lemon::ListDigraph g;
    lemon::ListDigraph::Node n1 = g.addNode();
    lemon::ListDigraph::Node n2 = g.addNode();
    lemon::ListDigraph::Node n3 = g.addNode();

    lemon::ListDigraph::Arc a1 = g.addArc(n1, n2);
    lemon::ListDigraph::Arc a3 = g.addArc(n1, n3);
    lemon::ListDigraph::Arc a5 = g.addArc(n2, n3);

    lemon::ListDigraph::ArcMap<double> aM(g);
    aM[a1] = 1;
    aM[a3] = 5;
    aM[a5] = 1;
    PMF pmf(&g, &aM, 2, parameter_list);
    pmf.set_node_filter(true);
    pmf.run();

    std::list<Set> sL = pmf.get_set_list();
    EXPECT_EQ(sL.size(), 3);
    std::list<Set>::iterator it = sL.begin();
    EXPECT_EQ(*it, Set(std::string("111"))); // { 0, 1, 2 }
    it++;
    EXPECT_EQ(*it, Set(std::string("101"))); // { 0, 2 }
    it++;
    EXPECT_EQ(*it, Set(std::string("001"))); // { 2 }

    std::list<double> lambda_list = pmf.get_lambda_list();
    EXPECT_EQ(lambda_list.size(), 2);
    std::list<double>::iterator it_2 = lambda_list.begin();
    EXPECT_DOUBLE_EQ(*it_2, 1);
    it_2++;
    EXPECT_DOUBLE_EQ(*it_2, 5);

}

TEST(PDT, RUN) {
    std::vector<std::tuple<std::size_t, std::size_t, double>> elt;
    elt.push_back(std::make_tuple(0, 1, 1));
    elt.push_back(std::make_tuple(0, 2, 5));
    elt.push_back(std::make_tuple(1, 2, 1));
    psp::PSP ic(elt, 3);
    ic.run("pdt");
    std::vector<double> lambda_list = ic.get_critical_values();
    std::vector<Partition> partition_list = ic.get_partitions();
    for (double d : lambda_list) {
        std::cout << d << std::endl;
    }
    for (Partition &p : partition_list) {
        std::cout << p << std::endl;
    }
}

TEST(PMF, ComputeCut) {
    lemon::ListDigraph g;
    lemon::ListDigraph::Node n1 = g.addNode();
    lemon::ListDigraph::Node n2 = g.addNode();
    lemon::ListDigraph::Node n3 = g.addNode();

    lemon::ListDigraph::Arc a1 = g.addArc(n1, n2);
    lemon::ListDigraph::Arc a2 = g.addArc(n2, n3);
    lemon::ListDigraph::Arc a3 = g.addArc(n1, n3);

    lemon::ListDigraph::ArcMap<double> aM(g);
    aM[a1] = 1;
    aM[a2] = 2;
    aM[a3] = 4;
    stl::CSet t;
    t.AddElement(2);
    EXPECT_DOUBLE_EQ(submodular::get_cut_value(g, aM, t), 6);
    t.AddElement(1);
    EXPECT_DOUBLE_EQ(submodular::get_cut_value(g, aM, t), 5);
    t.AddElement(0);
    EXPECT_DOUBLE_EQ(submodular::get_cut_value(g, aM, t), 0);
}
}
namespace demo {
TEST_F(Graph4PointTest, PDT) {
    psp::PSP ic(edges, 4);
    ic.run("pdt");
    lambda_list = ic.get_critical_values();
    partition_list = ic.get_partitions();
}

TEST(GivenPoint8, PDT) {
    double a[8][2] = { {3.1, 3.2},
                        {4.0, 4.0 },
                        {1.1, -2.2},
                        {3.9, -2.0},
                        {-3.9, -2.0},
                        {-2.2, -3.5},
                        {-3.9, 2.4},
                        {-3.1, 2.6}
    };
    Gaussian2DGraphBase g2g(8, 0.1, a);
    EdgeListTuple elt = g2g.get_edge_list_tuple();
    psp::PSP ic(elt, 8);
    ic.run("pdt");
    std::vector<double> lambda_list = ic.get_critical_values();
    std::vector<parametric::Partition> partition_list = ic.get_partitions();


    ic.run("dt");
    std::vector<double> lambda_list_2 = ic.get_critical_values();
    std::vector<stl::Partition> partition_list_2 = ic.get_partitions();

    EXPECT_EQ(lambda_list.size(), lambda_list_2.size());
    lemon::Tolerance<double> Tol;
        
    std::vector<double>::iterator it = lambda_list.begin();
    ;
    for (std::vector<double>::iterator it_2 = lambda_list_2.begin(); it_2 != lambda_list_2.end(); it_2++) {
        EXPECT_NEAR(*it, *it_2, Tol.epsilon());
        it++;
    }
    EXPECT_EQ(partition_list, partition_list_2);
}

TEST_F(ThreePointComplete, PDT) {
    psp::PSP ic(edges, 3);
    ic.run("pdt");
    lambda_list = ic.get_critical_values();
    partition_list = ic.get_partitions();
}

TEST_F(FourPointNotComplete, PDT) {
    psp::PSP ic(edges, 4);
    ic.run("pdt");    
    lambda_list = ic.get_critical_values();
    partition_list = ic.get_partitions();
}
}