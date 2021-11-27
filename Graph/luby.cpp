//
// Created by Giacomo on 27/11/2021.
//

#include<iostream>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/named_function_params.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/graph/copy.hpp>
#include <string>
#include <future>
#include <boost/algorithm/string/split.hpp>


using namespace boost;
using namespace std;

const int NSegments = 4;

typedef property<boost::vertex_color_t,int> colorProperty;
typedef property<boost::vertex_degree_t,int> degreeProperty;

// serve per etichettare i nodi come rimossi, invece di rimuoverli
namespace boost {
    enum vertex_removed_t {
        vertex_removed = 124
    };
    BOOST_INSTALL_PROPERTY(vertex, removed);
}

// adjacency_list< EdgeList, VertexList, Directed, VertexProperties, EdgeProperties, GraphProperties>
typedef boost::adjacency_list<boost::vecS,boost::vecS,
        boost::undirectedS,
        boost::property<boost::vertex_color_t,int,
                boost::property<vertex_removed_t, int,
                        degreeProperty > >,
        no_property> Graph;


typedef graph_traits<Graph>::vertex_iterator vertex_iter;
std::pair<vertex_iter, vertex_iter> vp;

using V = Graph::vertex_descriptor;

// serve per creare una map da passare al filtered graph in modo da tenere solamente i modi etichettati come non rimossi
template <typename VerticesNotRemovedMap>
struct vertexNotRemoved {

    vertexNotRemoved() = default;
    explicit vertexNotRemoved(VerticesNotRemovedMap removed) : m_removed(removed) { }

    bool operator() (V v) const {
        return get(m_removed,v) == 0;
    }
    VerticesNotRemovedMap m_removed;
};

// serve a creare una map da passare al filtered graph dividendo in segmenti i vertici etichettati come non rimossi, in modo da lavorare in parallelo
template <typename VerticesNotRemovedMap>
struct SegmentVerticesPlusRemoved {
    unsigned m_segment;
    VerticesNotRemovedMap m_removed;

    SegmentVerticesPlusRemoved() = default;
    explicit SegmentVerticesPlusRemoved(VerticesNotRemovedMap removed, unsigned segment) : m_removed(removed), m_segment(segment) { }

    bool operator()(V vd) const {
        if(get(m_removed,vd) == 0)
            return (std::hash<V>{}(vd) % NSegments) == m_segment;
        else return false;
    }
};

// serve per creare una map da passare al filtered graph con i nodi non ancora colorati
template <typename VerticesNotColoredMap>
struct vertexNotColored {

    vertexNotColored() = default;
    explicit vertexNotColored(VerticesNotColoredMap colored) : m_colored(colored) { }

    bool operator() (V v) const {
        return get(m_colored,v) == 0;
    }
    VerticesNotColoredMap m_colored;
};

// serve a dividere i nodi in segmenti
struct SegmentVertices {
    unsigned segment;

    bool operator()(V vd) const {
        return (std::hash<V>{}(vd) % NSegments) == segment;
    }
};


typedef property_map<Graph, vertex_removed_t>::type VerticesNotRemovedMap;

typedef property_map<Graph, vertex_color_t>::type VerticesNotColoredMap;

using F = boost::filtered_graph<Graph, boost::keep_all, SegmentVerticesPlusRemoved<VerticesNotRemovedMap>>;

