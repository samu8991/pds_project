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

struct vertexDescriptor{int color;};

typedef boost::adjacency_list<boost::vecS,boost::vecS,
        boost::undirectedS,
        vertexDescriptor> graphAdjList;

typedef boost::adjacency_matrix<boost::undirectedS,
        vertexDescriptor> graphAdjMatrix;

typedef boost::compressed_sparse_row_graph<
        boost::bidirectionalS,
        vertexDescriptor> graphCSR;

typedef pair<int,int> Pair;

template <typename T>
class Graph{

private:
    Graph()= default;
    friend T;
    int N;
protected:
    /*** put here help algorithms**/
    void
    parallel_count_adj(list<int>& l){

        auto f = [&](int min,int max){
            auto it = l.begin();
            std::advance(it,min);
            for(int i = min; i < max; i++){
                int j = 0;
                auto neighbours = boost::adjacent_vertices(i,static_cast<T&>(*this).g);
                for (auto vd : make_iterator_range(neighbours))j++;
                l.insert(it,j);
            }
        };
        int step = static_cast<T&>(*this).g.m_vertices.size()/4;
        int min = 0;

        for(int i = 0; i < 4; i++){
            if(i == 3 && static_cast<T&>(*this).g.m_vertices.size()%2 != 0){
                auto handle = async(std::launch::async,f,min,min+step+1);
            }
            else auto handle = async(std::launch::async,f,min,min+step);
            min += step;
        }

    }
    int
    creatq(int n){
        int q = 0;
        for(int i=n; i<= 2*n; i++) {
            if((i%2==0)||(i%3==0)||(i%5==0)||(i%7==0)||(i%9==0)){
                continue;
            } else {
                q=i;
                break;
            }
        }
        return q;
    }

public:
    /*** put here algorithms ***/
    int&
    sequential_algorithm(){

        int colors[static_cast<T&>(*this).N];

        /*************************************random permutation*************************************/
        srand(time(nullptr));
        std::unordered_set<int> rand_perm;
        while(rand_perm.size() != static_cast<T&>(*this).N){
            int rand_vert = rand()%static_cast<T&>(*this).N;
            rand_perm.insert(rand_vert);
        }

        /***********************************MAIN LOOP************************************************/
        std::unordered_set<int> U(static_cast<T&>(*this).N);std::unordered_set<int> C;

        int last_used_color = 1; // colors are numerated starting from 1 and are always positive

        for(int i = 0; i < static_cast<T&>(*this).N; i++)U.insert(i);

        for(int i = 0; i < static_cast<T&>(*this).N; i++){
            int vi = rand_perm.extract(*rand_perm.begin()).value();
            auto neighbours = boost::adjacent_vertices(vi,static_cast<T&>(*this).g);
            for (auto vd : make_iterator_range(neighbours)) {
                if (colors[vd] != 0)
                    C.insert(colors[vd]);
            }
            int c = 0;
            bool found = false;
            for(int i = 1; i <= last_used_color; i++ ) {
                if (C.count(i) == 0) {
                    c = i;
                    found = true;
                    break;
                }

            }
            if(!found) c = last_used_color;
            colors[vi] = c;
            last_used_color++;
            U.extract(vi);
            C.clear();
        }
        return *colors;
    }
    int&
    parallel_sequential_algorithm()
    {
        std::unordered_set<int> U,R;
        for(int i = 0; i < static_cast<T&>(*this).N; i++)U.insert(i);
        int colors[static_cast<T&>(*this).N];
        for(int i = 0; i < static_cast<T&>(*this).N; i++)
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
            int step = static_cast<T&>(*this).N / 4;
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
                if (i == 3 && static_cast<T&>(*this).N % 2 != 0) {
                    auto handle = async(std::launch::async, f1, min, min + step + 1);
                } else auto handle = async(std::launch::async, f1, min, min + step);
                min += step;
            }
        }
        return colors;
    }
    int&
    luby(){

        std::unordered_set<int> I;
        int colors[static_cast<T&>(*this).N];
        int n = static_cast<T&>(*this).N;
        int q = creatq(n);
        list<int> adj_vrtx;
        while(static_cast<T&>(*this).g.vertex_set().size() != 0){
            parallel_count_adj(static_cast<T&>(*this).g,adj_vrtx);

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

    GraphAdjList (int N,vector<Pair>& edge_array):N(N){
        g = graphAdjList (N);
        for (int i = 0; i < 8; ++i)
            add_edge(edge_array[i].first, edge_array[i].second, g);
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
