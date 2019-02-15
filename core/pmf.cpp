#include <tuple>
#include <numeric>
#include <lemon/list_graph.h>
#include "core/parametric_preflow.h"
#include <iostream>
#include "core/pmf.h"

double compute_lambda(const std::vector<pair>& parameter_list, const double target_value) {
    // get all breakpoints from parameter_list and sort them from smallest to largest
    std::vector<double> turning_points;
    for (const pair& p : parameter_list) {
        turning_points.push_back((p.first - p.second) / 2);
    }
    std::sort(turning_points.begin(), turning_points.end());
    // compute values at breakpoints
    double sum = std::accumulate(parameter_list.begin(), parameter_list.end(), 0, 
        [](double a, pair b) {return (a + b.second); }
    );
    if (target_value > sum)
        return -1;
    double slope = 0;
    double last_tp = turning_points[0];
    if (target_value == sum)
        return last_tp;
    for (double& tp : turning_points) {
        sum += slope * (tp - last_tp);
        if (sum <= target_value) {
            return (target_value - sum) / slope + tp;
        }
        slope -= 2;
        last_tp = tp;
    }
    return (target_value - sum) / slope + last_tp;
}
int remain(){
    // set elevator and flow maps before running the algorithm.
    lemon::ListDigraph g;
    std::vector<lemon::ListDigraph::Node> node_list;
    int graph_size = 4;
    //sink node id is graph_size, source node id is graph_size+1
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
    lemon::Preflow<lemon::ListDigraph, ArcMap>::FlowMap flowMap(g);
    flowMap[arc1] = 2;
    flowMap[arc2] = 1;
    flowMap[arc3] = 4;
    flowMap[arc4] = 4;
    flowMap[arc5] = 1;
    pf.flowMap(flowMap);

    pf.init();
    pf.startFirstPhase();
    pf.startSecondPhase();
    double minimum_value = pf.flowValue();
    std::cout << minimum_value << std::endl;
    std::cout << flowMap[arc1] << std::endl;
    std::cout << flowMap[arc2] << std::endl;
    std::cout << flowMap[arc3] << std::endl;
    std::cout << flowMap[arc4] << std::endl;
    std::cout << flowMap[arc5] << std::endl;

    edge_cost_map[arc4] -= 1;
    flowMap[arc4] -= 1;
    pf.updateExcess();
    pf.startFirstPhase();
    pf.startSecondPhase();
    std::cout << "Second min value: " << pf.flowValue() << std::endl;
    return 0;
}
