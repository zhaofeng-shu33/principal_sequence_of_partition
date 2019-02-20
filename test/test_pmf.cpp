#include <gtest/gtest.h>
#include "core/pmf.h"
namespace parametric {
TEST(PMF, EquationSolve) {
    std::vector<pair> parameter_list;
    parameter_list.push_back(std::make_pair(2, 0));
    parameter_list.push_back(std::make_pair(0, 0));
    double lambda_1 = compute_lambda(parameter_list, -1.0);
    EXPECT_DOUBLE_EQ(lambda_1, 0.5);
    double lambda_2 = compute_lambda(parameter_list, -6.0);
    EXPECT_DOUBLE_EQ(lambda_2, 2);
    double lambda_3 = compute_lambda(parameter_list, 1);
    EXPECT_DOUBLE_EQ(lambda_3, -1);
    double lambda_4 = compute_lambda(parameter_list, 0);
    EXPECT_DOUBLE_EQ(lambda_4, 0);
    double lambda_5 = compute_lambda(parameter_list, -2);
    EXPECT_DOUBLE_EQ(lambda_5, 1);
    parameter_list[0].first = 0;
    double lambda_6 = compute_lambda(parameter_list, -2); // slope = -4 for lambda > 0
    EXPECT_DOUBLE_EQ(lambda_6, 0.5);
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
    parameter_list.push_back(std::make_pair(0, 0));
    parameter_list.push_back(std::make_pair(0, 0));
    parameter_list.push_back(std::make_pair(0, 0));

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
TEST(Set, Partition) {
    std::set<int> a;
    a.insert(1);
    a.insert(2);
    std::set<int> b;
    b.insert(1);
    b.insert(2);
    EXPECT_TRUE(a == b);
}
}