#include "config.h"
#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include "utility/gaussian2Dcase.h"
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

    class_<GaussianGraph>("Gaussian2DGraph", init<int>())
        .def("run", &GaussianGraph::run)
        .def("get_gamma_list", &GaussianGraph::get_gamma_list, return_internal_reference<>())
        .def("get_x_pos_list", &GaussianGraph::get_x_pos_list, return_internal_reference<>())
        .def("get_y_pos_list", &GaussianGraph::get_y_pos_list, return_internal_reference<>())
        .def("get_category", &GaussianGraph::get_category);
}