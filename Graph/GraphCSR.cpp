//
// Created by voidjocker on 24/11/21.
//

#include <iostream>
#include "Graph.h"
using namespace my_graph;
GraphCSR::GraphCSR (int N,vector<Pair>& edge_array){
    this->N = N;
    g = graphCSR(boost::edges_are_unsorted_multi_pass,
                    std::begin(edge_array),
                        edge_array.end(), N);
    BGL_FORALL_VERTICES(current_vertex, g, graphCSR) {
            g[current_vertex].color = -1;
    }
    cout << "Graph constructed:\n -Internal rappresentation >> Compressed sparse row \n -Number of nodes >> " << N << "\n";
}

void
GraphCSR::for_each_vertex(unsigned long node,std::function<void()> f) {
    BGL_FORALL_VERTICES(curr, g, graphCSR) {
            node = curr;
            f();
        }
}

void
GraphCSR::for_each_neigh(unsigned long current_vertex,unsigned long* n,std::function<void()> f){
    using AdjacencyIterator = boost::detail::adj_list_gen<adjacency_list<
            vecS, vecS, undirectedS, vertex_descriptor>,
                vecS, vecS, undirectedS, vertex_descriptor, no_property, no_property, listS>
                    ::config::adjacency_iterator;
    AdjacencyIterator neighbor{};
    AdjacencyIterator end{};
    for(auto[neighbor,end] = boost::adjacent_vertices(current_vertex,this->g);
        neighbor != end; ++neighbor){
        cout << *neighbor << endl;
        *n = *neighbor;
        f();
    }
}

/*void
GraphCSR::mia_prova(unsigned long current_vertex, unsigned long *neighbor, std::function<void()> f) {

    BGL_FORALL_ADJ(current_vertex,neigh,g,GraphCSR){
        *neighbor = neigh;
        f();
    }
}*/
