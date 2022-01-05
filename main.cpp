#include <iostream>
#include <string>
#include <cstdlib>
#include <boost/filesystem.hpp>
#include "Graph/Graph.h"


using namespace std;
using namespace my_graph;

void credits();
std::string getEnv( const std::string & var );
void readFile(string&,vector<Pair>&,int&);
void run_simulation(int,int16_t,int16_t,vector<Pair>&);
void automatic_simulation(int,vector<Pair>&);
void menu();
void show_benchmarks();
void start();
#define DEBUG 0
int 
main(){
#if DEBUG == 1
    vector<Pair> edge_array = { Pair(0,1), Pair(0,2), Pair(0,3),Pair(0,4),Pair(0,5),
                                  Pair(1,0),Pair(1,3),Pair(1,4),Pair(1,5),Pair(1,6),Pair(1,7),
                                  Pair(2,0),
                                  Pair(3,0),Pair(3,1), Pair(3,4),Pair(3,5),Pair(3,7),
                                  Pair(4,0),Pair(4,1),Pair(4,3),Pair(4,6),
                                  Pair(5,0),Pair(5,1),Pair(5,3),Pair(5,6),Pair(5,7),
                                  Pair(6,5),Pair(6,1),Pair(6,4),
                                  Pair(7,5),Pair(7,3),Pair(7,1)};
      GraphCSR g(8,2,edge_array);
      g.printGraph();
      g.luby();
#else
    start();
#endif
    return EXIT_SUCCESS;
}
std::string getEnv( const std::string & var ) {
    const char * val = std::getenv( var.c_str() );
    if ( val == nullptr ) { // invalid to assign nullptr to std::string
        return "";
    }
    else {
        return val;
    }
}
void
credits(){
    std::cout << "**************************************CREDITS*********************************************\n";
    std::cout << "Vertex Coloring Project" << '\n';
    std::cout << "Politecnico di Torino 2020/2021\n";
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
#if _WIN64 || _WIN32
    string base = "C:\\benchmarks\\";
#else
    string base = getEnv("HOME")+"/benchmarks/";
#endif
    fname = base+fname;
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
    cout << "Reading file... \n";
    for(int i=0; i<V; i++){
        getline(fin, line);
        lineStream = stringstream(line);
        while(lineStream >> neighbour)
            edges.emplace_back(Pair(i,neighbour-1));
    }
    cout << "File read\n";
    fin.close();
}
void
run_simulation(int V,int16_t r,int16_t a,vector<Pair>& edges, int8_t nothreads){
    switch (r) {
        case 0:{
            GraphAdjList g(V,nothreads,edges);
            g.startAlg(a);
            break;
        }
        case 1:{
            GraphAdjMatrix g(V,nothreads,edges);
            g.startAlg(a);
            break;
        }
        case 2:{
            GraphCSR g(V,nothreads,edges);
            g.startAlg(a);
            break;
        }
        default:
            break;

    }
}
void
automatic_simulation(int V,vector<Pair>& edges,int8_t nothreads){
    cout << "Automatic testing procedure entered\n";
    cout << "Every graph will be tested 9 times one time for each rappresentation and algorithm\n ";
#if _WIN64 || _WIN32
    std::string constructed_path_str_dbg="C:\\benchmarks\\";
#else
    std::string constructed_path_str_dbg = getEnv("HOME")+"/benchmarks/";
#endif
    std::string ext(".graph");int i = 0;
    for (auto& p : boost::filesystem::recursive_directory_iterator(constructed_path_str_dbg)) {
        cout << "Starting simulation number " << i << " on file " << p<<"\n";
        string s = p.path().generic_string();
        readFile(s, edges, V);
        for (int16_t a = 0; a < 3; ++a) {
            for (int16_t r = 0; r < 3; ++r) {
                run_simulation(V, r, a, edges,nothreads);
            }
        }
        i++;
    }
}
void
menu(){

    cout << "This program solves the vertex coloring problem implementing 3 parallel algorithms:\n";
    cout << "- Luby algorithm\n- Parallel sequential algorithm\n- Jones_plassmann\n";
    cout << "In order to have more realistic simulations, another feature of this program is that graphs are implemented whith 3 different"
            "rappresentations: \n";
    cout << "- Adjacent List\n- Adjacency Matrix\n- Compressed Sparse Row\n";
}
void
show_benchmarks(){
    cout<< "This is the list of benchmark used to test these algorithms...\n";
#if _WIN64 || _WIN32
    std::string constructed_path_str_dbg = "C:\\benchmarks\\";
#else
    std::string constructed_path_str_dbg = getEnv("HOME")+"/benchmarks/";
#endif
    std::string ext(".graph");
    for (auto& p : boost::filesystem::recursive_directory_iterator(constructed_path_str_dbg)){
        if (p.path().extension() == ext)
            std::cout << p << '\n';
    }

}
void
start(){
    vector<Pair> edges;int V=0;
    string fileName,automatic;int16_t r,a;
    int nothreads;
    credits();
    menu();
    show_benchmarks();
    cout<< "Do you want an automatic testing procedure? (y/n)";
    cin>>automatic;
    cout << "Choose thread's number(1,2,4,8 or hardware based(0))";
    cin>>nothreads;
    if(automatic == "y")
        automatic_simulation(V,edges,nothreads);
    else {
        cout << "Choose the graph you want to use for this test >> ";
        cin >> fileName;
        cout << "Choose the rappresentation >> ";
        cin >> r;
        cout << "Choose the algorithm >> ";
        cin >> a;
        readFile(fileName, edges, V);
        run_simulation(V, r, a, edges,nothreads);
    }
}
