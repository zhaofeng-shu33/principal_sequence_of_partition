#include <lemon/list_graph.h>
#include "psp/hpsp.h"
#include "psp/graph.h"
typedef lemon::ListDigraph Digraph;
typedef Digraph::ArcMap<double> ArcMap;

TEST(HPSP, SIMPLE_FOUR_POINT) {
    Digraph* g = new Digraph();
    ArcMap* edge_map = new ArcMap(*g);
    std::vector<std::tuple<std::size_t, std::size_t, double>> elt;
    elt.push_back(std::make_tuple(0, 1, 1));
    elt.push_back(std::make_tuple(0, 2, 0.4));
    elt.push_back(std::make_tuple(1, 3, 0.5));
    elt.push_back(std::make_tuple(2, 3, 2));
    submodular::make_dgraph(4, elt, *g, *edge_map);
    psp::HPSP psp_class(g, edge_map);
    psp_class.run();
    std::vector<int> K = psp_class.get_K();
    std::vector<double> W = psp_class.get_W();
    std::vector<int> _K({0, 0, 0, 2});
    std::vector<double> _W({0.9, 1, 0.9, 2});
    for(int i = 0; i < 4; i++) {
        EXPECT_EQ(K[i], _K[i]);
        EXPECT_FLOAT_EQ(W[i], _W[i]);
    }
    std::list<double> cv = psp_class.get_critical_values();
    EXPECT_EQ(cv.size(), 3);
    std::list<double>::iterator it = cv.begin();
    EXPECT_DOUBLE_EQ(*it, 0.9);
    it++;
    EXPECT_DOUBLE_EQ(*it, 1);
    it++;
    EXPECT_DOUBLE_EQ(*it, 2);
    std::list<stl::Partition> psp_list = psp_class.get_psp();
    EXPECT_EQ(psp_list.size(), 4);
    std::list<stl::Partition>::iterator it_2 = psp_list.begin();
    EXPECT_EQ(*it_2, stl::Partition::makeDense(4));
    stl::Partition p2;
    p2.AddElement(stl::CSet(std::string("11")));
    p2.AddElement(stl::CSet(std::string("0011")));
    it_2++;
    EXPECT_EQ(*it_2, p2);
    stl::Partition p3;
    p3.AddElement(stl::CSet(std::string("1")));
    p3.AddElement(stl::CSet(std::string("01")));
    p3.AddElement(stl::CSet(std::string("0011")));
    it_2++;
    EXPECT_EQ(*it_2, p3);
    it_2++;
    EXPECT_EQ(*it_2, stl::Partition::makeFine(4));      
    delete edge_map;
    delete g;
}

TEST(HPSP, NESTED_FOUR_POINT) {
    Digraph* g = new Digraph();
    ArcMap* edge_map = new ArcMap(*g);
    std::vector<std::tuple<std::size_t, std::size_t, double>> elt;
    elt.push_back(std::make_tuple(0, 1, 5));
    elt.push_back(std::make_tuple(0, 2, 2));
    elt.push_back(std::make_tuple(0, 3, 1));
    elt.push_back(std::make_tuple(1, 2, 2));
    elt.push_back(std::make_tuple(1, 3, 1));
    submodular::make_dgraph(4, elt, *g, *edge_map);
    psp::HPSP psp_class(g, edge_map);
    psp_class.run();
    std::vector<int> K = psp_class.get_K();
    std::vector<double> W = psp_class.get_W();
    std::vector<int> _K({0, 0, 0, 0});
    std::vector<double> _W({2, 5, 4, 2});
    for(int i = 0; i < 4; i++) {
        EXPECT_EQ(K[i], _K[i]);
        EXPECT_FLOAT_EQ(W[i], _W[i]);
    }
    std::list<double> cv = psp_class.get_critical_values();
    EXPECT_EQ(cv.size(), 3);
    std::list<double>::iterator it = cv.begin();
    EXPECT_DOUBLE_EQ(*it, 2);
    it++;
    EXPECT_DOUBLE_EQ(*it, 4);
    it++;
    EXPECT_DOUBLE_EQ(*it, 5);
    std::list<stl::Partition> psp_list = psp_class.get_psp();
    EXPECT_EQ(psp_list.size(), 4);
    std::list<stl::Partition>::iterator it_2 = psp_list.begin();
    EXPECT_EQ(*it_2, stl::Partition::makeDense(4));
    stl::Partition p2;
    p2.AddElement(stl::CSet(std::string("111")));
    p2.AddElement(stl::CSet(std::string("0001")));
    it_2++;
    EXPECT_EQ(*it_2, p2);
    stl::Partition p3;
    p3.AddElement(stl::CSet(std::string("11")));
    p3.AddElement(stl::CSet(std::string("001")));
    p3.AddElement(stl::CSet(std::string("0001")));
    it_2++;
    EXPECT_EQ(*it_2, p3);
    it_2++;
    EXPECT_EQ(*it_2, stl::Partition::makeFine(4));    
    delete edge_map;
    delete g;
}