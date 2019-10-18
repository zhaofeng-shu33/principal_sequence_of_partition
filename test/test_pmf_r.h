#pragma once
#include <lemon/lgf_reader.h>
#include <lemon/adaptors.h>

#include "psp/pmf_r.h"
#include "utility.h"
namespace parametric {
TEST(PMF_R, reverse) {
    lemon::ListDigraph digraph;
    ArcMap cap(digraph);
    std::string lgf_str = "@nodes\nlabel\n0\n1\n2\n3\n4\n@arcs\n\t\tlabel\tcapacity\n4\t0\t0\t1\n4\t2\t1\t2\n0\t1\t2\t3\n2\t0\t3\t5\n2\t3\t4\t6\n1\t2\t5\t4\n1\t3\t6\t7";
    std::stringstream ss;
    ss << lgf_str;
    lemon::digraphReader(digraph, ss)
        .arcMap("capacity", cap)
        .run();
    lemon::ReverseDigraph<lemon::ListDigraph> rdig(digraph);
    lemon::Preflow<lemon::ReverseDigraph<lemon::ListDigraph>, ArcMap> pf(rdig, cap, rdig.nodeFromId(3), rdig.nodeFromId(4));
    pf.run();
    EXPECT_DOUBLE_EQ(pf.flowValue(), 3);
    EXPECT_TRUE(pf.minCut(rdig.nodeFromId(0)));
    EXPECT_TRUE(pf.minCut(rdig.nodeFromId(1)));
    EXPECT_TRUE(pf.minCut(rdig.nodeFromId(2)));
    EXPECT_TRUE(pf.minCut(rdig.nodeFromId(3)));
    EXPECT_FALSE(pf.minCut(rdig.nodeFromId(4)));
}

TEST(PMF_R, execute_reverse) {
    lemon::ListDigraph digraph;
    ArcMap cap(digraph);
    std::vector<pair> y_lambda;
    std::string lgf_str = "@nodes\nlabel\n0\n1\n2\n3\n4\n@arcs\n\t\tlabel\tcapacity\n4\t0\t0\t1\n4\t2\t1\t2\n0\t1\t2\t3\n2\t0\t3\t5\n2\t3\t4\t6\n1\t2\t5\t4\n1\t3\t6\t7";
    std::stringstream ss;
    ss << lgf_str;
    lemon::digraphReader(digraph, ss)
        .arcMap("capacity", cap)
        .run();
    PMF_R pmfR(&digraph, &cap, 1, y_lambda);
    // construct a maximum flow map
    PMF_R::FlowMap fm(digraph);
    pmfR.set_source_node_id(4);
    pmfR.set_sink_node_id(3);
    pmfR.set_tilde_G_size(5);
    stl::CSet S("00001");
    stl::CSet T("00010");
    stl::CSet T_a;
    double n_a;
    PMF_R::FlowMap n_fm(digraph);
    lemon::ReverseDigraph<lemon::ListDigraph> reverse_newDig(digraph);
    PMF_R::ThreadArgumentPack TAP(digraph, cap, fm, S, T, T_a, n_a, n_fm, NULL, NULL, &reverse_newDig);
    InterruptibleThread::thread right(&PMF_R::executePreflow_reverse, &pmfR, std::ref(TAP));
    right.join();
    if (TAP.ele_reverse_out)
        delete TAP.ele_reverse_out;
    EXPECT_DOUBLE_EQ(n_a, 3);
    EXPECT_EQ(T_a, stl::CSet("1111"));
}

TEST(PMF_R, contract) {
    lemon::ListDigraph digraph;
    ArcMap cap(digraph);
    std::vector<pair> y_lambda;
    std::string lgf_str = "@nodes\nlabel\n0\n1\n2\n3\n4\n@arcs\n\t\tlabel\tcapacity\n4\t0\t0\t1\n4\t2\t1\t2\n0\t1\t2\t3\n2\t0\t3\t5\n2\t3\t4\t6\n1\t2\t5\t4\n1\t3\t6\t7";
    std::stringstream ss;
    ss << lgf_str;
    lemon::digraphReader(digraph, ss)
        .arcMap("capacity", cap)
        .run();
    PMF_R pmfR(&digraph, &cap, 1, y_lambda);
    pmfR.set_source_node_id(4);
    pmfR.set_sink_node_id(3);
    pmfR.set_tilde_G_size(5);
    using Set = stl::CSet;
    Set S("00101"), T("01010");
    lemon::ListDigraph new_digraph;
    ArcMap new_cap(new_digraph);
    pmfR.contract(S, T, digraph, cap, new_digraph, new_cap);
    EXPECT_EQ(lemon::countNodes(new_digraph), 3);
    EXPECT_EQ(lemon::countArcs(new_digraph), 4);
    std::map<int,std::map<int, double>> flowMap;
    for (lemon::ListDigraph::ArcIt a(new_digraph); a != lemon::INVALID; ++a) {
        int u_id = new_digraph.id(new_digraph.source(a));
        int v_id = new_digraph.id(new_digraph.target(a));
        flowMap[u_id][v_id] = new_cap[a];
    }
            
    EXPECT_DOUBLE_EQ(flowMap[4][0], 6);
    EXPECT_DOUBLE_EQ(flowMap[4][3], 6);
    EXPECT_DOUBLE_EQ(flowMap[3][4], 4);
    EXPECT_DOUBLE_EQ(flowMap[0][3], 3);
}

TEST(PMF_R, modifyFlow) {
    lemon::ListDigraph digraph;
    ArcMap cap(digraph);
    std::vector<pair> y_lambda;
    std::string lgf_str = "@nodes\nlabel\n0\n1\n2\n3\n4\n@arcs\n\t\tlabel\tcapacity\n4\t0\t0\t1\n4\t2\t1\t2\n0\t1\t2\t3\n2\t0\t3\t5\n2\t3\t4\t6\n1\t2\t5\t4\n1\t3\t6\t7";
    std::stringstream ss;
    ss << lgf_str;
    lemon::digraphReader(digraph, ss)
        .arcMap("capacity", cap)
        .run();
    PMF_R pmfR(&digraph, &cap, 1, y_lambda);
    pmfR.set_source_node_id(4);
    pmfR.set_sink_node_id(3);
    pmfR.set_tilde_G_size(5);
    using Set = stl::CSet;
    Set S("00101"), T("01010");
    lemon::ListDigraph new_digraph;
    ArcMap new_cap(new_digraph);
    pmfR.contract(S, T, digraph, cap, new_digraph, new_cap);
    lemon::ListDigraph::Node source_node = digraph.nodeFromId(4);
    lemon::ListDigraph::Node sink_node = digraph.nodeFromId(3);
    PMF_R::Preflow pf(digraph, cap, source_node, sink_node);
    pf.run();
    PMF_R::FlowMap newFlowMap(digraph);
    pmfR.modify_flow(S, T, new_digraph, digraph, pf.flowMap(), newFlowMap);
    std::map<int, std::map<int, double>> flowMap;
    for (lemon::ListDigraph::ArcIt a(new_digraph); a != lemon::INVALID; ++a) {
        int u_id = new_digraph.id(new_digraph.source(a));
        int v_id = new_digraph.id(new_digraph.target(a));
        flowMap[u_id][v_id] = newFlowMap[a];
    }
    EXPECT_DOUBLE_EQ(flowMap[4][0], 1);
    EXPECT_DOUBLE_EQ(flowMap[4][3], 2);
    EXPECT_DOUBLE_EQ(flowMap[0][3], 1);
    EXPECT_DOUBLE_EQ(flowMap[3][4], 0);
}

TEST(PMF_R, PMClass) {
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
    PMF_R pmf(&g, &aM, 2, parameter_list);
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

TEST(PDT_R, RUN) {
    std::vector<std::tuple<std::size_t, std::size_t, double>> elt;
    elt.push_back(std::make_tuple(0, 1, 1));
    elt.push_back(std::make_tuple(0, 2, 5));
    elt.push_back(std::make_tuple(1, 2, 1));
    psp::PSP ic(elt, 3);
    ic.run("pdt_r");
    std::vector<double> lambda_list = ic.get_critical_values();
    std::vector<Partition> partition_list = ic.get_partitions();
    EXPECT_EQ(lambda_list.size(), 2);
    EXPECT_EQ(partition_list.size(), 3);
    std::vector<double>::iterator d = lambda_list.begin();
    EXPECT_DOUBLE_EQ(*d, 2);
    d++;
    EXPECT_DOUBLE_EQ(*d, 5);
    std::vector<Partition>::iterator p = partition_list.begin();
    EXPECT_EQ(*p, Partition::makeDense(3));
    p++;
    Partition nP;
    nP.AddElement(stl::CSet("101"));
    nP.AddElement(stl::CSet("010"));
    EXPECT_EQ(*p, nP);
    p++;
    EXPECT_EQ(*p, Partition::makeFine(3));
}

TEST(PMF_R, construct_new_base) {
    lemon::ListDigraph digraph;
    ArcMap cap(digraph);
    std::string lgf_str = "@nodes\nlabel\n0\n1\n2\n3\n@arcs\n\t\tlabel\tcapacity\n0\t1\t0\t2\n0\t2\t1\t3\n0\t3\t2\t5\n1\t3\t3\t4\n1\t2\t4\t1\n2\t3\t5\t7";
    std::stringstream ss;
    ss << lgf_str;
    lemon::digraphReader(digraph, ss)
        .arcMap("capacity", cap)
        .run();
    std::vector<pair> y;
    PMF_R pmfR(&digraph, &cap, 3, y);
    pmfR.set_source_node_id(4);
    pmfR.set_sink_node_id(3);
    lemon::ListDigraph g;
    for (int i = 0; i < 5; i++)
        g.addNode();
    stl::CSet S("10001");
    stl::CSet T("0001");
    g.erase(g.nodeFromId(0));
    g.erase(g.nodeFromId(3));
    std::map<int, pair> new_update_base;
    pmfR.construct_new_update_base(g, S, T, new_update_base);
    ASSERT_DOUBLE_EQ(new_update_base[1].first, 2);
    ASSERT_DOUBLE_EQ(new_update_base[1].second, 4);
    ASSERT_DOUBLE_EQ(new_update_base[2].first, 3);
    ASSERT_DOUBLE_EQ(new_update_base[2].second, 7);
}
}
namespace demo {
TEST_F(Graph4PointTest, PDT_R) {
    psp::PSP ic(edge_list_tuple_1, 4);
    ic.run("pdt_r");
    std::vector<double> lambda_list = ic.get_critical_values();
    std::vector<parametric::Partition> partition_list = ic.get_partitions();
    EXPECT_EQ(lambda_list.size(), 1);
    std::vector<double>::iterator lambda_it = lambda_list.begin();
    EXPECT_DOUBLE_EQ(*lambda_it, 1 + 2 / 3.0);

    EXPECT_EQ(partition_list.size(), 2);
    std::vector<parametric::Partition>::iterator partition_it = partition_list.begin();

    EXPECT_EQ(*partition_it, parametric::Partition::makeDense(4));
    partition_it++;
    EXPECT_EQ(*partition_it, parametric::Partition::makeFine(4));

}

TEST(GivenPoint8, PDT_R) {
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
    ic.run("pdt_r");
    std::vector<double> lambda_list = ic.get_critical_values();
    std::vector<parametric::Partition> partition_list = ic.get_partitions();


    ic.run();
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

TEST(ThreePointComplete, PDT_R) {
    std::vector<std::tuple<std::size_t, std::size_t, double>> edges;
    edges.push_back(std::make_tuple(0, 1, 1.0));
    edges.push_back(std::make_tuple(0, 2, 1.0));
    edges.push_back(std::make_tuple(1, 2, 5.0));
    psp::PSP ic(edges, 3);
    ic.run("pdt_r");
    std::vector<double> lambda_list = ic.get_critical_values();
    std::vector<parametric::Partition> partition_list = ic.get_partitions();
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

TEST(ThreePointNotComplete, PDT_R) {
    std::vector<std::tuple<std::size_t, std::size_t, double>> edges;
    edges.push_back(std::make_tuple(0, 1, 1.0));
    psp::PSP ic(edges, 3);
    ic.run("pdt_r");
    std::vector<double> lambda_list = ic.get_critical_values();
    std::vector<parametric::Partition> partition_list = ic.get_partitions();
    std::vector<double>::iterator it = lambda_list.begin();
    EXPECT_DOUBLE_EQ(*it, 0);
    it++;
    EXPECT_DOUBLE_EQ(*it, 1);

    EXPECT_EQ(lambda_list.size(), 2);
    EXPECT_EQ(partition_list.size(), 3);

    stl::Partition p = stl::Partition::makeDense(3);

    std::vector<parametric::Partition>::iterator it_p = partition_list.begin();
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

TEST(FourPointNotComplete, PDT_R) {
    std::vector<std::tuple<std::size_t, std::size_t, double>> edges;
    edges.push_back(std::make_tuple(0, 1, 1.0));
    edges.push_back(std::make_tuple(2, 3, 1.0));
    psp::PSP ic(edges, 4);
    ic.run("pdt_r");
    std::vector<double> lambda_list = ic.get_critical_values();
    std::vector<parametric::Partition> partition_list = ic.get_partitions();
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
}