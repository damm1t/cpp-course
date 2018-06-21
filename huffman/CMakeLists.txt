cmake_minimum_required(VERSION 3.9)

project(HUFFMAN)
include_directories(${HUFFMAN_SOURCE_DIR})


add_library(huffman_lib STATIC
        library/huffman.cpp
	library/huffman.h
	library/huffexception.h
	library/huffexception.cpp )

add_executable(huffman
        library/huffman.h
        library/huffman.cpp
	library/huffexception.h
	library/huffexception.cpp
	main.cpp )

add_executable(huffman_testing
        tests/huffman_testing.cpp
        library/huffman.h
        library/huffman.cpp
	library/huffexception.h
	library/huffexception.cpp
        gtest/gtest-all.cc
        gtest/gtest.h
        gtest/gtest_main.cc )

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11 -pedantic")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address,undefined -D_GLIBCXX_DEBUG")
endif()

target_link_libraries(huffman_testing -lgmp -lgmpxx -lpthread)
