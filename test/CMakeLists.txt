cmake_minimum_required(VERSION 3.5)
# test for the core module


include_directories(${PROJECT_SOURCE_DIR})
include_directories(${GTEST_INCLUDE_DIR})
add_executable(TestAll test.cpp)
if(UNIX AND "${CMAKE_BUILD_TYPE}" STREQUAL "")
set(CMAKE_BUILD_TYPE "Release")
endif()
if((UNIX AND "${CMAKE_BUILD_TYPE}" STREQUAL debug) OR WIN32)
# WIN32 supports multiple target, so debug library is necessary
target_link_libraries(TestAll debug ${GTEST_MAIN_LIBRARY_DEBUG} debug ${GTEST_LIBRARY_DEBUG})
endif()

if(WIN32)
target_link_libraries(TestAll optimized ${GTEST_MAIN_LIBRARY} optimized ${GTEST_LIBRARY})
endif()

if((UNIX AND "${CMAKE_BUILD_TYPE}" STREQUAL release))
target_link_libraries(TestAll ${GTEST_MAIN_LIBRARY} ${GTEST_LIBRARY} pthread)
endif()

add_test(NAME TestAll COMMAND TestAll)
