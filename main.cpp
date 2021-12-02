#include <iostream>
#include "Graph/Graph.h"

using namespace std;
using namespace my_graph;

#define DAVIDE
#ifdef TEST
class WebPage
{
public:
    std::string url;
};

#endif

string fileName(){
    string firstName = "C:\\Users\\neyma\\CLionProjects\\untitled4\\";
    string finalName;
    cout << "Inserisci il nome del file: ";
    cin >> finalName;
    return firstName.append(finalName);
}

ifstream readFile(){
    ifstream inFile;
    do{
        inFile.open(fileName(), std::ifstream::in);
        if(!inFile) {
            cout << "File non trovato" << endl;
            //throw(inFile.fail());
        }
    }while(!inFile);
    return inFile;
}

vector <Pair> getAdjacencies(node &N){
    ifstream file = readFile();
    vector <Pair> pairs;

    std::string line;
    std::vector<std::string> firstLine; //nodi e archi
    getline(file, line);
    boost::split(firstLine, line, [](char c){return c == ' ';});
    N = stoi(firstLine[0]);
    node edges = stoi(firstLine[1]);
    node n = 1;
    while(getline(file, line)) {
        std::set<std::string> adjacencies;
        boost::split(adjacencies, line, [](char c){return c == ' ';});
        for (auto x : adjacencies){
            node adjs;
            stringstream(x)>>adjs;

            //cout << "adj: " << a << endl;
            //rgg_n_2_15_s0.graph
            Pair p;
            p.first = n;
            p.second = adjs;

            pairs.push_back(p);
        }
        n++;
    }
    return pairs;
}




/*graphCSR createGraph(int& nodes){
    vector <Pair> pairs = getAdjacencies(nodes);
    GraphCSR g (nodes, pairs);
    for(auto x : pairs)
        if(!edge(x.first, x.second, g).second)
            add_edge(x.first, x.second, g);
    return g;
}
*/
int main(){

    node N = 0;
    vector <Pair> edges = getAdjacencies(N);
    GraphAdjMatrix g(N,edges);
    g.printGraph();
//    g.sequential_algorithm();
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
    dynamic_properties dp;
    dp.property("label", get(&WebPage::url, g));
    std::ofstream out("web-graph.dot");
    write_graphviz(out, g, dp, std::string(), get(vertex_index, g));
#endif
#ifdef DAVIDE

#endif
    return EXIT_SUCCESS;
}
