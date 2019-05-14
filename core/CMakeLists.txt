cmake_minimum_required(VERSION 3.5)
set(ic_lib io_utility.cpp algorithms/reporter.cpp)


add_library(ic_static ${ic_lib})
add_library(pmf pmf.cpp)

if(UNIX)
    SET_TARGET_PROPERTIES(ic_static PROPERTIES COMPILE_FLAGS "-fPIC")
    SET_TARGET_PROPERTIES(pmf PROPERTIES COMPILE_FLAGS "-fPIC")
endif()
