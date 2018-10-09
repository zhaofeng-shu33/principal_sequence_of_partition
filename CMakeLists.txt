cmake_minimum_required(VERSION 3.10)

project(sfm LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${PROJECT_SOURCE_DIR})
find_package(Eigen3 CONFIG REQUIRED)
add_executable(main main2.cpp)
target_link_libraries(main PRIVATE Eigen3::Eigen)
