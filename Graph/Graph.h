//
// Created by voidjocker on 19/11/21.
//

#ifndef PDS_PROJECT_GRAPH_H
#define PDS_PROJECT_GRAPH_H

#include <boost/graph/compressed_sparse_row_graph.hpp>
#include <boost/graph/adjacency_matrix.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/graph/iteration_macros.hpp>
#include <boost/graph/graph_utility.hpp>
#include <atomic>
#include <ctime>
#include <exception>
#include <memory>
#include <future>
#include <functional>
#include <thread>
#include <list>
#include <random>



using namespace std;
using namespace boost;
using namespace std::chrono;

namespace my_graph {
    enum algo{seq,p_seq,luby,jp};
    enum rappr{list,matrix,csr};
    struct vertex_descriptor {
        int color;
        bool isDeleted;
        int weight;
    };

    typedef boost::adjacency_list<boost::listS,
                                    boost::vecS,
                                        boost::directedS,vertex_descriptor> graphAdjList;

    typedef boost::adjacency_matrix<boost::undirectedS,
            vertex_descriptor> graphAdjMatrix;

    typedef boost::compressed_sparse_row_graph<
            boost::bidirectionalS,
            vertex_descriptor> graphCSR;

#if _WIN64 || _WIN32
    typedef unsigned int node;
#else
    typedef unsigned long node;
#endif

    typedef pair<node, node> Pair;

    template<typename T>
    class Graph {

    private:
        Graph() = default;
        friend T;
    protected:
        int N;
        int current_vertex_no;
        int threadAvailable;
        std::atomic<bool> exit_thread_flag{false};
        std::atomic<bool> alg_finished{false};
        std::mutex m;
    protected:
        void
        generate_random_distr(std::unordered_set<node>& S,node start,node end){
            // serve per fare in modo che almeno un nodo venga preso dentro end e start
            node nonsisamai = static_cast<T&>(*this).N+1;
            node curr;
            for_each_vertex(&curr, [&curr,&nonsisamai,this,&S,&start,&end]() {
                // nodo compreso tra start e end e non deve essere stato cancellato
                if(curr <= end && curr >= start && !static_cast<T&>(*this).g[curr].isDeleted){
                    if(!static_cast<T&>(*this).g[curr].isDeleted){
                        nonsisamai = curr;
                    }
                    std::random_device rd;
                    std::uniform_int_distribution<int> distribution(0, 1);
                    std::mt19937 engine(rd()); // Mersenne twister MT19937
                    int value = distribution(engine);
                    float threshold = 0.2;

                    if (value > threshold) {
                        S.insert(curr);

                    }
                }
            });
            //cout << "non " << nonsisamai << endl;

            if(S.size() == 0 && nonsisamai != static_cast<T&>(*this).N+1)S.insert(nonsisamai);
        }
        int
        degree_induced_graph(node curr){
            int count = 0;
            node nei;
            for_each_neigh(curr,&nei,[this,&nei,&count](){
               if(!static_cast<T&>(*this).g[nei].isDeleted)
                   count++;
            });
            return count;
        }

    public:
        /*** put here algorithms ***/
        void
        sequential_algorithm();

        void
        parallel_sequential_algorithm();

        void
        luby();

        void
        jones_plassmann();

