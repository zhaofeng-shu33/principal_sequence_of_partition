#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include "config.h"
#include "utility/gaussian2Dcase.h"
using namespace boost::python;
#define GaussianGraph demo::Gaussian2DGraph<double>
typedef std::vector<double> cList;
BOOST_PYTHON_MODULE(psp)
{
    scope().attr("__version__") = PSP_VERSION_MAJOR;
    class_<cList>("cList").def(vector_indexing_suite<cList>());
    class_<GaussianGraph>("Gaussian2DGraph", init<int>())
        .def("run", &GaussianGraph::run)
        .def("get_gamma_list", &GaussianGraph::get_gamma_list, return_internal_reference<>())
        .def("get_x_pos_list", &GaussianGraph::get_x_pos_list, return_internal_reference<>())
        .def("get_y_pos_list", &GaussianGraph::get_y_pos_list, return_internal_reference<>());

}