cmake_minimum_required(VERSION 3.5)
set(ic_lib set_utils.cpp io_utility.cpp reporter.cpp)

if(USE_EIGEN3)
list(APPEND ic_lib linalg.cpp base.cpp agglomerative.cpp)
endif()

add_library(ic_static ${ic_lib})
add_library(pmf pmf.cpp)

if(UNIX)
    SET_TARGET_PROPERTIES(ic_static PROPERTIES COMPILE_FLAGS "-fPIC")
    SET_TARGET_PROPERTIES(pmf PROPERTIES COMPILE_FLAGS "-fPIC")
endif()
