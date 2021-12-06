//
// Created by voidjocker on 30/11/21.
//
#include "Graph.h"
using namespace std;

template<typename T>
void
my_graph::Graph<T>::sequential_algorithm() {

        /*************************************random permutation*************************************/
        srand(time(nullptr));
        std::unordered_set<int> rand_perm;
        while (rand_perm.size() != static_cast<T &>(*this).N) {
            int rand_vert = rand() % static_cast<T &>(*this).N;
            rand_perm.insert(rand_vert);
        }

        /***********************************MAIN LOOP************************************************/
        std::unordered_set<int> C;

        int last_used_color = 1; // colors are numerated starting from 1 and are always positive

        for (int i = 0; i < static_cast<T &>(*this).N && !exit_thread_flag; i++) {
            node vi = rand_perm.extract(*rand_perm.begin()).value();
            node n;
            for_each_neigh(vi, &n, [this, &n, &C]() {
                if (static_cast<T &>(*this).g[n].color != -1)
                    C.insert(static_cast<T &>(*this).g[n].color);
            });
            /*auto neighbours = boost::adjacent_vertices(vi, static_cast<T &>(*this).g);
            for (auto vd: make_iterator_range(neighbours)) {
                if (static_cast<T &>(*this).g[vd].color != -1)
                    C.insert(static_cast<T &>(*this).g[vd].color);
            }*/
            int c = 0;
            bool found = false;
            for (int i = 1; i <= last_used_color; i++) {
                if (C.count(i) == 0) {
                    c = i;
                    found = true;
                    break;
                }

            }
            if (!found)c = last_used_color;
            static_cast<T &>(*this).g[vi].color = c;
            last_used_color++;
            C.clear();
        }

    return;
}

template<typename T>
void
my_graph::Graph<T>::parallel_sequential_algorithm(){
    std::unordered_set<int> U, R;
    for (int i = 0; i < static_cast<T &>(*this).N; i++)U.insert(i);
    int colors[static_cast<T &>(*this).N];
    for (int i = 0; i < static_cast<T &>(*this).N; i++)
        colors[i] = -1;

    auto f = [&](int min, int max) {
        int cur_color = 1;
        for (unsigned long i = min; i < max; ++i) {
            std::unordered_set<int> c(cur_color);
            for (unsigned long j = 1; j <= cur_color; ++j)c.insert(j);
            auto neighbours = boost::adjacent_vertices(i, static_cast<T &>(*this).g);
            for (auto vd: make_iterator_range(neighbours))
                c.extract(colors[vd]);
            int color_tobe_assigned = *std::min_element(c.begin(), c.end());
            colors[i] = color_tobe_assigned;
            if (color_tobe_assigned > cur_color)cur_color = color_tobe_assigned;
        }
    };
    auto f1 = [&](int min, int max) {
        int cur_color = 1;
        for (unsigned long i = min; i < max; ++i) {
            auto neighbours = boost::adjacent_vertices(i, static_cast<T &>(*this).g);
            for (auto vd: make_iterator_range(neighbours))
                if (colors[vd] == colors[i])
                    R.insert(i);
        }
        U = R;
    };
    while (!U.empty()) {
        int step = static_cast<T &>(*this).N / 4;
        int min = 0;
        for (int i = 0; i < 4; i++) {
            if (i == 3 && static_cast<T &>(*this).N % 2 != 0) {
                auto handle = async(std::launch::async, f, min, min + step + 1);
            } else auto handle = async(std::launch::async, f, min, min + step);
            min += step;
        }
        //detect conflicts and create recoloring list
        min = 0;
        for (int i = 0; i < 4; i++) {
            if (i == 3 && static_cast<T &>(*this).N % 2 != 0) {
                auto handle = async(std::launch::async, f1, min, min + step + 1);
            } else auto handle = async(std::launch::async, f1, min, min + step);
            min += step;
        }
    }
    return;
}

template<typename T>
void
my_graph::Graph<T>::luby(){
    std::unordered_set<unsigned long> I;
    int q = my_graph::Graph<T>::creat_prime();
    int G = static_cast<T&>(*this).current_vertex_no;
    std::list<int> deg(static_cast<T&>(*this).N);
    while(G != 0){
        parallel_count_adj(deg);

    }
}
template
class my_graph::Graph<my_graph::GraphCSR>;

template
class my_graph::Graph<my_graph::GraphAdjMatrix>;

template
class my_graph::Graph<my_graph::GraphAdjList>;