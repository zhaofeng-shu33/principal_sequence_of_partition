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
* Version 1.4: expose psp_i method.