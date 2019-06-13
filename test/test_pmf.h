#pragma once
#include <gtest/gtest.h>
#include <cmath>
#include "core/pmf.h"
#include "core/graph/info_cluster.h"
#include "core/graph/gaussian2Dcase.h"
#include "test/utility.h"
namespace parametric {
TEST(PMF, EquationSolve) {
    std::vector<pair> parameter_list;
    parameter_list.push_back(std::make_pair(2, 0));
    parameter_list.push_back(std::make_pair(0, 0));
    std::vector<pair> y_lambda;
    lemon::ListDigraph g;
    lemon::ListDigraph::ArcMap<double> aM(g);
    PMF pmf(&g, &aM, 0, y_lambda);
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

    pmf.reset_j(0);
    pmf.run();
    for (Set& s : pmf.get_set_list())
        std::cout << s << std::endl;
}


TEST(PDT, RUN) {
	std::vector<std::tuple<std::size_t, std::size_t, double>> elt;
	elt.push_back(std::make_tuple(0, 1, 1));
	elt.push_back(std::make_tuple(0, 2, 5));
	elt.push_back(std::make_tuple(1, 2, 1));
	submodular::InfoCluster ic(elt, 3);
    ic.run_pdt();
    std::list<double> lambda_list = ic.get_critical_values();
    std::list<Partition> partition_list = ic.get_psp();
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
		submodular::InfoCluster ic(edge_list_tuple_1, 4);
		ic.run_pdt();
        std::list<double> lambda_list = ic.get_critical_values();
        std::list<parametric::Partition> partition_list = ic.get_psp();
        EXPECT_EQ(lambda_list.size(), 1);
        std::list<double>::iterator lambda_it = lambda_list.begin();
        EXPECT_DOUBLE_EQ(*lambda_it, 1 + 2 / 3.0);

        EXPECT_EQ(partition_list.size(), 2);
        std::list<parametric::Partition>::iterator partition_it = partition_list.begin();
        
        EXPECT_EQ(*partition_it, parametric::Partition::makeDense(4));
        partition_it++;
        EXPECT_EQ(*partition_it, parametric::Partition::makeFine(4));

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
		submodular::InfoCluster ic(elt, 8);
		ic.run_pdt();
        std::list<double> lambda_list = ic.get_critical_values();
        std::list<parametric::Partition> partition_list = ic.get_psp();


        ic.run();
        std::list<double> lambda_list_2 = ic.get_gamma_list();
        std::list<stl::Partition> partition_list_2 = ic.get_psp();

        EXPECT_EQ(lambda_list.size(), lambda_list_2.size());
        lemon::Tolerance<double> Tol;
        
        std::list<double>::iterator it = lambda_list.begin();
		;
        for (std::list<double>::iterator it_2 = lambda_list_2.begin(); it_2 != lambda_list_2.end(); it_2++) {
            EXPECT_NEAR(*it, *it_2, Tol.epsilon());
            it++;
        }
		EXPECT_EQ(partition_list, partition_list_2);
    }
	TEST(ThreePointComplete, PDT) {
		std::vector<std::tuple<std::size_t, std::size_t, double>> edges;
		edges.push_back(std::make_tuple(0, 1, 1.0));
		edges.push_back(std::make_tuple(0, 2, 1.0));
		edges.push_back(std::make_tuple(1, 2, 5.0));
		submodular::InfoCluster ic(edges, 3);
		ic.run_pdt();
		std::list<double> lambda_list = ic.get_critical_values();
		std::list<parametric::Partition> partition_list = ic.get_psp();
		std::list<double>::iterator it = lambda_list.begin();
		EXPECT_DOUBLE_EQ(*it, 2);
		it++;
		EXPECT_DOUBLE_EQ(*it, 5);

		EXPECT_EQ(lambda_list.size(), 2); 
		EXPECT_EQ(partition_list.size(), 3);

		stl::Partition p = stl::Partition::makeDense(3);
		
		std::list<parametric::Partition>::iterator it_p = partition_list.begin();
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

	TEST(ThreePointNotComplete, PDT) {
		std::vector<std::tuple<std::size_t, std::size_t, double>> edges;
		edges.push_back(std::make_tuple(0, 1, 1.0));
		submodular::InfoCluster ic(edges, 3);
		ic.run_pdt();
		std::list<double> lambda_list = ic.get_critical_values();
		std::list<parametric::Partition> partition_list = ic.get_psp();
		std::list<double>::iterator it = lambda_list.begin();
		EXPECT_DOUBLE_EQ(*it, 0);
		it++;
		EXPECT_DOUBLE_EQ(*it, 1);

		EXPECT_EQ(lambda_list.size(), 2); 
		EXPECT_EQ(partition_list.size(), 3);

		stl::Partition p = stl::Partition::makeDense(3);

		std::list<parametric::Partition>::iterator it_p = partition_list.begin();
		EXPECT_EQ(*it_p, p);

		it_p++;
		p.clear();
		p.AddElement(stl::CSet(std::string("110")));
		p.AddElement(stl::CSet(std::string("001")));
		EXPECT_EQ(*it_p, p);

		p = stl::Partition::makeFine(3);
		it_p++;
		EXPECT_EQ(*it_p, p);
	}

	TEST(FourPointNotComplete, PDT) {
		std::vector<std::tuple<std::size_t, std::size_t, double>> edges;
		edges.push_back(std::make_tuple(0, 1, 1.0));
		edges.push_back(std::make_tuple(2, 3, 1.0));
		submodular::InfoCluster ic(edges, 4);
		ic.run_pdt();
		std::list<double> lambda_list = ic.get_critical_values();
		std::list<parametric::Partition> partition_list = ic.get_psp();
		std::list<double>::iterator it = lambda_list.begin();
		EXPECT_DOUBLE_EQ(*it, 0);
		it++;
		EXPECT_DOUBLE_EQ(*it, 1);

		EXPECT_EQ(lambda_list.size(), 2);  
		EXPECT_EQ(partition_list.size(), 3);

		stl::Partition p = stl::Partition::makeDense(4);

		std::list<parametric::Partition>::iterator it_p = partition_list.begin();
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
}