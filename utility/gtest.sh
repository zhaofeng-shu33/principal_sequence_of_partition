#!/bin/bash
# build and install gtest
pushd /usr/src/gtest
cmake .
make install
popd
