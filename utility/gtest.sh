#!/bin/bash
# build and install gtest
pushd /usr/src/gtest
pwd
/usr/bin/cmake ./
make
# install by symobolic linking
sudo ln -s -t /usr/lib /usr/src/gtest/libgtest.a
sudo ln -s -t /usr/lib /usr/src/gtest/libgtest_main.a
popd
