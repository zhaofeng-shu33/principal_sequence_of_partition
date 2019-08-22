cmake_minimum_required(VERSION 3.5)
set(ic_lib digraph_utility.cpp io_utility.cpp pmf.cpp pmf_r.cpp dt.cpp psp_i.cpp graph/info_cluster.cpp graph/gaussian2Dcase.cpp)

add_library(ic_static ${ic_lib})

target_compile_definitions(ic_static PRIVATE -DINTERRUPT)

if(CODE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(ic_static PRIVATE --coverage)
    target_link_libraries(ic_static INTERFACE --coverage)
endif()
target_link_libraries(ic_static PRIVATE InterruptibleThread)
if(UNIX)
    SET_TARGET_PROPERTIES(ic_static PROPERTIES COMPILE_FLAGS "-fPIC")
endif()
