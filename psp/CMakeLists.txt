cmake_minimum_required(VERSION 3.5)
set(psp_lib digraph_utility.cpp io_utility.cpp pmf.cpp pmf_r.cpp dt.cpp psp_i.cpp psp.cpp)

add_library(psp ${psp_lib})

target_compile_definitions(psp PRIVATE -DINTERRUPT)

if(CODE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(psp PRIVATE --coverage)
    target_link_libraries(psp INTERFACE --coverage)
endif()
target_link_libraries(psp PRIVATE InterruptibleThread)
if(UNIX)
    SET_TARGET_PROPERTIES(psp PROPERTIES COMPILE_FLAGS "-fPIC")
endif()
add_subdirectory(set)
add_subdirectory(preflow)

INSTALL(TARGETS psp
ARCHIVE DESTINATION lib)

INSTALL(FILES psp.h DESTINATION include/psp
set/set_stl.h DESTINATION include/psp/set)