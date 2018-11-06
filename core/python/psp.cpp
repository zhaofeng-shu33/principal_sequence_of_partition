#include <boost/python.hpp>
#include "config.h"
using namespace boost::python;
BOOST_PYTHON_MODULE(psp)
{
    scope().attr("__version__") = PSP_VERSION_MAJOR;

}