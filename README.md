[![Windows](https://ci.appveyor.com/api/projects/status/github/zhaofeng-shu33/principal_sequence_of_partition?branch=master&svg=true)](https://ci.appveyor.com/project/zhaofeng-shu33/principal-sequence-of-partition)
[![Build Status](https://api.travis-ci.com/zhaofeng-shu33/principal_sequence_of_partition.svg?branch=master)](https://travis-ci.com/zhaofeng-shu33/principal_sequence_of_partition/)
# Introduction
This repo contains code to compute the principal sequence of partition for Dilworth truncation function.
Currently, two methods are available:

1. Dilworth truncation based on graph maximal flow
2. using paramatric maximal flow

Both method relies on [LEMON](https://lemon.cs.elte.hu/trac/lemon) Library to compute maximal flow for graph.

# How to build

## Dependencies

* LEMON (required)
* boost 
	* static lib of info-clustering does not require boost library
	* boost-program-options is required to build the executable program 
	* boost-python is required to build the python-binding
* gtest (optional, used in unit-test)


It is tested you need g++ version >= 6.0 to compile the source code.

## CMake
This project uses [CMake](https://cmake.org) build system. We recommand out of source build. From project root directory,
```shell
mkdir build
cd build
cmake ..
```
If any error occurs, you should fix the dependencies first.

### options
use `-DUSE_CXX11_ABI=OFF` if your system boost library is built by g++ version <=4.8.

We provide a naive brute force search method to solve submodular function minimization(SFM) problem. For set with more than 10 elements, it is impractical. We use graph maximal flow(MF) to solve the special SFM problem. MF requires [lemon](https://lemon.cs.elte.hu/trac/lemon) library, which is enabled by default.

This library is included in Ubuntu from 18.04, see [liblemon-dev](https://packages.ubuntu.com/bionic/liblemon-dev).

### with boost
This project uses boost library in two places. Firstly, the `main.cpp` uses *boost-option* to parse command-line arguments. This feature is optional but highly recommanded, and enabled by default. To disable it, run `cmake` with `-DUSE_BOOST_OPTION=OFF`. Secondly, *boost-python* is used to make the procedure callable from Python.
Disabled by default, to use it invoke cmake with `-DUSE_PYTHON=ON`.

### with GTest
Testing is disabled by default, which requires gtest library. To enable it, run `cmake` with `-DENABLE_TESTING=ON`

## Windows
On Windows platform, [vcpkg](https://github.com/microsoft/vcpkg) is required to fix the dependencies.

# Python binding
[![PyPI](https://img.shields.io/pypi/v/info_cluster.svg)](https://pypi.org/project/info_cluster)
Disabled by default. The binding requires boost-python library. To enable it, run `cmake` with `-DUSE_PYTHON=ON`
To make it independent of boost dynamic library, static linking should be enabled in CMAKE configuration.
To package the library, use `python setup.py bdist_wheel`.
Install the package by `pip install --user info_cluster`. If your system cmake is called cmake3, you can use
CMAKE=cmake3 pip install --user info_cluster`.
Below is the prebuild
binary packages:

| Platform | py3.6 | py3.7 |
| -------- | :---: | :---: |
| Windows  |   T   |       |
| MacOS    |       |       |
| Linux    |       |       |

## Demo code
![](example.png)
We provide a high-level wrapper of info-clustering algorithm. 
After installing `info_cluster`, you can use it as follows:
```Python
from info_cluster import InfoCluster
import networkx as nx
g = nx.Graph() # undirected graph
g.add_edge(0, 1, weight=1)
g.add_edge(1, 2, weight=1)
g.add_edge(0, 2, weight=5)
ic = InfoCluster(affinity='precomputed') # use precomputed graph structure
ic.fit(g)
ic.print_hierachical_tree()
```
The output is like
```shell
      /-0
   /-|
--|   \-2
  |
   \-1
```
```Python
import psp # classify the three data points shown in the above figure
g = psp.PyGraph(3, [(0,1,1),(1,2,1),(0,2,5)]) # index started from zero, similarity is 5 for vertex 0 and 2
g.run() # use maximal flow algorithm to classify them
print(g.get_critical_values()) # [2,5]
print(g.get_category(2)) # get the result which has at least 2 categories, which is [1,0,1]
```    

## Parametric Dilworth Truncation(pdt) implementation
We provide another alternative implementation, which can be used similar to **PyGraph**.
You shold apply a patch [preflow.patch](./preflow.patch) to `preflow.h`, which belongs to lemon library 1.3.1, see
[#625](https://lemon.cs.elte.hu/trac/lemon/ticket/625).

```Python
import psp
g = psp.PyGraphPDT(3, [(0,1,1),(1,2,1),(0,2,5)]) # index started from zero, similarity is 5 for vertex 0 and 2
g.run() # use maximal flow algorithm to classify them
print(g.get_critical_values()) # [2,5]
print(g.get_category(2)) # get the result which has at least 2 categories, which is [0,1,0]
```  

# Reference
1. [2016] Info-Clustering: A Mathematical Theory for Data Clustering
1. [https://github.com/ktrmnm/SFM](https://github.com/ktrmnm/SFM)
1. [2010] A Faster Algorithm for Computing the Principal Sequence of Partitions of a Graph
1. [2017] Info-Clustering: An Efficient Algorithm by Network Information Flow

# Contributing
See [contributing.md](./contributing.md)

# ChangeLog
* Version 1.1: expose `Gaussian2DGraph` (C++) class, which can be used directly in python.
* Version 1.2: expose `PyGraph` (C++) class, which is high customizable in python.
* Version 1.3: expose `PyGraphPDT` (C++) class, which has similar api as `PyGraph` but different inner implementation.