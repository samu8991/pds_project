//
// Created by voidjocker on 24/11/21.
//
#include "Graph.h"

using namespace my_graph;
GraphAdjMatrix::GraphAdjMatrix(int N, int nothreads,vector<Pair>& edge_array){
    this->N = N;
    this->current_vertex_no = N;
    this->threadAvailable = nothreads;
    if(nothreads == 0)this->threadAvailable = std::thread::hardware_concurrency();
    cout << "Starting graph construction...\n";
    g = graphAdjMatrix (edge_array.begin(),edge_array.end(),N);
    BGL_FORALL_VERTICES(current_vertex, this->g, graphAdjMatrix) {
            g[current_vertex].color = -1;
            g[current_vertex].isDeleted = false;
            g[current_vertex].weight = rand() % 100 + 1;
    }
    cout << "Graph constructed:\n -Internal rappresentation >> Adjacent Matrix \n -Number of nodes >> " << N << "\n -Number of branches >> " << edge_array.size() << "\n -avg. node degree>> "<< edge_array.size()/N <<"\n";
}

void
GraphAdjMatrix::for_each_vertex(node* current_vertex, std::function<void()> f){
    BGL_FORALL_VERTICES(curr, this->g, graphAdjMatrix){
            *current_vertex = curr;
            f();
        }
}
void
GraphAdjMatrix::for_each_neigh(node current_vertex, node* neighbor, std::function<void()> f){
    BGL_FORALL_ADJ(current_vertex, neigh, g, graphAdjMatrix){
            *neighbor = neigh;
            f();
        }
}

int
GraphAdjMatrix::degree(node node){
    return boost::out_degree(node,this->g);
}

void
GraphAdjMatrix::for_each_edge(Pair* current_edge,std::function<void()> f) {
    BGL_FORALL_EDGES(current, this->g, graphAdjMatrix){
            current_edge->first = source(current,g);
            current_edge->second = target(current,g);
            f();
        }
}
