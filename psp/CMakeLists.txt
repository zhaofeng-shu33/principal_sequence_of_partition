cmake_minimum_required(VERSION 3.5)
if(AGG_FEATURE)
    find_package(Eigen REQUIRED)
endif()
set(psp_lib digraph_utility.cpp io_utility.cpp pmf.cpp pmf_r.cpp dt.cpp psp_i.cpp psp.cpp agglomerative.cpp hpsp.cpp)

add_library(psp_static ${psp_lib})

target_compile_definitions(psp_static PRIVATE -DINTERRUPT)
if(AGG_FEATURE)
    target_compile_definitions(psp_static PRIVATE -DAGG)
endif()


if(CODE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(psp_static PRIVATE --coverage)
    target_link_libraries(psp_static INTERFACE --coverage)
endif()
target_link_libraries(psp_static PRIVATE InterruptibleThread)
if(UNIX)
    SET_TARGET_PROPERTIES(psp_static PROPERTIES COMPILE_FLAGS "-fPIC")
endif()
add_subdirectory(set)
add_subdirectory(preflow)

if(UNIX)
    add_library(psp SHARED psp.cpp)
    target_link_libraries(psp PRIVATE psp_static)
    target_link_libraries(psp PRIVATE InterruptibleThread)
    target_link_libraries(psp PRIVATE ${LEMON_LIBRARY})
    target_link_libraries(psp PRIVATE set_stl)
    
    INSTALL(TARGETS psp
    LIBRARY DESTINATION lib)
    INSTALL(FILES psp.h DESTINATION include/psp)
    INSTALL(FILES set/set_stl.h DESTINATION include/psp/set)
endif()