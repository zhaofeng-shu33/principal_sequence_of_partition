#include <gtest/gtest.h>
#include "core/pmf.h"
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

TEST(PMF, PMClass) {
    std::vector<pair> parameter_list;
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
    submodular::PMF pmf(g, aM, parameter_list);
    pmf.init();

    std::list<submodular::Set> sL = pmf.get_set_list();
    for (submodular::Set& s : sL) {
        std::cout << s << std::endl;
    }
}