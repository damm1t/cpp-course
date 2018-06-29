cmake_minimum_required(VERSION 3.9)

project(PERSISTENT_SET)
include_directories(${PERSISTENT_SET_SOURCE_DIR})

add_executable(set_testing
        set.h
        set.cpp
	main.cpp )

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11 -pedantic")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address,undefined -D_GLIBCXX_DEBUG")
endif()

target_link_libraries(set_testing -lpthread)
