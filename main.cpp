/**
* \author : zhaofeng-shu33
* \brief : accomplish real task by info-clustering technique
*/
#include "config.h"

#if USE_BOOST
#include <boost/program_options.hpp>
#endif
#include "utility/gaussian2Dcase.h"
void check_size(int size) {
    if (size % 4 != 0) {
        std::cout << "Node size must be multiplier of 4\n";
        exit(0);
    }
}
//! To be finished
int main(int argc, const char *argv[]){
#if USE_BOOST
    boost::program_options::options_description desc;
    desc.add_options()
        ("help,h", "Show this help screen")
        ("full", boost::program_options::value<bool>()->default_value(true))
        ("size", boost::program_options::value<int>()->default_value(8)->notifier(check_size), "total number of points to be classified, must be multiplier of 4");
    boost::program_options::variables_map vm;
    boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
    boost::program_options::notify(vm);
    if (vm.count("help")) {
        std::cout << desc << '\n';
        return 0;
    }
    submodular::InfoCluster* g2g = new demo::Gaussian2DGraph(vm["size"].as<int>(),0.5);
#else
    demo::Gaussian2DGraph<double>* g2g = new demo::Gaussian2DGraph<double>(8);
#endif
    g2g->run();
    std::vector<submodular::Set> p = g2g->get_smallest_partition(4);
    std::cout << p;
    delete g2g;
    return 0;
}