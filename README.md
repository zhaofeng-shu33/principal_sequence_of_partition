[![Windows](https://ci.appveyor.com/api/projects/status/github/zhaofeng-shu33/principal_sequence_of_partition?branch=master&svg=true)](https://ci.appveyor.com/project/zhaofeng-shu33/principal-sequence-of-partition)
[![Build Status](https://api.travis-ci.com/zhaofeng-shu33/principal_sequence_of_partition.svg?branch=master)](https://travis-ci.com/zhaofeng-shu33/principal_sequence_of_partition/)
# Introduction
This repository contains code to compute the principal sequence of partition for Dilworth truncation function.
Currently, three methods are available:

1. Traditional Canonical Method based on Dilworth truncation and Graph Maximal Flow
2. Parametric computing scheme combined with Parametric Maximal Flow
3. Graph Contraction combined with rapid jump (under development)
Both method relies on [LEMON](https://lemon.cs.elte.hu/trac/lemon) Library to compute maximal flow for graph.

# How to build

## Dependencies

* LEMON (required)
* boost-program-options is required to build the executable program 
* googletest (optional, used in unit-test)


You need to enable C++11 to compile the code.

## CMake
This project uses [CMake](https://cmake.org) build system. We recommand out of source build. From project root directory,
```shell
mkdir build
cd build
cmake ..
```
If any error occurs, you should fix the dependencies first.
For example, `lemon` library is included in Ubuntu from 18.04, see [liblemon-dev](https://packages.ubuntu.com/bionic/liblemon-dev). On Windows platform, [vcpkg](https://github.com/microsoft/vcpkg) is required to fix the dependencies.


### options
use `-DEnable_Testing=ON` to compile the test binary (requiring `googletest`).


# Reference
1. [2016] Info-Clustering: A Mathematical Theory for Data Clustering
1. [https://github.com/ktrmnm/SFM](https://github.com/ktrmnm/SFM)
1. [2010] A Faster Algorithm for Computing the Principal Sequence of Partitions of a Graph
1. [2017] Info-Clustering: An Efficient Algorithm by Network Information Flow

# Contributing
See [contributing.md](./contributing.md)

# ChangeLog
* Version 1.1: expose `Gaussian2DGraph` (C++) class, which can be used directly in python.
* Version 1.2: expose `PSP` (C++) class, which is high customizable in python.
* Version 1.3: expose `PyGraphPDT` (C++) class, which has similar api as `PyGraph` but different inner implementation.
* Version 1.4: expose `psp_i` method.