        void
        startAlg(int16_t a){
            cout << "Starting simulation...\n";
            /*high_resolution_clock::time_point start = high_resolution_clock::now();
            thread t1([&](){
                int end = 60;
                int t = 0;
                while(!alg_finished && t < end){
                    std::this_thread::sleep_for(std::chrono::milliseconds (1000));
                    t++;
                }
                if(!alg_finished)exit_thread_flag = true;
                else;
            });*/
            switch (a) {
                case seq : {
                    cout << "Algorithm used is : sequential" << "\n";
                    sequential_algorithm();
                    break;
                }
                case p_seq: {
                    cout << "Algorithm used is : parallel sequential"<< "\n";
                    parallel_sequential_algorithm();
                    break;
                }
                case algo::luby: {
                    cout << "Algorithm used is : Luby"<< "\n";
                    luby();
                    break;
                }
                case jp: {
                    cout << "Algorithm used is : Jones-Plassmann"<< "\n";
                    jones_plassmann();
                    break;
                }
                default:
                        break;
            }
            /*alg_finished = true;
            if(exit_thread_flag){
                try{
                    throw std::runtime_error("unexpected end of stream");
                }catch(const runtime_error& e){
                    cerr << "Time limit exceeded\n";
                }
            }
            else{
                high_resolution_clock::time_point end = high_resolution_clock::now();
                auto duration = duration_cast<microseconds>(end - start);
                auto durations = duration_cast<seconds>(end - start);
                cout << "Execution time >> " << duration.count() << "\u03BCs" << "(" << durations.count() <<"s)"<<endl;
                cout << "Simulation ended\n";
            }
            t1.join();*/
        }

        void
        printSolCorrectness() {
            node curr;
            for_each_vertex(&curr,[this,&curr](){
                node nei;
                cout << "Nodo: " << curr << " colore: " << static_cast<T &>(*this).g[curr].color << endl;
                for_each_neigh(curr,&nei,[this,curr,&nei](){
                    if(static_cast<T&>(*this).g[curr].color == static_cast<T&>(*this).g[nei].color){
                        cout << "Incorrect solution \n";
                        exit(-2);
                    }
                });
            });
            cout << "Solution is correct\n";
        }

        void
        printGraph() {
            boost::print_graph(static_cast<T &>(*this).g);
        }

        int
        degree(node node){
            return static_cast<T &>(*this).degree(node);
        }

        void
        for_each_vertex(node* current_vertex, std::function<void()> f){
            return static_cast<T&>(*this).for_each_vertex(current_vertex,f);
        };

        void
        for_each_neigh(node v, node* neighbor, std::function<void()> f){
            return static_cast<T&>(*this).for_each_neigh(v,neighbor,f);
        };

        void
        for_each_edge(Pair *current_edge,std::function<void()> f){
            return static_cast<T &>(*this).for_each_edge(current_edge,f);
        }


        void stampaGrafo();
    };

    class GraphCSR :
            public Graph<GraphCSR> {
    private:
        friend Graph<GraphCSR>;
        graphCSR g;
        int N;
    public:

        GraphCSR(int N, int nothreads,vector<Pair> &edge_array);

        void
        for_each_vertex(node* current_vertex, std::function<void()> f);

        void
        for_each_neigh(node v, node* neighbor, std::function<void()> f);

        int
        degree(node node);

        void
        for_each_edge(Pair *current_edge,std::function<void()> f);
    };


    class GraphAdjList :
            public Graph<GraphAdjList> {
    private:
        friend Graph<GraphAdjList>;
        graphAdjList g;
    public:

        GraphAdjList(int N, int nothreads, vector<Pair> &edge_array);

        void
        for_each_vertex(node* current_vertex, std::function<void()> f);

        void
        for_each_neigh(node v, node* neighbor, std::function<void()> f);

        void
        for_each_edge(Pair* current_edge,std::function<void()> f);

        int
        degree(node node);
    };

    class GraphAdjMatrix :
            public Graph<GraphAdjMatrix> {
    private:
        friend Graph<GraphAdjMatrix>;
        graphAdjMatrix g = graphAdjMatrix(0);
    public:

        GraphAdjMatrix(int N, int nothreads, vector<Pair> &edge_array);

        void
        for_each_vertex(node* current_vertex, std::function<void()> f);

        void
        for_each_neigh(node v, node* neighbor, std::function<void()> f);

        void
        for_each_edge(Pair* current_edge,std::function<void()> f);

        int
        degree(node node);
    };
}
#endif //PDS_PROJECT_GRAPH_H
