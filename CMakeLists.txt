cmake_minimum_required(VERSION 3.16)
project(formal)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
set(CMAKE_CXX_STANDARD 20)

find_package(fmt REQUIRED)


include(FetchContent)
FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG release-1.12.1
)

FetchContent_MakeAvailable(googletest)

add_subdirectory(libformal)
add_subdirectory(playground)
add_subdirectory(hw4)
add_subdirectory(regexp)
add_subdirectory(earley)
add_subdirectory(cyk)
add_subdirectory(tests)