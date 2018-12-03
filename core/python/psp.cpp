#include "config.h"
#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include "utility/gaussian2Dcase.h"
#include "core/python/graph_cut_py.h"
using namespace boost::python;
#define GaussianGraph demo::Gaussian2DGraph<double>
template class std::vector<double>;
typedef std::vector<double> dList;
typedef std::vector<int> iList;


BOOST_PYTHON_MODULE(psp)
{
    scope().attr("__version__") = PSP_VERSION_MAJOR;
    class_<dList>("dList").def(vector_indexing_suite<dList>())
        .def(self_ns::str(self_ns::self));
        
        
    class_<iList>("iList").def(vector_indexing_suite<iList>())
        .def(self_ns::str(self_ns::self));

    class_<GaussianGraph>("Gaussian2DGraph", init<int,double>())
        .def("run", &GaussianGraph::run)
        .def("get_critical_values", &GaussianGraph::get_critical_values)
        .def("get_x_pos_list", &GaussianGraph::get_x_pos_list, return_internal_reference<>())
        .def("get_y_pos_list", &GaussianGraph::get_y_pos_list, return_internal_reference<>())
        .def("get_partitions", &GaussianGraph::get_partitions)
        .def("get_category", &GaussianGraph::get_category);

    class_<submodular::PyGraph>("PyGraph", init<int,list, double>())
        .def("run", &submodular::PyGraph::run)
        .def("get_critical_values", &submodular::PyGraph::get_critical_values)
        .def("get_partitions", &submodular::PyGraph::get_partitions)
        .def("get_category", &submodular::PyGraph::get_category);
}