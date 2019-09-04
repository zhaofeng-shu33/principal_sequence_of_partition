## File Description
This directory contains core algorithm implementation in C++.

* dt.h : contains Dilworh truncation (Method 1)
* pmf.h : contains parametric maximal flow (Method 2)
* io_utility.h : print some vectors
* graph
    * info_cluster.h : wrapper of dt and pmf, can be used as C++ library
    * graph.h : lemon graph utility function
* algorithms
    * brute_force.h : use enumeration to solve the minimization of modified graph cut function
    * sfm_mf.h : use lemon maximal flow to solve the minimization of modified graph cut function
    * sfm_algorithm.h : algorithm base case
    * reporter.h : algorithm log class
* python
    * graph_cut_py.h : wrapper of info_cluster.h
    * psp.cpp : export PyGraph class to be used by python.

## General Notice
Some classes are template based and cannot be compiled to static library. Only files ended with `cpp` should be compiled. Header files
are included directly in other project.