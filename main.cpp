/**
* \author : zhaofeng-shu33
* \brief : accomplish real task by info-clustering technique
*/
#include "config.h"
#include <string>
#include <fstream>
#include <sstream>
#include <boost/program_options.hpp>
#include <lemon/lgf_reader.h>
#include "core/dt.h"
#include "core/pmf.h"
#include "core/psp_i.h"

int main(int argc, const char *argv[]){
    boost::program_options::options_description desc;
	desc.add_options()
		("help,h", "Show this help screen")
		("graph", boost::program_options::value<std::string>(), "input graph file, currently only lgf format is supported")
		("result", boost::program_options::value<std::string>(), "result file")
		("pdt", boost::program_options::value<bool>()->default_value(true), "whether to use parametric Dilworth truncation")
		("psp_i", boost::program_options::value<bool>()->default_value(false), "whether to use improved principal sequence of partition");

    boost::program_options::variables_map vm;
    boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
    boost::program_options::notify(vm);
    if (vm.count("help")) {
        std::cout << desc << '\n';
        return 0;
    }

	typedef lemon::ListDigraph Digraph;
	typedef double T;
	typedef Digraph::ArcMap<T> ArcMap;
	typedef Digraph::Node Node;
	typedef Digraph::NodeIt NodeIt;

	Digraph digraph;
	ArcMap cap(digraph);

    bool use_pdt = vm["pdt"].as<bool>();
	bool use_psp_i = vm["psp_i"].as<bool>();
    std::string graph_filename = vm["graph"].as<std::string>();
	std::string result_filename = vm["result"].as<std::string>();
	std::ifstream fin(graph_filename);

	lemon::digraphReader(digraph, fin)
		.arcMap("capacity", cap).run();

	std::stringstream result;
	if (use_psp_i) {
		std::list<double> critical_values;
		std::list<stl::Partition> partition_list;
		psp::PSP psp_i(&digraph, &cap);
		psp_i.run();
		critical_values = psp_i.get_critical_values();
		partition_list = psp_i.get_psp();
		std::list<stl::Partition>::iterator it_2 = partition_list.begin();
		result << *it_2 << std::endl;
		for (std::list<double>::iterator it_1 = critical_values.begin(); it_1 != critical_values.end(); it_1++) {
			it_2++;
			result << *it_1 << std::endl;
			result << *it_2 << std::endl;
		}
	}
    else if (use_pdt) {
		std::list<double> critical_values;
		std::list<stl::Partition> partition_list;
		parametric::PDT pmf(digraph, cap);
		pmf.run();
		critical_values = pmf.get_lambda_list();
		partition_list = pmf.get_partition_list();
		std::list<stl::Partition>::iterator it_2 = partition_list.begin();
		result << *it_2 << std::endl;
		for (std::list<double>::iterator it_1 = critical_values.begin(); *it_1 != INFINITY; it_1++) {
			it_2++;
			result << *it_1 << std::endl;
			result << *it_2 << std::endl;
		}
    }
	else{
		std::vector<double> critical_values;
		std::vector<stl::Partition> partition_list;
		submodular::PSP psp_class(&digraph, &cap);
		psp_class.run();
		critical_values = psp_class.Get_critical_values();
		partition_list = psp_class.Get_psp();
		for (int i = 0; i < partition_list.size()-1; i++) {
			if (partition_list[i].Cardinality() > 0) {
				result << partition_list[i] << std::endl;
				result << critical_values[i] << std::endl;
			}
		}
		result << partition_list[partition_list.size() - 1] << std::endl;
	}
	std::ofstream fout(result_filename);
	fout << result.str();
	fout.close();
    return 0;
}
