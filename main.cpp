#include <iostream>
#include <fstream>
#include <string>
#include "Graph/Graph.h"
#include <boost/property_map/dynamic_property_map.hpp>
#include <boost/graph/graphviz.hpp>

#define MAIN

using namespace std;
using namespace my_graph;
#ifdef MAIN
void credits(){
    std::cout << "**************************************CREDITS*********************************************\n";
    std::cout << "Vertex Coloring Project" << '\n';
    std::cout << "Politenico di Torino 2020/2021\n";
    std::cout << "Professors:\n";
    std::cout << " - Alessandro Savino\n";
    std::cout << " - Stefano Quer\n";
    std::cout << "Students:\n";
    std::cout << " - Paone Samuele\n";
    std::cout << " - Manca Davide\n";
    std::cout << " - Vitali Giacomo\n";
    std::cout << "***************************************MENU************************************************\n";
}
void
readFile(string& fname,vector<Pair>& edges,int& V){

    string line;int E;
    string buffer;
    stringstream lineStream;
    fstream fin(fname, ios::in);
    if(!fin.is_open()) {
        cerr << "errore apertura file fin" << endl;
    }

    std::getline(fin, line);
    lineStream = stringstream(line);
    lineStream >> V >> E;

    int neighbour;
    edges.reserve(E); //riservo E posti,

    for(int i=0; i<V; i++){
        getline(fin, line);
        lineStream = stringstream(line);
        while(lineStream >> neighbour)
            edges.emplace_back(Pair(i,neighbour-1));
    }
    fin.close();
}
void
run_simulation(int V,int16_t r,int16_t a,vector<Pair>& edges){
    switch (r) {
        case 0:{
            GraphAdjList g(V,edges);
            g.startAlg(a);
            break;
        }
        case 1:{
            GraphAdjMatrix g(V,edges);
            g.startAlg(a);
            break;
        }
        case 2:{
           GraphCSR g(V, edges);
            g.startAlg(a);
           break;
        }
        default:
            break;

    }
}
void
menu(){

    cout << "This program solves the vertex coloring problem implementing 3 parallel algorithms:\n";
    cout << "- Luby algorithm\n- Parallel sequential algorithm\n- TODO\n";
    cout << "In order to have more realistic simulations, another feature of this program is that graphs are implemented whith 3 different"
            "rappresentations: \n";
    cout << "- Adjacent List\n- Adjacency Matrix\n- Compressed Sparse Row\n";
}
void
start(){
    vector<Pair> edges;int V;
    string fileName;int16_t r,a;
    credits();
    menu();
    cout << "Inserire il nome del file >> ";
    cin >> fileName;
    cout << "Inserire rappresentazione del grafo >> ";
    cin >> r;
    cout << "Inserire algoritmo >> ";
    cin >> a;
    readFile(fileName,edges,V);
    run_simulation(V,r,a,edges);
}
#endif
struct WebPage{
    string url = "";
};
int 
main(){
#ifdef MAIN
    ///home/voidjocker/Downloads/quer_project_01_COLORING/benchmarks/rgg_n_2_15_s0.graph
    start();

  /*  const int N = 5;

    vector<Pair> edge_array = { Pair(0,1), Pair(0,2), Pair(0,3),
                                Pair(0,4),
                                Pair(2,4), Pair(3,1), Pair(3,4),
                                Pair(4,1) };
    GraphCSR g(N,edge_array);
    g.printGraph();
    g.sequential_algorithm();*/
#endif

#ifdef PROVA
    unique_ptr<vector<int>> vec = g.sequential_algorithm();

    g.printGraph();

    for(auto it = vec->begin(); it < vec->end(); it++)
        cout << *it << endl;
#endif
#ifdef TEST
    typedef std::pair<int, int> E;
    const char* urls[6] = {
            "http://www.boost.org/libs/graph/doc/index.html",
            "http://www.boost.org/libs/graph/doc/table_of_contents.html",
            "http://www.boost.org/libs/graph/doc/adjacency_list.html",
            "http://www.boost.org/libs/graph/doc/history.html",
            "http://www.boost.org/libs/graph/doc/bundles.html",
            "http://www.boost.org/libs/graph/doc/using_adjacency_list.html",
    };

    E the_edges[] = { E(0, 1), E(0, 2), E(0, 3), E(1, 0), E(1, 3), E(1, 5),
                      E(2, 0), E(2, 5), E(3, 1), E(3, 4), E(4, 1), E(5, 0),
                      E(5, 2) };

    typedef compressed_sparse_row_graph<directedS, WebPage> WebGraph;
    WebGraph g(boost::edges_are_sorted, &the_edges[0], &the_edges[0] + sizeof(the_edges)/sizeof(E), 6);

    // Set the URLs of each vertex
    int index = 0;
    BGL_FORALL_VERTICES(v, g, WebGraph)
            g[v].url = urls[index++];

    // Output each of the links
    std::cout << "The web graph:" << std::endl;
    BGL_FORALL_EDGES(e, g, WebGraph)
            std::cout << "  " << g[source(e, g)].url << " -> " << g[target(e, g)].url
                      << std::endl;

    // Output the graph in DOT format
    boost::dynamic_properties dp;
    dp.property("label", get(&WebPage::url, g));
    std::ofstream out("web-graph.dot");
    write_graphviz(out, g, dp, std::string(), get(vertex_index, g));
#endif

    return EXIT_SUCCESS;
}
