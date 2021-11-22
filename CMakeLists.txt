cmake_minimum_required(VERSION 3.20)
project(pds_project)

set(CMAKE_CXX_STANDARD 17)

find_package (Threads REQUIRED Boost COMPONENTS program_options REQUIRED)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
include_directories("../boost_1_76_0")

add_executable(pds_project
        main.cpp
        Graph/Graph.h menu.h)
