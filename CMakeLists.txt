cmake_minimum_required(VERSION 3.5)
if(WIN32)
    if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
      set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
          CACHE STRING "")
    endif()
    if(DEFINED ENV{VCPKG_DEFAULT_TRIPLET} AND NOT DEFINED VCPKG_TARGET_TRIPLET)
      set(VCPKG_TARGET_TRIPLET "$ENV{VCPKG_DEFAULT_TRIPLET}" CACHE STRING "")
    endif()
endif()
project(sfm LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set (PSP_VERSION_MAJOR 1.3 CACHE STRING "PSP version")
option(USE_CXX11_ABI "USE GLIB CXX11 ABI" ON)
option(CODE_COVERAGE "Enable coverage reporting" OFF)

message(${CMAKE_SYSTEM})

if(UNIX AND CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions("-D_DEBUG")
endif()

if(NOT USE_CXX11_ABI)
    add_definitions("-D_GLIBCXX_USE_CXX11_ABI=0")
endif()

find_package(lemon REQUIRED)

add_executable(main main.cpp)

find_package(Boost REQUIRED program_options)

target_include_directories(main PUBLIC ${Boost_INCLUDE_DIRS})

if(WIN32)
    target_link_libraries(main debug ${Boost_PROGRAM_OPTIONS_LIBRARY_DEBUG})    
    target_link_libraries(main optimized ${Boost_PROGRAM_OPTIONS_LIBRARY_RELEASE})
else()    
    target_link_libraries(main ${Boost_PROGRAM_OPTIONS_LIBRARY_RELEASE})
endif()
include_directories(${PROJECT_SOURCE_DIR})
include_directories(${LEMON_INCLUDE_DIR})



option(ENABLE_TESTING "Enable testing" OFF)
if(ENABLE_TESTING)
  find_package(GTest REQUIRED)
  include(CTest)
endif()
add_subdirectory(psp)
target_link_libraries(main psp)
target_link_libraries(main set_stl)
target_link_libraries(main ${LEMON_LIBRARY})

if(ENABLE_TESTING)
  add_subdirectory(test)
endif()
