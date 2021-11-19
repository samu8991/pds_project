//
// Created by voidjocker on 19/11/21.
//

#ifndef PDS_PROJECT_GRAPH_H
#define PDS_PROJECT_GRAPH_H

#include <boost/graph/compressed_sparse_row_graph.hpp>
#include <boost/graph/adjacency_matrix.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/property_map/property_map.hpp>
#include <future>

using namespace std;
using namespace boost;
typedef boost::property<boost::vertex_color_t,int> colorProperty;
struct vertexDescriptor{int color;};
typedef boost::adjacency_list<boost::vecS,boost::vecS,
        boost::undirectedS,
        vertexDescriptor> graphAdjList;

typedef boost::adjacency_matrix<boost::undirectedS,
        vertexDescriptor> graphAdjMatrix;

typedef boost::compressed_sparse_row_graph<
        boost::bidirectionalS,
        vertexDescriptor> graphCSR;

template <typename T>
class Graph{

private:
    Graph(){}
    friend T;

protected:
    /*** put here help algorithms**/


public:
    /*** put here algorithms ***/

    int&
    template_sequential_algorithm()
    {
        std::unordered_set<int> U,R;
        for(int i = 0; i < static_cast<T&>(*this).g.N; i++)U.insert(i);
        int colors[static_cast<T&>(*this).g.N];
        for(int i = 0; i < static_cast<T&>(*this).g.N; i++)
            colors[i] = -1;

        auto f = [&](int min,int max){
            int cur_color = 1;
            for(int i = min; i < max; ++i){
                std::unordered_set<int> c(cur_color);
                for(int j = 1; j <= cur_color;++j)c.insert(j);
                auto neighbours = boost::adjacent_vertices(i,static_cast<T&>(*this).g);
                for (auto vd : make_iterator_range(neighbours))
                    c.extract(colors[vd]);
                int color_tobe_assigned = *std::min_element(c.begin(), c.end());
                colors[i] = color_tobe_assigned;
                if(color_tobe_assigned > cur_color)cur_color = color_tobe_assigned;
            }
        };
        auto f1 = [&](int min,int max){
            int cur_color = 1;
            for(int i = min; i < max; ++i){
                auto neighbours = boost::adjacent_vertices(i,static_cast<T&>(*this).g.N);
                for (auto vd : make_iterator_range(neighbours))
                    if(colors[vd] == colors[i])
                        R.insert(i);
            }
            U = R;
        };
        while(!U.empty()) {
            int step = static_cast<T&>(*this).g.N / 4;
            int min = 0;
            for (int i = 0; i < 4; i++) {
                if (i == 3 && static_cast<T&>(*this).g.N % 2 != 0) {
                    auto handle = async(std::launch::async, f, min, min + step + 1);
                } else auto handle = async(std::launch::async, f, min, min + step);
                min += step;
            }
            //detect conflicts and create recoloring list
            min = 0;
            for (int i = 0; i < 4; i++) {
                if (i == 3 && static_cast<T&>(*this).g.N % 2 != 0) {
                    auto handle = async(std::launch::async, f1, min, min + step + 1);
                } else auto handle = async(std::launch::async, f1, min, min + step);
                min += step;
            }
        }
        return colors;
    }

};

class GraphCSR:
        public Graph<GraphCSR>{
private:
    friend Graph<GraphCSR>;
    graphCSR g;
    int N;
public:

    GraphCSR (int N):N(N){
        g = graphCSR (N);
    }
};


class GraphAdjList:
        public Graph<GraphAdjList>{
private:
    friend Graph<GraphAdjList>;
    graphAdjList g;
    int N;
public:

    GraphAdjList (int N):N(N){
        g = graphAdjList (N);
    }
};

class GraphAdjMatrix:
        public Graph<GraphAdjMatrix>{
private:
    friend Graph<GraphAdjMatrix>;
    graphAdjMatrix g = graphAdjMatrix (0);
    int N;
public:

    GraphAdjMatrix(int N) : N(N) {
        g = graphAdjMatrix (N);
    }


};
#endif //PDS_PRJ_GRAPH_H
