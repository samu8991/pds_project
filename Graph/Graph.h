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
#include <memory>
#include <future>
#include <functional>


using namespace std;
using namespace boost;


namespace my_graph {
    struct vertex_descriptor {
        int color;
    };

    typedef boost::adjacency_list<boost::vecS, boost::vecS,
            boost::undirectedS,
            vertex_descriptor> graphAdjList;

    typedef boost::adjacency_matrix<boost::undirectedS,
            vertex_descriptor> graphAdjMatrix;

    typedef boost::compressed_sparse_row_graph<
            boost::bidirectionalS,
            vertex_descriptor> graphCSR;

    typedef pair<int, int> Pair;

    template<typename T>
    class Graph {

    private:
        Graph() = default;

        friend T;
    protected:
        /*** put here help algorithms**/
        int N;

        void
        parallel_count_adj(list<int> &l) {

            auto f = [&](int min, int max) {
                auto it = l.begin();
                std::advance(it, min);
                for (unsigned long i = min; i < max; i++) {
                    int j = 0;
                    auto neighbours = boost::adjacent_vertices(i, static_cast<T &>(*this).g);
                    for (auto vd: make_iterator_range(neighbours))j++;
                    l.insert(it, j);
                }
            };
            int step = static_cast<T &>(*this).N / 4;
            int min = 0;

            for (int i = 0; i < 4; i++) {
                if (i == 3 && static_cast<T &>(*this).N % 2 != 0) {
                    auto handle = async(std::launch::async, f, min, min + step + 1);
                } else auto handle = async(std::launch::async, f, min, min + step);
                min += step;
            }

        }

        int
        creatq(int n) {
            int q = 0;
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
        /*
        void
        for_each_neigh(unsigned long node,unsigned long n,std::function<void()> f){
            auto neighbours = boost::adjacent_vertices(node,static_cast<T&>(*this).g);
            for (auto vd : make_iterator_range(neighbours)) {
                n = vd;
                f();
            }
        }*/

    public:
        /*** put here algorithms ***/
        void
        sequential_algorithm();

        void
        parallel_sequential_algorithm();

        void
        printSol() {
            for (unsigned long i = 0; i < static_cast<T &>(*this).N; ++i)
                cout << static_cast<T &>(*this).g[i].color << endl;
        }

        void
        printGraph() {
            boost::print_graph(static_cast<T &>(*this).g);
        }

        void
        for_each_vertex(unsigned long node, std::function<void()> f) {
            return static_cast<T &>(*this).for_each_vertex(node, f);
        }

        void
        for_each_neigh(unsigned long node, unsigned long* n, std::function<void()> f) {
            return static_cast<T &>(*this).for_each_neigh(node, n, f);
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
        for_each_vertex(unsigned long node, std::function<void()> f);

        void
        for_each_neigh(unsigned long node, unsigned long* n,std::function<void()> f);
    };


    class GraphAdjList :
            public Graph<GraphAdjList> {
    private:
        friend Graph<GraphAdjList>;
        graphAdjList g;
    public:

        GraphAdjList(int N, vector<Pair> &edge_array);

        void
        for_each_vertex(unsigned long node, std::function<void()> f);

        void
        for_each_neigh(unsigned long node, unsigned long* n, std::function<void()> f);
    };

    class GraphAdjMatrix :
            public Graph<GraphAdjMatrix> {
    private:
        friend Graph<GraphAdjMatrix>;
        graphAdjMatrix g = graphAdjMatrix(0);
    public:

        GraphAdjMatrix(int N, vector<Pair> &edge_array);

        void
        for_each_vertex(unsigned long node, std::function<void()> f);

        void
        for_each_neigh(unsigned long node,unsigned long* n, std::function<void()> f);


    };
}
#endif //PDS_PROJECT_GRAPH_H
