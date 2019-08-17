[![Windows](https://ci.appveyor.com/api/projects/status/github/zhaofeng-shu33/principal_sequence_of_partition?branch=master&svg=true)](https://ci.appveyor.com/project/zhaofeng-shu33/principal-sequence-of-partition)
[![Build Status](https://api.travis-ci.com/zhaofeng-shu33/principal_sequence_of_partition.svg?branch=master)](https://travis-ci.com/zhaofeng-shu33/principal_sequence_of_partition/)
[![codecov](https://codecov.io/gh/zhaofeng-shu33/principal_sequence_of_partition/branch/master/graph/badge.svg)](https://codecov.io/gh/zhaofeng-shu33/principal_sequence_of_partition)

# Introduction
This repository contains code to compute the principal sequence of partition for Dilworth truncation function.
Currently, four methods are available:

1. Traditional Canonical Method based on Dilworth truncation and Graph Maximal Flow
2. Parametric computing scheme combined with Parametric Maximal Flow
3. Graph Contraction combined with rapid jump (under development)
  Both method relies on [LEMON](https://lemon.cs.elte.hu/trac/lemon) Library to compute maximal flow for graph.
4. Parametric Dilworth truncation used in paper 2010.

# How to build

## Dependencies

* LEMON (required)
* boost-program-options is required to build the executable program `main`
* googletest (optional, used in unit-test)


You need to enable C++11 to compile the code.

### Dependency Install Guide
* If you use CentOS/Fedora based system, you can install the pre-built binary package `liblemon` from [copr](https://copr.fedorainfracloud.org/coprs/freewind201301/liblemon/).
* If you use Ubuntu(<18.04), you can install the pre-built binary package `liblemon` from [launchpad](https://launchpad.net/~zhaofeng-shu33/+archive/ubuntu/liblemon); `liblemon` library is included in Ubuntu from 18.04, see [liblemon-dev](https://packages.ubuntu.com/bionic/liblemon-dev).
* On Windows platform, [vcpkg](https://github.com/microsoft/vcpkg) is required to fix the dependencies.
* On MacOS platform, [brew](https://brew.sh) is recommended to fix the dependencies.

## CMake
This project uses [CMake](https://cmake.org) build system. We recommand out of source build. From project root directory,
```shell
mkdir build
cd build
cmake ..
```
If any error occurs, you should fix the dependencies first.



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
* Version 1.2: implementation of `PSP` (C++) class, which uses traditional Dilworth truncation.
* Version 1.3: implementation of `PDT` (C++) class, which has similar API as `PSP` but different inner implementation.
* Version 1.4: implement `psp_i` method.
* Version 1.5: implement `pdt_r` method. 

