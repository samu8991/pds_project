//
// Created by voidjocker on 24/11/21.
//

#include <iostream>
#include "Graph.h"
using namespace my_graph;
GraphCSR::GraphCSR (int N, int nothreads,vector<Pair>& edge_array){
    this->N = N;
    this->threadAvailable = nothreads;
    if(nothreads == 0)this->threadAvailable = std::thread::hardware_concurrency();
    this->current_vertex_no = N;
    cout << "Starting graph construction...\n";
    g = graphCSR(boost::edges_are_unsorted_multi_pass,
                    std::begin(edge_array),
                        edge_array.end(), N);
    BGL_FORALL_VERTICES(current_vertex, g, graphCSR) {
            g[current_vertex].color = -1;
            g[current_vertex].isDeleted = false;
            g[current_vertex].weight = rand() % 100 + 1;
    }
    cout << "Graph constructed:\n -Internal rappresentation >> Compressed Sparse Row \n -Number of nodes >> " << N << "\n";
}

void
GraphCSR::for_each_vertex(node* current_vertex, std::function<void()> f){
    BGL_FORALL_VERTICES(curr, g, graphCSR){
            *current_vertex = curr;
            f();
        }
}
void
GraphCSR::for_each_neigh(node current_vertex, node* neighbor, std::function<void()> f){
    BGL_FORALL_ADJ(current_vertex, neigh, g, graphCSR){
            *neighbor = neigh;
            f();
        }
}
/*
void
GraphCSR::for_each_vertex(node node,std::function<void()> f) {
    BGL_FORALL_VERTICES(curr, g, graphCSR) {
            node = curr;
            f();
        }
}*/
/*
void
GraphCSR::for_each_neigh(node current_vertex,node n,std::function<void()> f){
    using AdjacencyIterator = boost::detail::adj_list_gen<adjacency_list<
            vecS, vecS, undirectedS, vertex_descriptor>,
                vecS, vecS, undirectedS, vertex_descriptor, no_property, no_property, listS>
                    ::config::adjacency_iterator;
    AdjacencyIterator neighbor{};
    AdjacencyIterator end{};
    for(auto[neighbor,end] = boost::adjacent_vertices(current_vertex,this->g);
        neighbor != end; ++neighbor){
        n = *neighbor;
        f();
    }
}*/

void
GraphCSR::for_each_edge(std::function<void()> f) {

}

int
GraphCSR::degree(node node){
    return boost::out_degree(node,this->g);
}
