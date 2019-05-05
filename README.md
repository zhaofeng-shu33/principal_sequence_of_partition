[![Windows](https://ci.appveyor.com/api/projects/status/github/zhaofeng-shu33/principal_sequence_of_partition?branch=master&svg=true)](https://ci.appveyor.com/project/zhaofeng-shu33/principal-sequence-of-partition)
[![Build Status](https://api.travis-ci.com/zhaofeng-shu33/principal_sequence_of_partition.svg?branch=master)](https://travis-ci.com/zhaofeng-shu33/principal_sequence_of_partition/)
# Introduction
This repo contains code to compute the principal sequence of partition for Dilworth truncation function.
Currently, two methods are available:

1. Dilworth truncation based on graph maximal flow
2. using paramatric maximal flow

Both method relies on Lemon Library to compute maximal flow for graph.

Agglomerative clustering method is implemented but cannot be used in general case because of FWRobust method is cursed with float accuracy.

# How to build
`main.cpp` is the user-implemented part. Run `cmake` will generate build recipe for building this cpp file.

# CMake with GTest
Testing is disabled by default, which requires gtest library. To enable it, run `cmake` with `-DENABLE_TESTING=ON`

# Lemon library
Use brute force search to solve submodualr function minimization(SFM) problem. For set with more than 10 elements, it is impractical. We use graph maximal flow(MF) to solve the special SFM problem. MF requires [lemon](https://lemon.cs.elte.hu/trac/lemon) library, which is disabled by default. To enable it, run `cmake` with `-DUSE_LEMON=ON`. 

# boost library
This project uses boost library in two places. Firstly, the `main.cpp` uses *boost-option* to parse command-line arguments. But this feature is optional. To use it, run `cmake` with `-DUSE_BOOST_OPTION=ON`. Secondly, *boost-python* is used to make the procedure callable from Python.

# Python binding
[![PyPI](https://img.shields.io/pypi/v/info_cluster.svg)](https://pypi.org/project/info_cluster)
Disabled by default. The binding requires boost-python library. To enable it, run `cmake` with `-DUSE_PYTHON=ON`
To make it independent of boost dynamic library, static linking should be enabled in CMAKE configuration.
To package the library, use `python setup.py bdist_wheel`.
Install the package by `pip install info_cluster`, below is the prebuild
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
g.run(False) # use maximal flow algorithm to classify them
print(g.get_critical_values()) # [2,5]
print(g.get_category(2)) # get the result which has at least 2 categories, which is [1,0,1]
```    
## Further experiment
In the directory utility, we make two simple experiments. The first is `plot_art.py`, 
which plots the clustering results for two artificial datasets.
The second one is `empirical_compare.py`, which tests the info-clustering algorithm on 5 datasets
and compare the results with *kmeans*, *affinity propagation*, *spectral clustering* and *agglomerative clustering*.
For more detail, see [experiment](utility/README.md).

## Parametric Dilworth Truncation(pdt) implementation
We provide another alternative implementation, which can be used similar to **PyGraph**.
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

# ChangeLog
* Version 1.1: expose `Gaussian2DGraph` (C++) class, which can be used directly in python.
* Version 1.2: expose `PyGraph` (C++) class, which is high customizable in python.