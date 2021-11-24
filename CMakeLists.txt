cmake_minimum_required(VERSION 3.17)
project(pds_project)

set(CMAKE_CXX_STANDARD 17)

find_package (Threads REQUIRED)
find_package(Boost COMPONENTS program_options REQUIRED)
INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} )

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
include_directories("../boost_1_76_0")

add_executable(pds_project
        main.cpp
        Graph/Graph.h menu.h Graph/GraphCSR.cpp Graph/GraphAdjList.cpp Graph/GraphAdjMatrix.cpp)
TARGET_LINK_LIBRARIES( pds_project LINK_PUBLIC ${Boost_LIBRARIES} )