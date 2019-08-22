cmake_minimum_required(VERSION 3.5)

include_directories(${GTEST_INCLUDE_DIR})
add_executable(TestAll test.cpp)
target_link_libraries(TestAll ic_static)


if(UNIX AND "${CMAKE_BUILD_TYPE}" STREQUAL "")
set(CMAKE_BUILD_TYPE "Release")
endif()


if(WIN32)
# WIN32 supports multiple target, so debug library is necessary
target_link_libraries(TestAll debug ${GTEST_MAIN_LIBRARY_DEBUG} debug ${GTEST_LIBRARY_DEBUG})
target_link_libraries(TestAll optimized ${GTEST_MAIN_LIBRARY} optimized ${GTEST_LIBRARY})
target_link_libraries(TestAll set_stl)
endif()

if(UNIX)
target_link_libraries(TestAll ${GTEST_MAIN_LIBRARY} ${GTEST_LIBRARY})
target_link_libraries(TestAll set_stl)
endif()
target_link_libraries(TestAll InterruptibleThread)
target_link_libraries(TestAll ${LEMON_LIBRARY})

add_test(NAME TestAll COMMAND TestAll)
