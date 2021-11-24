#include<iostream>

#include "menu.h"
#include "Graph/Graph.h"
using namespace std;
using namespace boost;

int 
main(){
    const int N = 5;

    vector<Pair> edge_array = { Pair(0,1), Pair(0,2), Pair(0,3),
                           Pair(0,4),
                           Pair(2,4), Pair(3,1), Pair(3,4),
                           Pair(4,1) };
    GraphAdjList g(N,edge_array);
    g.sequential_algorithm();
    return EXIT_SUCCESS;
}
