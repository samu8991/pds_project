//
// Created by voidjocker on 24/11/21.
//

#include "Graph.h"
GraphCSR::GraphCSR (int N,vector<Pair>& edge_array):N(N){
    g = graphCSR(boost::edges_are_unsorted_multi_pass,
                    std::begin(edge_array),
                        edge_array.end(), N);
}
