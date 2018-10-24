cmake_minimum_required(VERSION 3.10)
# test for the core module


include_directories(${PROJECT_SOURCE_DIR})
include_directories(${GTEST_INCLUDE_DIR})
add_executable(TestAll test.cpp)
target_link_libraries(TestAll debug ${GTEST_MAIN_LIBRARY_DEBUG} debug ${GTEST_LIBRARY_DEBUG})
target_link_libraries(TestAll optimized ${GTEST_MAIN_LIBRARY} optimized ${GTEST_LIBRARY})

add_test(NAME TestAll COMMAND TestAll)
