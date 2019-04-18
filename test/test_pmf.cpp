#include <gtest/gtest.h>
#include <cmath>
#include "core/pmf.h"
#include "utility/gaussian2Dcase.h"
namespace parametric {
TEST(PMF, EquationSolve) {
    std::vector<pair> parameter_list;
    parameter_list.push_back(std::make_pair(2, 0));
    parameter_list.push_back(std::make_pair(0, 0));
    std::vector<pair> y_lambda;
    lemon::ListDigraph g;
    lemon::ListDigraph::ArcMap<double> aM(g);
    PMF pmf(g, aM, 0, y_lambda);
    double lambda_1 = pmf.compute_lambda(parameter_list, -1.0);
    EXPECT_DOUBLE_EQ(lambda_1, 0.5);
    double lambda_2 = pmf.compute_lambda(parameter_list, -6.0);
    EXPECT_DOUBLE_EQ(lambda_2, 2);
    EXPECT_THROW(pmf.compute_lambda(parameter_list, 1), std::range_error);
    double lambda_4 = pmf.compute_lambda(parameter_list, 0);
    EXPECT_DOUBLE_EQ(lambda_4, 0);
    double lambda_5 = pmf.compute_lambda(parameter_list, -2);
    EXPECT_DOUBLE_EQ(lambda_5, 1);
    parameter_list[0].first = 0;
    double lambda_6 = pmf.compute_lambda(parameter_list, -2); // slope = -4 for lambda > 0
    EXPECT_DOUBLE_EQ(lambda_6, 0.5);
    parameter_list[0].second = INFINITY;
    double lambda_7 = pmf.compute_lambda(parameter_list, 2);
    EXPECT_DOUBLE_EQ(lambda_7, -1);
    double lambda_8 = pmf.compute_lambda(parameter_list, -2);
    EXPECT_DOUBLE_EQ(lambda_8, 0.5);
    parameter_list[1].second = INFINITY;
    double lambda_9 = pmf.compute_lambda(parameter_list, 2);
    EXPECT_DOUBLE_EQ(lambda_9, -0.5);
    parameter_list[0] = std::make_pair(0.909, INFINITY);
    parameter_list[1] = std::make_pair(0.99, -0.737);
    double lambda_10 = pmf.compute_lambda(parameter_list, -0.1);
    EXPECT_DOUBLE_EQ(lambda_10, 0.136);

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
TEST(PM, COPY_ELEVATOR) {
    lemon::ListDigraph g;
    g.reserveNode(4);
    g.reserveArc(5);
    lemon::ListDigraph::Node source_node = g.addNode();
    lemon::ListDigraph::Node sink_node = g.addNode();
    lemon::ListDigraph::Node node_1 = g.addNode();
    lemon::ListDigraph::Node node_2 = g.addNode();
    // construct edge cost map
    typedef lemon::ListDigraph::ArcMap<double> ArcMap;
    ArcMap edge_cost_map(g);
    lemon::ListDigraph::Arc arc1 = g.addArc(source_node, node_1);
    edge_cost_map[arc1] = 2;
    lemon::ListDigraph::Arc arc2 = g.addArc(node_1, sink_node);
    edge_cost_map[arc2] = 1;
    lemon::ListDigraph::Arc arc3 = g.addArc(source_node, node_2);
    edge_cost_map[arc3] = 4;
    lemon::ListDigraph::Arc arc4 = g.addArc(node_2, sink_node);
    edge_cost_map[arc4] = 5;
    lemon::ListDigraph::Arc arc5 = g.addArc(node_1, node_2);
    edge_cost_map[arc5] = 3;

    lemon::Preflow<lemon::ListDigraph, ArcMap> pf(g, edge_cost_map, source_node, sink_node);
    pf.init();
    lemon::Preflow<lemon::ListDigraph, ArcMap> pf_2(g, edge_cost_map, source_node, sink_node);
    const Elevator* ele_1 = &pf.elevator();
    pf_2.copyElevator(*ele_1);
    const Elevator* ele_2 = &pf_2.elevator();
    for (lemon::ListDigraph::NodeIt n(g); n != lemon::INVALID; ++n) {
        EXPECT_EQ((*ele_1)[n], (*ele_2)[n]);
    }
    pf.startFirstPhase();
    lemon::Preflow<lemon::ListDigraph, ArcMap> pf_3(g, edge_cost_map, source_node, sink_node);
    pf_3.copyElevator(*ele_1);
    const Elevator* ele_3 = &pf_3.elevator();
    for (lemon::ListDigraph::NodeIt n(g); n != lemon::INVALID; ++n) {
        EXPECT_EQ((*ele_1)[n], (*ele_3)[n]);
    }
    pf.startSecondPhase();
    const FlowMap* fM = &pf.flowMap();
    // check that the label is valid for the flowMap
    for (lemon::ListDigraph::ArcIt e(g); e != lemon::INVALID; ++e) {
        if ((*fM)[e] < edge_cost_map[e])
            EXPECT_TRUE((*ele_3)[g.source(e)] <= (*ele_3)[g.target(e)] + 1);
    }
   
}
TEST(PM, TEST_VALID_LABEL) {
    // this test is used to test after the init method, the label is valid and nodes
    // are activated in the right way
    // example taken from https://en.wikipedia.org/wiki/Push¨Crelabel_maximum_flow_algorithm
    lemon::ListDigraph g;
    g.reserveNode(6);
    g.reserveArc(8);
    lemon::ListDigraph::Node source_node = g.addNode();
    lemon::ListDigraph::Node sink_node = g.addNode();
    lemon::ListDigraph::Node node_a = g.addNode();
    lemon::ListDigraph::Node node_b = g.addNode();
    lemon::ListDigraph::Node node_c = g.addNode();
    lemon::ListDigraph::Node node_d = g.addNode();
    // construct edge cost map
    typedef lemon::ListDigraph::ArcMap<double> ArcMap;
    ArcMap edge_cost_map(g);
    lemon::ListDigraph::Arc arc1 = g.addArc(source_node, node_a);
    edge_cost_map[arc1] = 15;
    lemon::ListDigraph::Arc arc2 = g.addArc(node_a, node_b);
    edge_cost_map[arc2] = 12;
    lemon::ListDigraph::Arc arc3 = g.addArc(source_node, node_c);
    edge_cost_map[arc3] = 4;
    lemon::ListDigraph::Arc arc4 = g.addArc(node_c, node_d);
    edge_cost_map[arc4] = 10;
    lemon::ListDigraph::Arc arc5 = g.addArc(node_b, node_c);
    edge_cost_map[arc5] = 3;
    lemon::ListDigraph::Arc arc6 = g.addArc(node_d, node_a);
    edge_cost_map[arc6] = 5;
    lemon::ListDigraph::Arc arc7 = g.addArc(node_b, sink_node);
    edge_cost_map[arc7] = 7;
    lemon::ListDigraph::Arc arc8 = g.addArc(node_d, sink_node);
    edge_cost_map[arc8] = 10;
    lemon::Preflow<lemon::ListDigraph, ArcMap> pf(g, edge_cost_map, source_node, sink_node);
    pf.init();
    lemon::Preflow<lemon::ListDigraph, ArcMap> pf_2(g, edge_cost_map, source_node, sink_node);
    pf.startFirstPhase();
    Elevator* new_elevator = Preflow::Traits::createElevator(g, 6);
    const Elevator* ele_1 = &pf.elevator();
    for (lemon::ListDigraph::NodeIt n(g); n != lemon::INVALID; ++n) {
        EXPECT_FALSE(ele_1->active(n));
    }
    Preflow::copy_elevator(g, pf.elevator(), new_elevator);
    // check the active status of new_elevator

    pf.startSecondPhase();
    const FlowMap* flowMap = &pf.flowMap();
    // check that the label is valid for the flowMap
    for (lemon::ListDigraph::ArcIt e(g); e != lemon::INVALID; ++e) {
        if ((*flowMap)[e] < edge_cost_map[e])
            EXPECT_TRUE((*new_elevator)[g.source(e)] <= (*new_elevator)[g.target(e)] + 1);
    }
    edge_cost_map[arc7] = 6;
    pf_2.init(*flowMap, *new_elevator);
    pf_2.startFirstPhase();
    pf_2.startSecondPhase();
    std::cout << pf_2.flowValue();
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
        EXPECT_EQ(*partition_it, parametric::Partition::makeFine(4));
        delete pdt;
    }
    TEST(GivenPoint8, SMALL) {
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
        lemon::ListDigraph g;
        g.reserveNode(8);
        for (int i = 0; i < 8; i++)
            g.addNode();
        lemon::ListDigraph::ArcMap<double> aM(g);
        for (std::tuple<std::size_t, std::size_t, double>& edge_tuple : elt) {
            lemon::ListDigraph::Node s = g.nodeFromId(std::get<0>(edge_tuple));
            lemon::ListDigraph::Node t = g.nodeFromId(std::get<1>(edge_tuple));
            lemon::ListDigraph::Arc a1 = g.addArc(s, t);
            lemon::ListDigraph::Arc a2 = g.addArc(t, s);
            aM[a1] = std::get<2>(edge_tuple);
            aM[a2] = std::get<2>(edge_tuple);
        }
        stl::CSet s(std::string("00001111"));
        double cut_value = parametric::compute_cut(g, aM, s);
        std::vector<pair> parameter_list;
        for(int i = 0; i < 8; i++)
            parameter_list.push_back(std::make_pair(0, -0.0286));
        parameter_list[0] = std::make_pair(0.993, -0.737);
        parameter_list[1] = std::make_pair(0.909, INFINITY);

        parametric::PMF pmf(g, aM, 2, parameter_list);
        pmf.run();
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
        parametric::PDT* pdt = parametric::make_pdt(8, elt);
        pdt->run();
        std::list<double> lambda_list = pdt->get_lambda_list();
        std::list<parametric::Partition> partition_list = pdt->get_partition_list();

        submodular::InfoCluster ic(elt, 8);
        ic.run();
        std::vector<double> lambda_list_2 = ic.get_gamma_list();
        std::vector<std::vector<submodular::Set>> partition_list_2 = ic.get_psp_list();

        EXPECT_EQ(lambda_list.size(), lambda_list_2.size());
        lemon::Tolerance<double> Tol;
        
        std::list<double>::iterator it = lambda_list.begin();
        for (int vector_it = 0; vector_it < lambda_list_2.size()-1; vector_it++) {
            EXPECT_NEAR(*it, lambda_list_2[vector_it], Tol.epsilon());
            it++;
        }
        std::list<parametric::Partition>::iterator partition_it = partition_list.begin();
        for (int i = 0; i < partition_list_2.size(); i++) {
            if (partition_list_2[i].size() == 0)
                continue;
            std::vector<submodular::Set> set = partition_list_2[i];
            parametric::Partition p;
            // convert one type to another and compare the two
            for (submodular::Set& s : set) {
                p.AddElement(stl::CSet(s.GetRepresentation()));
            }
            EXPECT_EQ(p, *partition_it);
            partition_it++;
        }

    }
}