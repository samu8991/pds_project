//
// Created by voidjocker on 24/11/21.
//
#include "Graph.h"
using namespace my_graph;
GraphAdjList::GraphAdjList (int N,vector<Pair>& edge_array){
    this->N = N;
    this->current_vertex_no = N;
    g = graphAdjList(edge_array.begin(),edge_array.end(),N);
    BGL_FORALL_VERTICES(current_vertex, g, graphAdjList) {
            g[current_vertex].color = -1;
            g[current_vertex].isDeleted = false;
            g[current_vertex].weight = rand() % 100 + 1;
    }
    cout << "Graph constructed:\n -Internal rappresentation >> Adjacent List \n -Number of nodes >> " << N << "\n";
}

void
GraphAdjList::for_each_vertex(node node,std::function<void()> f) {
    BGL_FORALL_VERTICES(curr, this->g, graphAdjMatrix) {
            node = curr;
            f();
        }
}
void
GraphAdjList::for_each_neigh(node current_vertex,node* n,std::function<void()> f){
    using AdjacencyIterator = boost::detail::adj_list_gen<adjacency_list<vecS,vecS, undirectedS, vertex_descriptor>,
                                                                    vecS, vecS, undirectedS, vertex_descriptor, no_property,
                                                                    no_property, listS>::config::adjacency_iterator;
    AdjacencyIterator neighbor{};
    AdjacencyIterator end{};
    for(auto[neighbor,end] = boost::adjacent_vertices(current_vertex,this->g);
        neighbor != end; ++neighbor){
        *n = *neighbor;
        f();
    }
}

int
GraphAdjList::degree(node node){
    return boost::out_degree(node,this->g);
}

void
GraphAdjList::for_each_edge(std::function<void()> f) {

}