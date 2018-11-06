[![Windows](https://ci.appveyor.com/api/projects/status/github/zhaofeng-shu33/principal_sequence_of_partition?branch=master&svg=true)](https://ci.appveyor.com/project/zhaofeng-shu33/principal-sequence-of-partition)
# Introduction
This repo contains code to compute the principal sequence of partition for Dilworth truncation function.

# How to build
`main2.cpp` is the user-implemented part.

# CMake with GTest
Testing is enabled by default, which requires gtest library. To disable it, run `cmake` with `-DENABLE_TESTING=OFF`

# Reference
1. [2016]Info-Clustering: A Mathematical Theory for Data Clustering
1. [https://github.com/ktrmnm/SFM](https://github.com/ktrmnm/SFM)
