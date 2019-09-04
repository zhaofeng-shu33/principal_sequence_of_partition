#include <iostream>
#include <list>
#include <lemon/list_graph.h>
#include <psp/psp.h>
#include <psp/set/set_stl.h>

int main() {
    // construct the example graph
    lemon::ListDigraph g;
    lemon::ListDigraph::ArcMap<double> a(g);
    typedef lemon::ListDigraph::Node Node;
    Node n1 = g.add_node();
    Node n2 = g.add_node();
    Node n3 = g.add_node();
    typedef lemon::ListDigraph::Arc Arc;
    Arc a1 = g.add_arc(n1, n2);
    a[a1] = 1;
    Arc a2 = g.add_arc(n1, n3);
    a[a2] = 5;
    Arc a3 = g.add_arc(n2, n3);
    a[a3] = 1;
    // construct the algorithm class instance
    psp::PSP psp_instance(g, a1);
    psp_instance.run();
    std::list<double> critical_values = psp_instance.get_critical_values();
    std::list<stl::Partiton> partition_list = psp_instance.get_partitions();
    // output the results to the console
    std::list<double>::iterator critical_values_iterator = critical_values.begin();
    std::list<stl::Partition> partition_list_iterator = psp_instance.begin();
    for (; critical_values_iterator != critical_values.end(); critical_values_iterator++) {
        std::cout << *partition_list_iterator << std::endl;
        std::cout << *critical_values_iterator << std::endl;
    }
    std::cout << *partition_list_iterator << std::endl;
}