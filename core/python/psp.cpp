#include "config.h"
#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include "utility/gaussian2Dcase.h"
#include "core/python/graph_cut_py.h"
using namespace boost::python;
template class std::vector<double>;
typedef std::vector<double> dVector;
typedef std::list<double> dList;
typedef std::vector<int> iList;


BOOST_PYTHON_MODULE(psp)
{
    scope().attr("__version__") = PSP_VERSION_MAJOR;
    class_<dVector>("dVector").def(vector_indexing_suite<dVector>())
        .def(self_ns::str(self_ns::self));

    class_<dList>("dList").def("__len__", &dList::size)
        .def("clear", &dList::clear)       
        .def("__iter__", iterator<std::list<double>>());

    class_<iList>("iList").def(vector_indexing_suite<iList>())
        .def(self_ns::str(self_ns::self));

    class_<demo::Gaussian2DGraph>("Gaussian2DGraph", init<int,double>())
        .def("run", &submodular::InfoCluster::run)
        .def("get_critical_values", &submodular::InfoCluster::get_critical_values)
        .def("get_x_pos_list", &demo::Gaussian2DGraph::get_x_pos_list, return_internal_reference<>())
        .def("get_y_pos_list", &demo::Gaussian2DGraph::get_y_pos_list, return_internal_reference<>())
        .def("get_partitions", &submodular::InfoCluster::get_partitions)
        .def("get_category", &submodular::InfoCluster::get_category);

    class_<submodular::PyGraph>("PyGraph", init<int,list>())
        .def("run", &submodular::PyGraph::run)
        .def("get_labels", &submodular::PyGraph::get_labels)
        .def("get_critical_values", &submodular::PyGraph::get_critical_values)
        .def("get_partitions", &submodular::PyGraph::get_partitions)
        .def("get_category", &submodular::PyGraph::get_category);

    class_<parametric::PyGraphPDT>("PyGraphPDT", init<int, list>())
        .def("run", &parametric::PyGraphPDT::run);
}