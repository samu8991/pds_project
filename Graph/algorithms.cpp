//
// Created by voidjocker on 30/11/21.
//
#include "Graph.h"
#include <cassert>
#define DEBUG 0

using namespace std;


template<typename T>
void
my_graph::Graph<T>::sequential_algorithm() {

        /*************************************random permutation*************************************/
        // choose a random permutation p(1), ... , p(n) of numbers 1, ..., n
        srand(time(nullptr));
        std::unordered_set<int> rand_perm;
        while (rand_perm.size() != static_cast<T &>(*this).N) {
            int rand_vert = rand() % static_cast<T &>(*this).N;
            rand_perm.insert(rand_vert);
        }

        /***********************************MAIN LOOP************************************************/
        // C := {colors of all colored neighbors of the selected vertex}
        std::unordered_set<int> C;

        int last_used_color = -1;

        // for i to n do in sequence
        for (int i = 0; i < static_cast<T &>(*this).N && !exit_thread_flag; i++) {
            // v := p(i)
            node vi = rand_perm.extract(*rand_perm.begin()).value();
            node n;
            for_each_neigh(vi, &n, [this, &n, &C]() {
                if (static_cast<T &>(*this).g[n].color != -1)
                    C.insert(static_cast<T &>(*this).g[n].color);
            });

            int c = -1;
            bool found = false;
            for (int j = 0; j <= last_used_color; j++) {
                if (C.count(j) == 0) {
                    c = j;
                    found = true;
                    break;
                }

            }
            if (!found)c = last_used_color+1;
            // c(v) := {smallest color not in C}
            static_cast<T &>(*this).g[vi].color = c;
            last_used_color++;
            C.clear();
        }

    return;
}

template<typename T>
void
my_graph::Graph<T>::parallel_sequential_algorithm(){
    int last_used_color = -1;
    function<void(node,node)> core = [this,&last_used_color](node start,node end){
        // choose a random permutation of nodes p(i)
        srand(time(nullptr));
        std::unordered_set<int> rand_perm;
        node curr = start;
        while (curr <= end) {
            rand_perm.insert(curr);
            curr++;
        }
        // C := {colors of all colored neighbors of the selected vertex}
        std::unordered_set<int> C;
        // for all nodes
        for(node i = start; i <= end; ++i){
            // v := p(i)
            node vi = rand_perm.extract(*rand_perm.begin()).value();
            node n;
            unique_lock<std::mutex> uniqueLock(m);

            for_each_neigh(vi, &n, [this, &n, &C]() {
                if (static_cast<T &>(*this).g[n].color != -1){
                    C.insert(static_cast<T &>(*this).g[n].color);
                }
            });
            uniqueLock.unlock();
            int c = -1;
            bool found = false;
            uniqueLock.lock();

            // c(v):= smallest color not in C
            for (int j = 0; j <= last_used_color+1; j++) {
                if (C.count(j) == 0) {
                    c = j;
                    found = true;
                    break;
                }

            }
            if (!found)c = last_used_color+1;
            static_cast<T &>(*this).g[vi].color = c;
            last_used_color++;
            uniqueLock.unlock();
            C.clear();
        }

        return;
    };
    int k = 0;
    std::vector<std::thread> threads(static_cast<T&>(*this).threadAvailable);
    int step = static_cast<T&>(*this).N/static_cast<T&>(*this).threadAvailable;
    step--;
    for (int j= 0; j < static_cast<T&>(*this).threadAvailable; ++j) {
        threads.emplace_back(core,k,k+step);
        k+=(step+1);
    }
    for(auto& t:threads){
        if(t.joinable())
            t.join();
    }
    threads.clear();
#if DEBUG == 1
    printSolCorrectness();
#endif

    return;
}



