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
    std::cout << 'K' << ' ' << 'W' << std::endl;
    for(int i = 0; i < 4; i++) {
        std::cout << K[i] << ' ' << W[i] << std::endl;
    }
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
    std::cout << 'K' << ' ' << 'W' << std::endl;
    for(int i = 0; i < 4; i++) {
        std::cout << K[i] << ' ' << W[i] << std::endl;
    }
    delete edge_map;
    delete g;
}