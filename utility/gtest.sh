#!/bin/bash
# build and install gtest
make
# install by symobolic linking
sudo ln -s -t /usr/lib /usr/src/gtest/libgtest.a
sudo ln -s -t /usr/lib /usr/src/gtest/libgtest_main.a