string fileName(){
    string firstName = "C:\\Users\\giogi\\OneDrive\\Desktop\\Graph-coloring-PDS\\benchmarks\\base\\";
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

set <pair <int, int>> getAdjacencies(int &nodes){
    ifstream file = readFile();
    set <pair <int, int>> pairs;

    std::string line;
    std::vector<std::string> firstLine; //nodi e archi
    getline(file, line);
    boost::split(firstLine, line, [](char c){return c == ' ';});
    nodes = stoi(firstLine[0]);

    int node = 1;
    while(getline(file, line)) {
        std::set<std::string> adjacencies;
        boost::split(adjacencies, line, [](char c){return c == ' ';});
        for (auto x : adjacencies){
            int adjs;
            stringstream(x)>>adjs;
            pairs.insert(make_pair(node, adjs));
        }
        node++;
    }
    return pairs;
}

Graph createGraph(int& nodes){
    set <pair <int, int>> pairs = getAdjacencies(nodes);
    Graph g (nodes);
    for(auto x : pairs)
        if(!edge(x.first, x.second, g).second)
            add_edge(x.first, x.second, g);
    return g;
}

int createPrimeq(int n){
    int q = 0;
    for(int i=n; i<= 2*n; i++) {
        if((i%2==0 && i!=2)||(i%3==0 && i!=3)||(i%5==0 && i!=5)||(i%7==0 && i!=7)||(i%9==0 && i!=9)){
            continue;
        } else {
            q=i;
            break;
        }
    }
    return q;
}

void parallel_count_adj_luby(Graph& g){

    //Creating a property_map with the degrees of the degrees of each vertex

    property_map<Graph,vertex_degree_t>::type deg = get(vertex_degree, g);
    property_map<Graph,vertex_removed_t>::type removed = get(vertex_removed, g);

    auto f = [&](F filtered_graph) {
        for (auto v : boost::make_iterator_range(vertices(filtered_graph))) {
            int j = 0;
            auto neighbours = boost::adjacent_vertices(v,g);
            for (auto vd : make_iterator_range(neighbours)) j++;
            deg[v] = j;
        }
    };



    for(unsigned segment = 0; segment < NSegments; ++segment){
        SegmentVerticesPlusRemoved<VerticesNotRemovedMap> filter(get(vertex_removed, g), segment);
        auto seg = F(g,{}, filter);
        auto handle = async(std::launch::async, f, seg);
    }

}

void parallel_degreeZero (Graph& g, std::unordered_set<int>& I) {
    //Creating a property_map with the degrees of the degrees of each vertex
    property_map<Graph,vertex_degree_t>::type deg = get(vertex_degree, g);
    property_map<Graph,vertex_removed_t>::type removed = get(vertex_removed, g);


    auto f = [&](F filtered_graph) {

        for (auto v : boost::make_iterator_range(vertices(filtered_graph))) {
            if (deg[v] == 0) {
                cout << v << " has a degree = 0" << endl;
                I.insert(v);
                removed[v] = 1;
            }
        }

        F::vertex_iterator v, vend;
        for(boost::tie(v,vend) = vertices(filtered_graph); v != vend; ++v)
            if(deg[*v] == 0)
                clear_vertex(*v, g);
    };

    for(unsigned segment = 0; segment < NSegments; ++segment){
        SegmentVerticesPlusRemoved<VerticesNotRemovedMap> filter(get(vertex_removed, g), segment);
        auto seg = F(g,{}, filter);
        auto handle = async(std::launch::async, f, seg);
    }
}

void parallel_computeNAndL(Graph& g, std::unordered_set<int>& X, int& q, int& x, int& y) {

    //Creating a property_map with the degrees of the degrees of each vertex
    property_map<Graph,vertex_degree_t>::type deg = get(vertex_degree, g);

    auto f = [&](F filtered_graph) {

        for (auto v : boost::make_iterator_range(vertices(filtered_graph))) {
            // compute n(i) = floor(q/2d(i)
            int n_i = q / (2 * deg[v]); // dovrei fare il floor in modo da sistemare i numeri negativi, che ne sono?
            // compute l(i) = (x+ y*i) mod q
            int l_i = (x + y * v) % q;
            // if l(i) <= n(i) then add i to X
            if (l_i <= n_i) X.insert(v);
        }
    };



    for(unsigned segment = 0; segment < NSegments; ++segment){
        SegmentVerticesPlusRemoved<VerticesNotRemovedMap> filter(get(vertex_removed, g), segment);
        auto seg = F(g,{}, filter);
        auto handle = async(std::launch::async, f, seg);
    }


}

// i e j devono essere in X
void parallel_lastLubyStep(Graph& g, std::unordered_set<int>& X, std::unordered_set<int>& IPrime) {

    //Creating a property_map with the degrees of the degrees of each vertex
    property_map<Graph,vertex_degree_t>::type deg = get(vertex_degree, g);
    property_map<Graph,vertex_degree_t>::type removed = get(vertex_degree, g);

    auto f = [&](filtered_graph<Graph, boost::keep_all, SegmentVertices> filtered_graph, auto v_x) {

        for (auto v_j : boost::make_iterator_range(vertices(filtered_graph))) {
            // if (i,j) belongs to E' then
            if(edge(v_x, v_j, g).second) {
                // if d(i) <= d(j) then I' <- I' - {i}
                if(deg[v_x] <= deg[v_j])
                    IPrime.extract(v_x);
                    // else I' <- I' - {j}
                else
                    IPrime.extract(v_j);
            }
        }

    };


    for (auto const& vd : X){
        for (unsigned segment = 0; segment < NSegments; ++segment) {
            filtered_graph<Graph, boost::keep_all, SegmentVertices> seg(g, {}, {segment});
            auto handle = async(std::launch::async, f, seg, vd);
        }
    }

}

void luby(Graph& g){

    /******************************** LUBY *********************/

    property_map<Graph,vertex_color_t>::type color = get(vertex_color, g);
    int min_color = 0;

    vertexNotColored<VerticesNotColoredMap> filter_colored(color);
    filtered_graph<Graph, boost::keep_all, vertexNotColored<VerticesNotColoredMap>> g_NotColored(g,{},filter_colored);
    do {

        // Create I <- empty
        std::unordered_set<int> I;
        // n = |V|
        int n = size(vertices(g));
        // compute a prime q such that n <= q <= 2n
        int q = createPrimeq(n);

        cout << "n vale " << n << " e q vale " << q << endl;

        // G' = (V',E') <- G = (V,E)
        Graph gPrime;
        copy_graph(g, gPrime);

        //Creating a property_map with the degrees of the degrees of each vertex
        property_map<Graph, vertex_degree_t>::type deg = get(vertex_degree, gPrime);
        property_map<Graph, vertex_removed_t>::type removed = get(vertex_removed, gPrime);



        // I create a filter map with all the vertices that has not been removed yet -> removed[vertex]=0
        vertexNotRemoved<VerticesNotRemovedMap> filter(removed);
        filtered_graph<Graph, boost::keep_all, vertexNotRemoved<VerticesNotRemovedMap> > gPrime_hasElements(gPrime, {},
                                                                                                            filter);

        //while G' != empty do
        do {


            cout << "while" << endl;


            // In parallel, for each i in V', compute d(i)
            parallel_count_adj_luby(gPrime);

            cout << "Found the degree of all verticex" << endl;


            // In parallel, for each i in V'
            // if d(i)=0 then add i to I and delete i from V'
            parallel_degreeZero(gPrime, I);

            cout << "remove all the vertices with degree 0" << endl;


            //find i in V' such that d(i) is maximum
            /** OTTIMIZZARE **/
            int maxDegree = -1;
            int vertexWithMaxDegree = -1;


            for (auto vd: make_iterator_range(vertices(gPrime_hasElements))) {

                if (deg[vd] > maxDegree) {
                    maxDegree = deg[vd];
                    vertexWithMaxDegree = vd;
                }
            }

            cout << "max vertex is " << vertexWithMaxDegree << " and degree is " << maxDegree << endl;

            // if d(i) >= n/16 then add i to I and let G' be
            // the graph induced on the vertices V'-({i} U N({i}))
            if (maxDegree >= n / 16) {
                I.insert(vertexWithMaxDegree);
                auto neighbours = adjacent_vertices(vertexWithMaxDegree, gPrime);
                std::set to_clear(neighbours.first, neighbours.second);
                for (auto v: to_clear) {
                    removed[v] = 1;
                    clear_vertex(v, gPrime);
                }

                // After I remove all the neighborhood I have to remove the vertex itself
                clear_vertex(vertexWithMaxDegree, gPrime);
                removed[vertexWithMaxDegree] = 1;

            }

            cout << "if d(i) >= n/16 then add i to I" << endl;


            for (auto vd: make_iterator_range(vertices(gPrime_hasElements))) {

                if (deg[vd] < n / 16) {
                    // randomly choose x and y such that 0 <= x,y <= q-1
                    int x = rand() % ((q - 1) + 1);
                    int y = rand() % ((q - 1) + 1);
                    // X <- empty
                    std::unordered_set<int> X;

                    cout << "calculate x = " << x << " and y = " << y << endl;

                    parallel_computeNAndL(gPrime, X, q, x, y);

                    cout << "Calcutated n_i and l_i for all vertices in V'" << endl;
                    cout << "X has inside " << X.size() << " elements" << endl;

                    // I' <- X
                    std::unordered_set<int> IPrime;
                    for (const auto &elem: X)
                        IPrime.insert(elem);

                    /** OTTIMIZZARE, C'E' SEMPRE UN FOR NON IN PARALLELO **/
                    parallel_lastLubyStep(gPrime, X, IPrime);

                    cout << " find I " << endl;

                    // I <- I U I'
                    for (const auto &elem: IPrime)
                        I.insert(elem);

                    // Y <- I' U N(I')
                    std::unordered_set<int> Y;
                    for (const auto &elem: IPrime) {
                        Y.insert(elem);
                        auto neighbours = boost::adjacent_vertices(elem, gPrime);
                        for (auto vd: make_iterator_range(neighbours)) {
                            if (removed[vd] != 1)
                                Y.insert(vd);
                        }
                    }

                    // G' = (V',E') is the induced subgraph on V'-Y
                    for (const auto &elem: Y) {
                        clear_vertex(elem, gPrime);
                        removed[elem] = 1;
                    }

                }
            }

            cout << size(vertices(gPrime_hasElements)) << " vertici ancora in gPrime con removed = 0" << endl;

        } while (size(vertices(gPrime_hasElements)) != 0);

        cout << I.size() << " elementi colorati con color = " << min_color << endl;


        for (const auto &elem: I) {
            color[elem] = min_color;
            min_color = min_color + 1;
        }

    } while(size(vertices(g_NotColored)) != 0);

}