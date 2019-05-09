/**
* \author : zhaofeng-shu33
* \brief : accomplish real task by info-clustering technique
*/
#include "config.h"


#if USE_BOOST
#include <boost/program_options.hpp>
#endif
#include "core/gaussian2Dcase.h"
#include "core/pmf.h"
void check_size(int size) {
    if (size % 4 != 0) {
        std::cout << "Node size must be multiplier of 4\n";
        exit(0);
    }
}
void check_positive(double gamma) {
    if (gamma <= 0) {
        std::cout << "gamma must be positive number";
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
        ("pdt", boost::program_options::value<bool>()->default_value(false), "whether to use parametric Dilworth truncation")
        ("gamma", boost::program_options::value<double>()->default_value(0.5)->notifier(check_positive))
        ("size", boost::program_options::value<int>()->default_value(8)->notifier(check_size), "total number of points to be classified, must be multiplier of 4");
    boost::program_options::variables_map vm;
    boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
    boost::program_options::notify(vm);
    if (vm.count("help")) {
        std::cout << desc << '\n';
        return 0;
    }
#endif
    int size = vm["size"].as<int>();
    bool run_full = vm["full"].as<bool>();
    bool use_pdt = vm["pdt"].as<bool>();
    double _gamma = vm["gamma"].as<double>();
    if (use_pdt) {
        demo::Gaussian2DPDT* gPDT = new demo::Gaussian2DPDT(size, _gamma);
        gPDT->run();
        parametric::Partition p = gPDT->get_smallest_partition(4);
        std::cout << p;
        delete gPDT;
    }
    else {
#if USE_BOOST
        submodular::InfoCluster* g2g = new demo::Gaussian2DGraph(size, _gamma);
#else
        demo::Gaussian2DGraph<double>* g2g = new demo::Gaussian2DGraph<double>(8);
#endif
        stl::Partition p;
        if (run_full) {
            g2g->run();
            p = g2g->get_smallest_partition(4);
        }
        else {
            p = g2g->get_partition(4);
        }
        std::cout << p;
        delete g2g;
    }
    return 0;
}
