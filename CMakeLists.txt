cmake_minimum_required(VERSION 3.16)
project(formal)

# set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")

find_package(fmt REQUIRED)

add_subdirectory(libformal)
add_subdirectory(playground)