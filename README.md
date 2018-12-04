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

```Python
import psp
g = psp.Gaussian2DGraph(16) # generate 16 2D points
g.run(False) # use maximal flow algorithm to classify them
cat = g.get_category(4) # get the result which has at least 4 categories
print(cat) # very likely [0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3]
```    

# Reference
1. [2016]Info-Clustering: A Mathematical Theory for Data Clustering
1. [https://github.com/ktrmnm/SFM](https://github.com/ktrmnm/SFM)

# ChangeLog
* Version 1.1: expose `Gaussian2DGraph` (C++) class, which can be used directly in python.
* Version 1.2: expose `PyGraph` (C++) class, which is high customizable in python.