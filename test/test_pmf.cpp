#include <gtest/gtest.h>
#include <cmath>
#include "core/pmf.h"
#include "utility/gaussian2Dcase.h"
namespace parametric {
TEST(PMF, EquationSolve) {
    std::vector<pair> parameter_list;
    parameter_list.push_back(std::make_pair(2, 0));
    parameter_list.push_back(std::make_pair(0, 0));
    double lambda_1 = compute_lambda(parameter_list, -1.0);
    EXPECT_DOUBLE_EQ(lambda_1, 0.5);
    double lambda_2 = compute_lambda(parameter_list, -6.0);
    EXPECT_DOUBLE_EQ(lambda_2, 2);
    EXPECT_THROW(compute_lambda(parameter_list, 1), std::range_error);
    double lambda_4 = compute_lambda(parameter_list, 0);
    EXPECT_DOUBLE_EQ(lambda_4, 0);
    double lambda_5 = compute_lambda(parameter_list, -2);
    EXPECT_DOUBLE_EQ(lambda_5, 1);
    parameter_list[0].first = 0;
    double lambda_6 = compute_lambda(parameter_list, -2); // slope = -4 for lambda > 0
    EXPECT_DOUBLE_EQ(lambda_6, 0.5);
    parameter_list[0].second = INFINITY;
    double lambda_7 = compute_lambda(parameter_list, 2);
    EXPECT_DOUBLE_EQ(lambda_7, -1);
    double lambda_8 = compute_lambda(parameter_list, -2);
    EXPECT_DOUBLE_EQ(lambda_8, 0.5);
    parameter_list[1].second = INFINITY;
    double lambda_9 = compute_lambda(parameter_list, 2);
    EXPECT_DOUBLE_EQ(lambda_9, -0.5);
}
TEST(PMF, insert_set) {
    using Set = stl::CSet;
    std::vector<pair> y_lambda;
    lemon::ListDigraph g;
    lemon::ListDigraph::ArcMap<double> aM(g);
    PMF pmf(g, aM, 0, y_lambda);
    Set V1(std::string("0001"));
    Set V2(std::string("1001"));
    Set V3(std::string("1101"));

    pmf.insert_set(V3);
    pmf.insert_set(V1);
    pmf.insert_set(V2);
    std::list<Set> set_list = pmf.get_set_list();
    std::list<Set>::iterator i = set_list.begin();
    EXPECT_EQ(*i, V3);
    i++;
    EXPECT_EQ(*i, V2);
    i++;
    EXPECT_EQ(*i, V1);
}
TEST(PMF, insert) {
    std::vector<pair> y_lambda;
    lemon::ListDigraph g;
    lemon::ListDigraph::ArcMap<double> aM(g);
    PMF pmf(g, aM, 0, y_lambda);
    pmf.insert(4);
    pmf.insert(6);
    pmf.insert(5);
    std::list<double> lambda_list = pmf.get_lambda_list();
    std::list<double>::iterator i = lambda_list.begin();
    EXPECT_DOUBLE_EQ(*i, 4);
    i++;
    EXPECT_DOUBLE_EQ(*i, 5);
    i++;
    EXPECT_DOUBLE_EQ(*i, 6);
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
    lemon::ListDigraph::Arc a2 = g.addArc(n2, n1);
    lemon::ListDigraph::Arc a3 = g.addArc(n1, n3);
    lemon::ListDigraph::Arc a4 = g.addArc(n3, n1);
    lemon::ListDigraph::Arc a5 = g.addArc(n2, n3);
    lemon::ListDigraph::Arc a6 = g.addArc(n3, n2);

    lemon::ListDigraph::ArcMap<double> aM(g);
    aM[a1] = 1;
    aM[a2] = 1;
    aM[a3] = 5;
    aM[a4] = 5;
    aM[a5] = 1;
    aM[a6] = 1;
    PMF pmf(g, aM, 2, parameter_list);
    pmf.run();

    std::list<Set> sL = pmf.get_set_list();
    EXPECT_EQ(sL.size(), 3);
    std::list<Set>::iterator it = sL.begin();
    EXPECT_EQ(*it, Set(std::string("111"))); // { 0, 1, 2 }
    it++;
    EXPECT_EQ(*it, Set(std::string("101"))); // { 0, 2 }
    it++;
    EXPECT_EQ(*it, Set(std::string("001"))); // { 2 }

    pmf.reset_j(0);
    pmf.run();
    for (Set& s : pmf.get_set_list())
        std::cout << s << std::endl;
}
TEST(PDT, RUN) {
    lemon::ListDigraph g;
    lemon::ListDigraph::Node n1 = g.addNode();
    lemon::ListDigraph::Node n2 = g.addNode();
    lemon::ListDigraph::Node n3 = g.addNode();

    lemon::ListDigraph::Arc a1 = g.addArc(n1, n2);
    lemon::ListDigraph::Arc a2 = g.addArc(n2, n1);
    lemon::ListDigraph::Arc a3 = g.addArc(n1, n3);
    lemon::ListDigraph::Arc a4 = g.addArc(n3, n1);
    lemon::ListDigraph::Arc a5 = g.addArc(n2, n3);
    lemon::ListDigraph::Arc a6 = g.addArc(n3, n2);

    lemon::ListDigraph::ArcMap<double> aM(g);
    aM[a1] = 1;
    aM[a2] = 1;
    aM[a3] = 5;
    aM[a4] = 5;
    aM[a5] = 1;
    aM[a6] = 1;
    PDT pdt(g, aM);
    pdt.run();
    std::list<double> lambda_list = pdt.get_lambda_list();
    std::list<Partition> partition_list = pdt.get_partition_list();
    for (double d : lambda_list) {
        std::cout << d << std::endl;
    }
    for (Partition &p : partition_list) {
        std::cout << p << std::endl;
    }
}
TEST(Set, Partition) {
    Partition a, b, c;
    a.AddElement(stl::CSet(std::string("101")));
    a.AddElement(stl::CSet(std::string("01")));
    b.AddElement(stl::CSet(std::string("01")));
    b.AddElement(stl::CSet(std::string("101")));
    EXPECT_EQ(a, b);
    c = c.expand(stl::CSet(std::string("1")));
    c = c.expand(stl::CSet(std::string("01")));
    c = c.expand(stl::CSet(std::string("101")));
    EXPECT_EQ(a, c);
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
    stl::CSet s;
    s.AddElement(0);
    EXPECT_DOUBLE_EQ(compute_cut(g, aM, s), 5);
    s.AddElement(1);
    EXPECT_DOUBLE_EQ(compute_cut(g, aM, s), 6);
    s.AddElement(2);
    EXPECT_DOUBLE_EQ(compute_cut(g, aM, s), 0);
}
}
namespace demo {
    TEST_F(Graph4PointTest, PDT) {
        parametric::PDT* pdt = parametric::make_pdt(4, edge_list_tuple_1);
        pdt->run();
        std::list<double> lambda_list = pdt->get_lambda_list();
        std::list<parametric::Partition> partition_list = pdt->get_partition_list();
        EXPECT_EQ(lambda_list.size(), 2);
        std::list<double>::iterator lambda_it = lambda_list.begin();
        EXPECT_DOUBLE_EQ(*lambda_it, 1 + 2 / 3.0);

        EXPECT_EQ(partition_list.size(), 2);
        std::list<parametric::Partition>::iterator partition_it = partition_list.begin();
        
        EXPECT_EQ(*partition_it, parametric::Partition::makeDense(4));
        partition_it++;
        EXPECT_EQ(*partition_it, parametric::Partition::makeFile(4));
        delete pdt;
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
        Gaussian2DGraphBase g2g(8, 1.0, a);
    }
}