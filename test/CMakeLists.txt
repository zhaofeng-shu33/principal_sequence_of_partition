cmake_minimum_required(VERSION 3.5)
# test for the core module
option(USE_EIGEN3 "USE Eigen3 Library" OFF)

include_directories(${GTEST_INCLUDE_DIR})
add_executable(TestAll test.cpp)
target_link_libraries(TestAll ic_static)

add_executable(test_agg test_agglomerative.cpp)
target_link_libraries(test_agg ic_static)

add_executable(test_pmf test_pmf.cpp)
target_link_libraries(test_pmf pmf)
target_link_libraries(test_pmf set_stl)
target_link_libraries(test_pmf ic_static)
target_link_libraries(test_pmf ${LEMON_LIBRARY})

if(UNIX AND "${CMAKE_BUILD_TYPE}" STREQUAL "")
set(CMAKE_BUILD_TYPE "Release")
endif()


if(WIN32)
# WIN32 supports multiple target, so debug library is necessary
target_link_libraries(TestAll debug ${GTEST_MAIN_LIBRARY_DEBUG} debug ${GTEST_LIBRARY_DEBUG})
target_link_libraries(TestAll optimized ${GTEST_MAIN_LIBRARY} optimized ${GTEST_LIBRARY})
if(USE_EIGEN3)
    target_link_libraries(test_agg debug ${GTEST_MAIN_LIBRARY_DEBUG} debug ${GTEST_LIBRARY_DEBUG})
    target_link_libraries(test_agg optimized ${GTEST_MAIN_LIBRARY} optimized ${GTEST_LIBRARY})
endif()
target_link_libraries(test_pmf debug ${GTEST_MAIN_LIBRARY_DEBUG} debug ${GTEST_LIBRARY_DEBUG})
target_link_libraries(test_pmf optimized ${GTEST_MAIN_LIBRARY} optimized ${GTEST_LIBRARY})

endif()

if(UNIX)
target_link_libraries(TestAll ${GTEST_MAIN_LIBRARY} ${GTEST_LIBRARY} pthread)
target_link_libraries(test_agg ${GTEST_MAIN_LIBRARY} ${GTEST_LIBRARY} pthread)
target_link_libraries(test_pmf ${GTEST_MAIN_LIBRARY} ${GTEST_LIBRARY} pthread)
endif()

if(USE_LEMON)
target_link_libraries(TestAll ${LEMON_LIBRARY})
endif()    

add_test(NAME TestAll COMMAND TestAll)