template<typename T>
void
my_graph::Graph<T>::luby()
{
    // I <- empty
    std::unordered_set<node> I;
    int current_color = 0;
    // G'= (V',E') <- G= (V,E)
    int BigC = static_cast<T&>(*this).N;
    int cvn = static_cast<T&>(*this).N;
    std::vector<std::thread> threads(static_cast<T&>(*this).threadAvailable);
    int step = static_cast<T&>(*this).N/static_cast<T&>(*this).threadAvailable;step--;
    int k = 0;
    function<void(int,int)> core = [this,&I,&cvn](int start, int end){

        while(1) {
            // X = empty
            std::unordered_set<node> S;

            // randomly choose to add v to X
            std::unique_lock<mutex> uniqueLock(m);
            generate_random_distr(S, start, end);

            if(S.size() == 0){
                uniqueLock.unlock();
                break;
            }
            Pair e;
            // alla fine nodi che sono vicini non saranno entrambi nell'insieme S
            // for all v in X, w in X
            for_each_edge(&e, [&S,&e,this]() {
                // if (v,w) in E' then
                if (S.find(e.first) != S.end() && S.find(e.second) != S.end()) {
                    // if d(v) <= d(w) then
                    if (degree_induced_graph(e.first) < degree_induced_graph(e.second))
                        // I' <- I' - {v}
                        S.erase(e.first);
                    else
                        // else I' <- I' - {w}
                        S.erase(e.second);

                }

            });

            for(auto it = S.begin(); it != S.end(); ++it){
                I.insert(*it);
                if(!static_cast<T&>(*this).g[*it].isDeleted){
                    static_cast<T&>(*this).g[*it].isDeleted = true;
                    cvn--;
                    assert(cvn >= 0);
                }
                node nei;
                for_each_neigh(*it,&nei,[this,&nei,&cvn]() {
                    if (!static_cast<T &>(*this).g[nei].isDeleted ){
                        static_cast<T &>(*this).g[nei].isDeleted = true;
                        cvn--;
                        assert(cvn >= 0);
                    }
                });
            }
            if(cvn<=0) {
            #if DEBUG == 1
                assert(([this]()->bool{
                    node curr;
                    bool ret = true;
                    for_each_vertex(&curr,[this,&curr,&ret]()->void{
                        if(!static_cast<T&>(*this).g[curr].isDeleted){
                            ret= false;
                            return;
                        }
                    });
                    return ret;
                })());
            #endif
                uniqueLock.unlock();
                break;
            }
            uniqueLock.unlock();
        }

    };
    // while V' is not empty
    while(BigC != 0){
        #if DEBUG == 1
        cout << BigC << endl;
        #endif
        k = 0;
        for (int j= 0; j < static_cast<T&>(*this).threadAvailable; ++j) {
            threads.emplace_back(core,k,k+step);
            k+=(step+1);
        }
        for(auto& t:threads){
            if(t.joinable())
                t.join();
        }
        threads.clear();
        // color all vertices in the MIS and remove them from G'
        for(node node: I)
            static_cast<T&>(*this).g[node].color = current_color;

        current_color++;
        node curr;
        BigC = static_cast<T&>(*this).N;

        for_each_vertex(&curr,[this,&curr,&BigC]{
            if(static_cast<T&>(*this).g[curr].color == -1){
                static_cast<T&>(*this).g[curr].isDeleted = false;
            }//non trovato
            else BigC--;
        });

        cvn = BigC;
        I.clear();
    }
#if DEBUG == 1
    printSolCorrectness();
#endif
return;
}

template<typename T>
void my_graph::Graph<T>::jones_plassmann() {
    // U := V    but in this case I'm interested only in the size, so I save the number of vertices N
    auto U = static_cast<T &>(*this).N;
    node neigh;

    // definisco sizeOfI dove andr?? a salvare la size di I di ogni esecuzione parallela
    // in modo da poter diminuire la size di U alla fine del while
    node sizeOfI[4];


    auto f = [this,&neigh,&sizeOfI](int min, int max, int i) {

        // I := { v such that w(v)>w(u) for all neighbors u in U }
        std::unordered_set<node> I;

        for (node vertex = min; vertex < max; ++vertex) {
            // utilizzo un bool per indicare se il nodo ha weight maggiore di tutti i suoi vicini, in modo da aggiungerlo poi in I e colorarlo
            bool majorThanNeigh = true;


            if (static_cast<T &>(*this).g[vertex].color != -1) majorThanNeigh = false;
            else {
                for_each_neigh(vertex, &neigh, [this, &neigh, vertex, &majorThanNeigh]() {
                    // Controllo se il nodo ?? gi?? colorato oppure se ha weights minore di uno dei suoi vicini; in questo caso non devo inserirlo in I
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
            // cerco il colore pi?? piccolo non usato dai vicini del nodo che sto esaminando
            for_each_neigh(elem, &neigh, [this, &neigh, &colors]() {
                if(static_cast<T &>(* this).g[neigh].color != -1)
                    colors.insert(static_cast<T &>(* this).g[neigh].color);
            });

            // scorro tutti i colori del set finch?? o non trovo un colore intermedio che manca -> colore minore da assegnare al nodo
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
            if (i == 3 && difference != 0){
                 auto handle = async(std::launch::async, f, min, min + step + difference, i);
            }
            else{
                auto handle = async(std::launch::async, f, min, min + step, i);
            }

            min += step;
        }

        for(node & elem : sizeOfI) {
            // U := U - I
            U -= elem;

            // la riazzero per la prossima iterazione
            elem=0;
        }

    }
#if DEBUG == 1
    printSolCorrectness();
#endif
    return;
}

template
class my_graph::Graph<my_graph::GraphCSR>;

template
class my_graph::Graph<my_graph::GraphAdjMatrix>;

template
class my_graph::Graph<my_graph::GraphAdjList>;
