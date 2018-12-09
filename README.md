[![Windows](https://ci.appveyor.com/api/projects/status/github/zhaofeng-shu33/principal_sequence_of_partition?branch=master&svg=true)](https://ci.appveyor.com/project/zhaofeng-shu33/principal-sequence-of-partition)
[![Build Status](https://api.travis-ci.com/zhaofeng-shu33/principal_sequence_of_partition.svg?branch=master)](https://travis-ci.com/zhaofeng-shu33/principal_sequence_of_partition/)
# Introduction
This repo contains code to compute the principal sequence of partition for Dilworth truncation function.

# How to build
`main.cpp` is the user-implemented part.

# CMake with GTest
Testing is enabled by default, which requires gtest library. To disable it, run `cmake` with `-DENABLE_TESTING=OFF`

# Python binding
Disabled by default. The binding requires boost-python library. To enable it, run `cmake` with `-DENABLE_PYTHON=ON`

## Demo code
![](example.png)
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
and compare the results with *kmeans*, *affinity propagation* and *spectral clustering*.

# Reference
1. [2016]Info-Clustering: A Mathematical Theory for Data Clustering
1. [https://github.com/ktrmnm/SFM](https://github.com/ktrmnm/SFM)

# ChangeLog
* Version 1.1: expose `Gaussian2DGraph` (C++) class, which can be used directly in python.
* Version 1.2: expose `PyGraph` (C++) class, which is high customizable in python.