#include <iostream>
#include <tuple>
#include <vector>
#include <list>
#include <psp/psp.h>

int main() {
    // construct the example graph
    std::vector<std::tuple<std::size_t, std::size_t, double>> arc_list;
    arc_list.push_back(std::make_tuple(0, 1, 1.0));
    arc_list.push_back(std::make_tuple(0, 2, 5.0));
    arc_list.push_back(std::make_tuple(1, 2, 1.0));
    // construct the algorithm class instance
    psp::PSP psp_instance(arc_list, 3);
    psp_instance.run();
    std::vector<double> critical_values = psp_instance.get_critical_values();
    std::vector<psp::Partition> partition_list = psp_instance.get_partitions();
    // output the results to the console
    std::vector<double>::iterator critical_values_iterator = critical_values.begin();
    std::vector<psp::Partition>::iterator partition_list_iterator = partition_list.begin();
    for (; critical_values_iterator != critical_values.end(); critical_values_iterator++) {
        std::cout << *partition_list_iterator << std::endl;
        std::cout << *critical_values_iterator << std::endl;
        partition_list_iterator++;
    }
    std::cout << *partition_list_iterator << std::endl;
}
