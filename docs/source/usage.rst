How to use our Library
======================

After installation, in your C++ code, you use our library in the following way:

.. code-block:: C++
   :linenos:
   
   #include <psp/psp.h>
   int main() {
     std::vector<std::tuple<std::size_t, std::size_t, double>> arc_list;
     // construct the example graph
     // ...
     // construct the algorithm class instance
     psp::PSP psp_instance(arc_list, 3);
     psp_instance.run();
     std::vector<double> critical_values = psp_instance.get_critical_values();
     std::vector<stl::Partition> partition_list = psp_instance.get_partitions();
     // precess the results
     // ...
   }

First you include our header file, which provides our algorithm class ``PSP`` and data structure ``Partition``.
Then you prepare the input data, which requires a ``std::vector`` whose elements are ``std::tuple``.
For each tuple :math:`(i,j,w)`, it represents the first node index, the second node index and the edge weight
between the two nodes. The second parameter to construct ``PSP`` is the number of nodes in your graph.
Next you run the algorithm by invoking the public method ``run`` of ``PSP``. Finally you get the critical values
and partitions results.