//
// Created by voidjocker on 30/11/21.
//
#include "Graph.h"
#include <assert.h>
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
        for (node i = min; i < max; ++i) {
            std::unordered_set<int> c(cur_color);
            for (node j = 1; j <= cur_color; ++j)c.insert(j);
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
        for (node i = min; i < max; ++i) {
            auto neighbours = boost::adjacent_vertices(i, static_cast<T &>(*this).g);
            for (auto vd: make_iterator_range(neighbours))
                if (colors[vd] == colors[i])
                    R.insert(i);
        }
        U = R;
    };
    while (!U.empty()) {
        int step = static_cast<T &>(*this).N / 4;
        int difference = static_cast<T &>(*this).N % 4;
        int min = 0;
        for (int i = 0; i < 4; i++) {
            if (i == 3 && difference != 0) {
                auto handle = async(std::launch::async, f, min, min + step + difference);
            } else auto handle = async(std::launch::async, f, min, min + step);
            min += step;
        }
        //detect conflicts and create recoloring list
        min = 0;
        for (int i = 0; i < 4; i++) {
            if (i == 3 && difference != 0) {
                auto handle = async(std::launch::async, f1, min, min + step + difference);
            } else auto handle = async(std::launch::async, f1, min, min + step);
            min += step;
        }
    }
    return;
}

template<typename T>
void
my_graph::Graph<T>::luby() {
    std::unordered_set<node> I;
    int q = my_graph::Graph<T>::creat_prime();
    int current_color = 0;
    int current_colored = 0;
    std::vector<int> deg(static_cast<T &>(*this).N);
label:
    while (static_cast<T &>(*this).current_vertex_no != 0) {
        parallel_count_adj(deg);
        second_phase_luby(I, deg, q);
        cout << "Current vertex number>> " << static_cast<T &>(*this).current_vertex_no << endl;
    }
    assert(static_cast<T &>(*this).current_vertex_no == 0);
    for (auto node: I) {
        static_cast<T &>(*this).g[node].color = current_color;
        current_colored++;
    }
    current_color++;
    if (current_colored != static_cast<T &>(*this).N) {
        node n;
        I.clear();
        for_each_vertex(&n, [this,n]() {
            if (static_cast<T &>(*this).g[n].color == -1) {
                static_cast<T &>(*this).g[n].isDeleted = false;
                static_cast<T &>(*this).current_vertex_no++;
            }
        });
        goto label;
    }
    assert(current_colored == static_cast<T &>(*this).N);
    printSol();
}

template<typename T>
void my_graph::Graph<T>::jones_plassmann() {
    // U := V    but in this case I'm interested only in the size, so I save the number of vertices N
    auto U = static_cast<T &>(*this).N;
    node neigh;

    // definisco sizeOfI dove andrò a salvare la size di I di ogni esecuzione parallela
    // in modo da poter diminuire la size di U alla fine del while
    node sizeOfI[4];


    auto f = [&](int min, int max, int i) {

        // I := { v such that w(v)>w(u) for all neighbors u in U }
        std::unordered_set<node> I;

        for (node vertex = min; vertex < max; ++vertex) {
            // utilizzo un bool per indicare se il nodo ha weight maggiore di tutti i suoi vicini, in modo da aggiungerlo poi in I e colorarlo
            bool majorThanNeigh = true;


            if (static_cast<T &>(*this).g[vertex].color != -1) majorThanNeigh = false;
            else {
                for_each_neigh(vertex, &neigh, [this, &neigh, vertex, &majorThanNeigh]() {
                    // Controllo se il nodo è già colorato oppure se ha weights minore di uno dei suoi vicini; in questo caso non devo inserirlo in I
                    if (static_cast<T &>(*this).g[neigh].color == -1 &&
                        (static_cast<T &>(*this).g[vertex].weight < static_cast<T &>(*this).g[neigh].weight))
                        majorThanNeigh = false;
                });
            }
           if (majorThanNeigh) I.insert(vertex);
        }

        // for all vertices v' in I do in parallel
        for(const auto &elem : I) {
            // considero 0 come colore minore
            int finalColor = 0;
            std::set<int> colors;
            // cerco il colore più piccolo non usato dai vicini del nodo che sto esaminando
            for_each_neigh(elem, &neigh, [this, &neigh, &colors]() {
                if(static_cast<T &>(* this).g[neigh].color != -1)
                    colors.insert(static_cast<T &>(* this).g[neigh].color);
            });

            // scorro tutti i colori del set finchè o non trovo un colore intermedio che manca -> colore minore da assegnare al nodo
            // oppure scorro tutti i colori e assegno al nodo il colore max(colors)+1
            for(auto it = colors.begin(); it != colors.end(); ++it, ++finalColor) {
                if(*it != finalColor) break;
            }
            // c(v') := minimum color not in S
            static_cast<T &>(* this).g[elem].color = finalColor;
        }

        sizeOfI[i] = I.size();
    };



    // while (|U|>0) do
    while (U > 0) {

        int step = static_cast<T &>(*this).N / 4;
        int difference = static_cast<T &>(*this).N % 4;
        int min = 0;

        // for all vertices v in U do in parallel
        for (int i = 0; i < 4; i++) {
            if (i == 3 && difference != 0) {
                auto handle = async(std::launch::async, f, min, min + step + difference, i);
            } else auto handle = async(std::launch::async, f, min, min + step, i);
            min += step;
        }

        for(node & elem : sizeOfI) {
            // U := U - I
            U -= elem;

            // la riazzero per la prossima iterazione
            elem=0;
        }

    }
}

template
class my_graph::Graph<my_graph::GraphCSR>;

template
class my_graph::Graph<my_graph::GraphAdjMatrix>;

template
class my_graph::Graph<my_graph::GraphAdjList>;