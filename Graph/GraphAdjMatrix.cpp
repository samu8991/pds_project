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
    cout << "Graph constructed:\n -Internal rappresentation >> Adjacent Matrix \n -Number of nodes >> " << N << "\n";
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
GraphAdjMatrix::for_each_edge(std::function<void()> f) {
     typedef boost::adjacency_matrix<boost::undirectedS,
             vertex_descriptor>::edge_iterator edge_iter;

     std::pair<edge_iter, edge_iter> ep;
     edge_iter ei, ei_end;
     int i = 0;
     for (tie(ei, ei_end) = edges(g); ei != ei_end; ++ei)
         f();

}
