cmake_minimum_required(VERSION 3.5)
set(ic_lib io_utility.cpp pmf.cpp dt.cpp sfm_mf.cpp psp_i.cpp)

add_library(ic_static ${ic_lib})

if(UNIX)
    SET_TARGET_PROPERTIES(ic_static PROPERTIES COMPILE_FLAGS "-fPIC")
endif()
