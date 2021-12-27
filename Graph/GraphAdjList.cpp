//
// Created by voidjocker on 24/11/21.
//
#include "Graph.h"
using namespace my_graph;
GraphAdjList::GraphAdjList (int N, int nothreads,vector<Pair>& edge_array){
    this->N = N;
    this->threadAvailable = nothreads;
    if(nothreads == 0)this->threadAvailable = std::thread::hardware_concurrency();
    this->current_vertex_no = N;
    cout << "Starting graph construction...\n";
    g = graphAdjList(edge_array.begin(),edge_array.end(),N);
    BGL_FORALL_VERTICES(current_vertex, g, graphAdjList) {
            g[current_vertex].color = -1;
            g[current_vertex].isDeleted = false;
            g[current_vertex].weight = rand() % 100 + 1;
    }
    cout << "Graph constructed:\n -Internal rappresentation >> Adjacent List \n -Number of nodes >> " << N << "\n";
}

void
GraphAdjList::for_each_vertex(node* current_vertex, std::function<void()> f){
    BGL_FORALL_VERTICES(curr, g, graphAdjList){
            *current_vertex = curr;
            f();
        }
}
void
GraphAdjList::for_each_neigh(node current_vertex, node* neighbor, std::function<void()> f){
    BGL_FORALL_ADJ(current_vertex, neigh, g, graphAdjList){
            *neighbor = neigh;
            f();
        }
}

int
GraphAdjList::degree(node node){
    return boost::out_degree(node,this->g);
}

void
GraphAdjList::for_each_edge(std::function<void()> f) {
    auto es = boost::edges(g);
    for (auto eit = es.first; eit != es.second; ++eit) {
        f();
    }
}