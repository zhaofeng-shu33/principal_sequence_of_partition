#include <boost/python.hpp>
#include "config.h"
#include "utility/gaussian2Dcase.h"
using namespace boost::python;
BOOST_PYTHON_MODULE(psp)
{
    scope().attr("__version__") = PSP_VERSION_MAJOR;
    class_<demo::Gaussian2DGraph>("Gaussian2DGraph", init<float>())
        .def("run", &demo::Gaussian2DGraph::run)
        .def("get_gamma_list", &demo::Gaussian2DGraph::get_gamma_list);
}