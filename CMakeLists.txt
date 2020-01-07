# CMakeLists.txt example for STXXL

project(cache)
cmake_minimum_required(VERSION 2.8)

# disallow in-source builds
if("${PROJECT_SOURCE_DIR}" STREQUAL "${PROJECT_BINARY_DIR}")
  message(SEND_ERROR "In-source builds are not allowed.")
endif("${PROJECT_SOURCE_DIR}" STREQUAL "${PROJECT_BINARY_DIR}")

# enable warnings (always good)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -W -Wall")

# apply STXXL CXXFLAGS to our configuration
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${STXXL_CXX_FLAGS}")

# build a program and link it with STXXL.
#add_executable(cache cache_adaptive.cpp)
#add_executable(cache non_cache_adaptive.cpp)
#add_executable(cache shell.cpp)

add_executable(non_cache_adaptive non_cache_adaptive.cpp)
add_executable(cache_adaptive cache_adaptive.cpp)
add_executable(cgroup_test cgroup_test.cpp)
add_executable(mmap_sanity mmap_sanity.cpp)
