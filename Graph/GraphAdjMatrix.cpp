//
// Created by voidjocker on 24/11/21.
//
#include "Graph.h"

using namespace my_graph;
GraphAdjMatrix::GraphAdjMatrix(int N,vector<Pair>& edge_array){
    this->N = N;
    g = graphAdjMatrix (edge_array.begin(),edge_array.end(),N);
    BGL_FORALL_VERTICES(current_vertex, this->g, graphAdjMatrix) {
            g[current_vertex].color = -1;
    }
    cout << "Graph constructed:\n -Internal rappresentation >> Adjacent Matrix \n -Number of nodes >> " << N << "\n";
}

void
GraphAdjMatrix::for_each_vertex(unsigned long node,std::function<void()> f) {
    BGL_FORALL_VERTICES(curr, this->g, graphAdjMatrix) {
            node = curr;
            f();
    }
}
void
GraphAdjMatrix::for_each_neigh(unsigned long  current_vertex,unsigned long* n,std::function<void()> f){
    using AdjacencyIterator = boost::detail::adj_list_gen<adjacency_list<vecS, vecS, undirectedS, vertex_descriptor>, vecS, vecS, undirectedS, vertex_descriptor, no_property, no_property, listS>::config::adjacency_iterator;
    AdjacencyIterator neighbor{};
    AdjacencyIterator end{};
    for(auto[neighbor,end] = boost::adjacent_vertices(current_vertex,this->g);
        neighbor != end; ++neighbor){
        *n = *neighbor;
        f();
    }
}