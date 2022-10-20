cmake_minimum_required(VERSION 3.5)

include_directories(${GTEST_INCLUDE_DIR})
add_executable(TestAll test.cpp gaussian2Dcase.cpp)
target_link_libraries(TestAll psp_static)


if(UNIX AND "${CMAKE_BUILD_TYPE}" STREQUAL "")
set(CMAKE_BUILD_TYPE "Release")
endif()


target_link_libraries(TestAll GTest::GTest GTest::Main)
target_link_libraries(TestAll set_stl)
target_link_libraries(TestAll ${LEMON_LIBRARY})

add_test(NAME TestAll COMMAND TestAll)
