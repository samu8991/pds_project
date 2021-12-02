#include <iostream>
#include "Graph/Graph.h"

using namespace std;
using namespace my_graph;
#ifdef TEST
class WebPage
{
public:
    std::string url;
};

#endif

int 
main(){

    const int N = 5;

    vector<Pair> edge_array = { Pair(0,1), Pair(0,2), Pair(0,3),
                                Pair(0,4),
                                Pair(2,4), Pair(3,1), Pair(3,4),
                                Pair(4,1) };
    GraphCSR g(N,edge_array);
    g.printGraph();
    g.sequential_algorithm();
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

    return EXIT_SUCCESS;
}
