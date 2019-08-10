/**
* \author : zhaofeng-shu33
* \brief : accomplish real task by info-clustering technique
*/
#include <string>
#include <fstream>
#include <sstream>
#include <boost/program_options.hpp>
#include <lemon/lgf_reader.h>
#include "core/dt.h"
#include "core/pmf.h"
#include "core/pmf_r.h"
#include "core/psp_i.h"

int main(int argc, const char *argv[]){
    boost::program_options::options_description desc;
	desc.add_options()
		("help,h", "Show this help screen")
		("graph", boost::program_options::value<std::string>(), "input graph file, currently only lgf format is supported")
		("method", boost::program_options::value<std::string>()->default_value("dt"), "method to use, should be within dt, pdt, psp_i, pdt_i")
		("result", boost::program_options::value<std::string>()->default_value("output.txt"), "result file");

    boost::program_options::variables_map vm;
    boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
    boost::program_options::notify(vm);
    if (vm.count("help")) {
        std::cout << desc << '\n';
        return 0;
    }
	std::string method = vm["method"].as<std::string>();
	if (!(method == "dt" || method == "psp_i" || method == "pdt" || method == "pdt_r")) {
		std::cout << "Unknown method, " + method << '\n';
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

    std::string graph_filename = vm["graph"].as<std::string>();
	std::string result_filename = vm["result"].as<std::string>();
	if (result_filename == "output.txt") {
		result_filename = method + "-" + graph_filename + ".txt";
	}
	std::ifstream fin(graph_filename);

	lemon::digraphReader(digraph, fin)
		.arcMap("capacity", cap).run();

	std::stringstream result;
	std::list<double> critical_values;
	std::list<stl::Partition> partition_list;

	if (method == "psp_i") {
		psp::PSP psp_i(&digraph, &cap);
		psp_i.run();
		critical_values = psp_i.get_critical_values();
		partition_list = psp_i.get_psp();
	}
    else if (method == "pdt") {
		parametric::PDT pmf(&digraph, &cap);
		pmf.run();
		critical_values = pmf.get_critical_values();
		partition_list = pmf.get_psp();
    }
	else if (method == "pdt_r") {
		parametric::PDT_R pmf(&digraph, &cap);
		pmf.run();
		critical_values = pmf.get_critical_values();
		partition_list = pmf.get_psp();
	}
	else{
		submodular::PSP psp_class(&digraph, &cap);
		psp_class.run();
		critical_values = psp_class.get_critical_values();
		partition_list = psp_class.get_psp();
	}
	std::list<stl::Partition>::iterator it_2 = partition_list.begin();
	result << *it_2 << std::endl;
	for (std::list<double>::iterator it_1 = critical_values.begin(); it_1 != critical_values.end(); it_1++) {
		it_2++;
		result << *it_1 << std::endl;
		result << *it_2 << std::endl;
	}
	std::ofstream fout(result_filename);
	fout << result.str();
	fout.close();
    return 0;
}
