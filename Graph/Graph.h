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
        parallel_count_adj(std::vector<int> &l) {
            std::vector<std::thread> threads(static_cast<T&>(*this).threadAvailable);
            int step = static_cast<T&>(*this).N/static_cast<T&>(*this).threadAvailable;int i = 0;
            for (int j= 0; j < static_cast<T&>(*this).threadAvailable; j++) {
                threads.emplace_back([this,i,&l](int start,int end){
                    for(node curr = start; curr < end; curr++){
                        int count = 0;
                        if(!static_cast<T&>(*this).g[curr].isDeleted){
                            node n;
                            for_each_neigh(curr,&n,[&count,this,&n](){
                                if(!static_cast<T&>(*this).g[n].isDeleted)
                                    count++;
                            });
                        }
                        l[curr] = count;
                    }
                    },i,i+step);

                i+=step;
            }
            for(auto& t:threads){
                if(t.joinable())
                    t.join();
            }
            threads.clear();
        }
        void
        second_phase_luby(std::unordered_set<node>& I,std::vector<int>& d,int q){

            std::vector<std::thread> threads(static_cast<T&>(*this).threadAvailable);
            int step = static_cast<T&>(*this).N/static_cast<T&>(*this).threadAvailable;
            int n = static_cast<T&>(*this).N;
            int s = 0;
            for (int j= 0; j < static_cast<T&>(*this).threadAvailable; j++) {
                threads.emplace_back([this, j,&d, &I,n,q,step,&threads](int start, int end) {
                    std::unique_lock<std::mutex> ul(m);
                    cout << "Thread " << j << " >> " << start << " , " << end << endl;

                    for(int i = start; i < end; ++i){
                        cout << "Thread " << j << " d[i]= " << d[i] << endl;
                        if (d[i] == 0 && !static_cast<T &>(*this).g[i].isDeleted) {
                            I.insert(i);
                            static_cast<T &>(*this).current_vertex_no--;
                            static_cast<T &>(*this).g[i].isDeleted = true;
                            assert(static_cast<T &>(*this).current_vertex_no >= 0);
                            cout << "Current vertex number 1 " << static_cast<T &>(*this).current_vertex_no << endl;
                        }
                        if(d[i]>= n/16 && !static_cast<T &>(*this).g[i].isDeleted){
                            I.insert(i);
                            node n;
                            static_cast<T&>(*this).g[n].isDeleted = true;
                            static_cast<T&>(*this).current_vertex_no--;
                            for_each_neigh(i,&n,[this,n](){
                                if(!static_cast<T &>(*this).g[n].isDeleted){
                                    static_cast<T&>(*this).g[n].isDeleted = true;
                                    static_cast<T&>(*this).current_vertex_no--;
                                }
                                assert(static_cast<T &>(*this).current_vertex_no >= 0);
                                cout << "Current vertex number 2 " << static_cast<T &>(*this).current_vertex_no << endl;
                            });
                        }else if(d[i] < n/16 && !static_cast<T &>(*this).g[i].isDeleted){
                            assert(d[i] != 0);
                            std::unordered_set<node> X;
                            int y = rand()%(q-1);
                            int x = rand()%(q-1)+1;
                            node n_i = q / (2 * d[i]);
                            node l_i = (x + y * i) % q;
                            if (l_i < n_i)X.insert(i);


                            std::unordered_set<node>I_p(X);
                            auto f1 = [this,&I_p,&d](int start,int end){
                                Pair current_edge;
                                std::function<void()> my_f = [this,&I_p,start,end,&d,&current_edge](){
                                    if (!static_cast<T &>(*this).g[current_edge.first].isDeleted && !static_cast<T &>(*this).g[current_edge.second].isDeleted
                                        && current_edge.first >= start
                                        && current_edge.first<= end
                                        && current_edge.second >= start
                                        && current_edge.second<= end){
                                        if (d[current_edge.first]<= d[current_edge.second]) {
                                            //auto it = std::find(I_p.begin(), I_p.end(), current_edge.first);
                                            I_p.erase(current_edge.first);
                                        } else {
                                            //auto it = std::find(I_p.begin(), I_p.end(), current_edge.second);
                                            I_p.erase(current_edge.second);
                                        }
                                    }
                                };
                                static_cast<T&>(*this).for_each_edge(&current_edge,my_f);
                            };
                            std::vector<std::thread> otherthreads(static_cast<T&>(*this).threadAvailable);
                            int k = 0;
                            for (int j= 0; j < static_cast<T&>(*this).threadAvailable; j++) {
                                otherthreads.emplace_back(f1,k,k+step);
                                k+=step;
                            }
                            for(auto& t:otherthreads){
                                if(t.joinable())
                                    t.join();
                            }
                            otherthreads.clear();

                            I.insert(I_p.begin(),I_p.end());
                            for(auto iter = I_p.begin(); iter != I_p.end();++iter){
                                node curr = *iter;
                                node nei;
                                static_cast<T&>(*this).g[curr].isDeleted = true;
                                static_cast<T &>(*this).current_vertex_no--;
                                for_each_neigh(curr,&nei,[this,nei](){
                                    static_cast<T&>(*this).g[nei].isDeleted = true;
                                    static_cast<T &>(*this).current_vertex_no--;
                                    assert(static_cast<T &>(*this).current_vertex_no >= 0);
                                    cout << "Current vertex number 3 " << static_cast<T &>(*this).current_vertex_no << endl;
                                });
                            }


                        }
                    }

                }, s, s + step);
                s += step;
            }
            for(auto& t:threads){
                if(t.joinable())
                    t.join();
            }
            threads.clear();



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
        jones_plassmann();

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
