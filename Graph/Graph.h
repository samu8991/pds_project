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
#include <memory>
#include <future>
#include <functional>
#include <thread>
#include <list>


using namespace std;
using namespace boost;
using namespace std::chrono;

namespace my_graph {
    enum algo{seq,p_seq,luby};
    enum rappr{list,matrix,csr};
    struct vertex_descriptor {
        int color;
        bool isDeleted;
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
        int threadAvailable = std::thread::hardware_concurrency();
        std::atomic<bool> exit_thread_flag{false};
        std::atomic<bool> alg_finished{false};
    protected:
        void
        parallel_count_adj(std::list<int> &l) {

            auto f = [&](int min, int max) {
                auto it = l.begin();
                std::advance(it, min);
                for (node i = min; i < max; i++) {
                    int j = 0;
                    auto neighbours = boost::adjacent_vertices(i, static_cast<T &>(*this).g);
                    for (auto vd: make_iterator_range(neighbours))j++;
                    l.insert(it, j);
                }
            };
            int step = static_cast<T &>(*this).N / static_cast<T&>(*this).threadAvailable;
            int min = 0;

            for (int i = 0; i < static_cast<T &>(*this).threadAvailable; i++) {
                if (i == static_cast<T &>(*this).threadAvailable
                        && static_cast<T &>(*this).N % 2 != 0) {
                    auto handle = async(std::launch::async, f, min, min + step + 1);
                } else auto handle = async(std::launch::async, f, min, min + step);
                min += step;
            }

        }

        void
        second_phase_luby(std::unordered_set<node>& I,std::list<int> d,int q){
            node i = 0;
            for(auto it = d.begin(); it != d.end(); ++it){
                if(*it == 0){
                    I.insert(i);
                    static_cast<T&>(*this).g[i].isDeleted = true;
                }
                if(*it >= static_cast<T&>(*this).N/16){
                    I.insert(i);
                    node* n;
                    for_each_neigh(i,n,[&](){
                        static_cast<T&>(*this).g[i].isDeleted = true;
                        static_cast<T&>(*this).g[*n].isDeleted = true;
                    });
                }
                else{
                    std::vector<int> X;
                    int y = rand()%(q-1);
                    int x = rand()%(q-1)+1;
                    std::list<std::thread> threads(static_cast<T&>(*this).threadAvailable);

                    auto f = [&](int start,int end){
                        auto it = X.begin()+start;
                        for(int i = start; i < end; ++i){
                            int n_i = q / (2* degree(i));
                            int l_i = (x+y*i)%q;
                            if(l_i < n_i)X[*it] = i;
                        }
                    };
                    int step = static_cast<T&>(*this).N/static_cast<T&>(*this).threadAvailable;
                    int i = 0;
                    for (int j= 0; j < static_cast<T&>(*this).threadAvailable; j++) {
                        threads.emplace_back(f,i,i+step);
                        i+=step;
                    }
                    for(auto& t:threads)t.join();
                    std::vector<int> I_p(X);
                    auto f1 = [&](){
                        vector<std::pair<int,int>> ret;
                        return ret;
                    };
                    std::copy(std::begin(I_p),std::end(I_p),std::inserter(I,std::end(I)));
                    std::unordered_set<node> N;
                    node* nod;
                    for_each_neigh(i,nod,[&](){
                       N.insert(*nod);
                    });
                    i++;
                    std::copy(std::begin(N),std::end(N),std::inserter(I_p,std::end(I_p)));
                    for_each_vertex(i,[&](){
                       static_cast<T&>(*this).g[i].isDeleted = true;
                    });
                }

            }
        }
        int
        creat_prime() {
            int q = 0;
            int n = static_cast<T&>(*this).N;
            for (int i = n; i <= 2 * n; i++) {
                if ((i % 2 == 0) || (i % 3 == 0) || (i % 5 == 0) || (i % 7 == 0) || (i % 9 == 0)) {
                    continue;
                } else {
                    q = i;
                    break;
                }
            }
            return q;
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
        startAlg(int16_t a){
            cout << "Starting simulation...\n";
            high_resolution_clock::time_point start = high_resolution_clock::now();
            thread t1([&](){
                int end = 60;
                int t = 0;
                while(!alg_finished && t < end){
                    std::this_thread::sleep_for(std::chrono::milliseconds (1000));
                    t++;
                }
                if(!alg_finished)exit_thread_flag = true;
                else;
            });
            switch (a) {
                case seq : {
                    cout << "Algorithm used is : sequential" << "\n";
                    sequential_algorithm();
                    break;
                }
                case p_seq: {
                    cout << "Algorithm used is : parallel sequential"<< "\n";
                    break;
                }
                case algo::luby: {
                    cout << "Algorithm used is : luby"<< "\n";
                    break;
                }
                default:
                        break;
            }
            alg_finished = true;
            if(exit_thread_flag)throw "Time exceded\n";
            else{
                high_resolution_clock::time_point end = high_resolution_clock::now();
                auto duration = duration_cast<microseconds>(end - start);
                auto durations = duration_cast<seconds>(end - start);
                cout << "Execution time >> " << duration.count() << "\u03BCs" << "(" << durations.count() <<"s)"<<endl;
                cout << "Simulation ended\n";
            }
            t1.join();
        }
        void
        printSol() {
            for (node i = 0; i < static_cast<T &>(*this).N; ++i)
                cout << static_cast<T &>(*this).g[i].color << endl;
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
        for_each_vertex(node node, std::function<void()> f) {
            return static_cast<T &>(*this).for_each_vertex(node, f);
        }

        void
        for_each_neigh(node node, ::my_graph::node* n, std::function<void()> f) {
            return static_cast<T &>(*this).for_each_neigh(node, n, f);
        }

        void
        for_each_edge(std::function<void()> f){
            return static_cast<T &>(*this).for_each_edge(f);
        }
    };

    class GraphCSR :
            public Graph<GraphCSR> {
    private:
        friend Graph<GraphCSR>;
        graphCSR g;
        int N;
    public:

        GraphCSR(int N, vector<Pair> &edge_array);

        void
        for_each_vertex(node node, std::function<void()> f);

        void
        for_each_neigh(node node, ::my_graph::node* n,std::function<void()> f);

        int
        degree(node node);

        void
        for_each_edge(std::function<void()> f);
    };


    class GraphAdjList :
            public Graph<GraphAdjList> {
    private:
        friend Graph<GraphAdjList>;
        graphAdjList g;
    public:

        GraphAdjList(int N, vector<Pair> &edge_array);

        void
        for_each_vertex(node node, std::function<void()> f);

        void
        for_each_neigh(node node, ::my_graph::node* n, std::function<void()> f);

        void
        for_each_edge(std::function<void()> f);

        int
        degree(node node);
    };

    class GraphAdjMatrix :
            public Graph<GraphAdjMatrix> {
    private:
        friend Graph<GraphAdjMatrix>;
        graphAdjMatrix g = graphAdjMatrix(0);
    public:

        GraphAdjMatrix(int N, vector<Pair> &edge_array);

        void
        for_each_vertex(node node, std::function<void()> f);

        void
        for_each_neigh(node node,::my_graph::node* n, std::function<void()> f);

        void
        for_each_edge(std::function<void()> f);

        int
        degree(node node);
    };
}
#endif //PDS_PROJECT_GRAPH_H
