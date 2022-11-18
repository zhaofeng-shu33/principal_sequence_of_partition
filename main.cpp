/**
* \author : zhaofeng-shu33
* \brief : accomplish real task by info-clustering technique
*/
#include <string>
#include <fstream>
#include <chrono>
#include <sstream>
#include <boost/program_options.hpp>
#include <lemon/lgf_reader.h>
#include "psp/psp.h"

int main(int argc, const char *argv[]){
    boost::program_options::options_description desc;
	desc.add_options()
		("help,h", "Show this help screen")
		("graph", boost::program_options::value<std::string>(), "input graph file, currently only lgf format is supported")
		("method", boost::program_options::value<std::string>()->default_value("dt"), "method to use, should be within dt, pdt, psp_i, pdt_r, agg_psp")
		("result", boost::program_options::value<std::string>()->default_value("output.txt"), "result file")
		("time", boost::program_options::value<bool>()->default_value(false), "report time used");

    boost::program_options::variables_map vm;
    boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
    boost::program_options::notify(vm);
    if (vm.count("help")) {
        std::cout << desc << '\n';
        return 0;
    }
	std::string method = vm["method"].as<std::string>();
	if (!(method == "dt" || method == "psp_i" || method == "pdt" || method == "pdt_r" || method == "agg_psp")) {
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
	bool report_time = vm["time"].as<bool>();
	if (result_filename == "output.txt") {
		result_filename = method + "-" + graph_filename + ".txt";
	}
	std::ifstream fin(graph_filename);

	lemon::digraphReader(digraph, fin)
		.arcMap("capacity", cap).run();

	std::stringstream result;
	std::vector<double> critical_values;
	std::vector<stl::Partition> partition_list;
	std::chrono::system_clock::time_point start_time;
	if(report_time)
		start_time = std::chrono::system_clock::now();

    psp::PSP psp_instance(&digraph, &cap);
    psp_instance.run(method);
	critical_values = psp_instance.get_critical_values();
	partition_list = psp_instance.get_partitions();
	
	if (report_time) {
		std::chrono::system_clock::duration dtn;
		std::chrono::system_clock::time_point end_time = std::chrono::system_clock::now();
		dtn = end_time - start_time;
		float time_used = std::chrono::duration_cast<std::chrono::milliseconds>(dtn).count() / 1000.0;
		std::cout << "Time used : " << time_used << std::endl;
	}
	std::vector<stl::Partition>::iterator it_2 = partition_list.begin();
	result << *it_2 << std::endl;
	for (std::vector<double>::iterator it_1 = critical_values.begin(); it_1 != critical_values.end(); it_1++) {
		it_2++;
		result << *it_1 << std::endl;
		result << *it_2 << std::endl;
	}
	std::ofstream fout(result_filename);
	fout << result.str();
	fout.close();
    return 0;
}
