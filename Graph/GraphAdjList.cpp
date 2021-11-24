//
// Created by voidjocker on 24/11/21.
//
#include "Graph.h"
GraphAdjList::GraphAdjList (int N,vector<Pair>& edge_array):N(N){
    g = graphAdjList (N);
    for (int i = 0; i < 8; ++i)
        add_edge(edge_array[i].first, edge_array[i].second, g);
}