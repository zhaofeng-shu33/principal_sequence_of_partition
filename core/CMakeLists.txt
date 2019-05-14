cmake_minimum_required(VERSION 3.5)
set(ic_lib io_utility.cpp algorithms/reporter.cpp)


add_definitions(-D_DEBUG)
add_library(ic_static ${ic_lib})
add_library(pmf pmf.cpp)
find_package(Boost REQUIRED serialization)
target_include_directories(pmf PUBLIC ${Boost_INCLUDE_DIRS})

if(UNIX)
    SET_TARGET_PROPERTIES(ic_static PROPERTIES COMPILE_FLAGS "-fPIC")
    SET_TARGET_PROPERTIES(pmf PROPERTIES COMPILE_FLAGS "-fPIC")
endif()
