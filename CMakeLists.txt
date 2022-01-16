cmake_minimum_required(VERSION 3.17)
project(pds_project)

set(CMAKE_CXX_STANDARD 17)

#find_package (Threads REQUIRED)
#find_package(Boost COMPONENTS program_options REQUIRED)
#INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} )
if(UNIX)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
    #include_directories("../boost_1_77_0")
    find_package(Boost COMPONENTS program_options filesystem REQUIRED)
    message(${Boost_INCLUDE_DIR})
    include_directories(${Boost_INCLUDE_DIR})

    add_executable(pds_project
        main.cpp
        Graph/Graph.h Graph/GraphCSR.cpp Graph/GraphAdjList.cpp Graph/GraphAdjMatrix.cpp Graph/algorithms.cpp)

    target_link_libraries( pds_project boost_program_options boost_filesystem pthread)
endif(UNIX)

if(WIN32)
    set(Boost_USE_STATIC_LIBS ON)
    set(BOOST_ROOT C:/boost)
    find_package (Threads REQUIRED)
    find_package(Boost 1.77.0 COMPONENTS program_options filesystem REQUIRED)
    INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} )

    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

    add_executable(pds_project
        main.cpp
        Graph/Graph.h Graph/GraphCSR.cpp Graph/GraphAdjList.cpp Graph/GraphAdjMatrix.cpp Graph/algorithms.cpp)
    TARGET_LINK_LIBRARIES( pds_project LINK_PUBLIC ${Boost_LIBRARIES} )
endif(WIN32)
