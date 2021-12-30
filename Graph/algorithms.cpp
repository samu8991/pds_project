//
// Created by voidjocker on 30/11/21.
//
#include "Graph.h"
#include <cassert>
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
            for (int j = 1; j <= last_used_color; j++) {
                if (C.count(j) == 0) {
                    c = j;
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

/*template<typename T>
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
*/
void stampaSet(std::unordered_set<my_graph::node>& I){
    cout << "Stampo insieme >> ";
    for(auto it = I.begin(); it!=I.end(); ++it)
        cout << *it << " ";
    cout << '\n';
}
template<typename T>
void
my_graph::Graph<T>::stampaGrafo(){
    cout << "Proprietà grafo: ";
    for(my_graph::node i = 0; i < static_cast<T&>(*this).N;i++ )
        cout << static_cast<T&>(*this).g[i].isDeleted << " ";
    cout << '\n';
}
template<typename T>
void
my_graph::Graph<T>::luby()
{

    std::unordered_set<node> I;
    int current_color = 0;
    int BigC = static_cast<T&>(*this).N;
    int cvn = static_cast<T&>(*this).N;
    std::vector<std::thread> threads(static_cast<T&>(*this).threadAvailable);
    int step = static_cast<T&>(*this).N/static_cast<T&>(*this).threadAvailable;step--;
    int k = 0;
    function<void(int,int)> core = [this,&I,&cvn](int start, int end){
        bool go = true;

        while(1) {
            //cout << "Thread "  << start << " - " << end << endl;
            //cout << "CVN " << cvn << endl;
            /*if(cvn == 0){
                //cout << "Ci siamo di sopra " << endl;
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
                uniqueLock.unlock();
                break;
            }*/

            std::unordered_set<node> S;
            std::unique_lock<mutex> uniqueLock(m);
            generate_random_distr(S, start, end);

            if(S.size() == 0){
                //cout << "Prima" << endl;
                //stampaGrafo();
                uniqueLock.unlock();
                break;
            }
            //cout << "S1: ";
            //stampaSet(S);

            Pair e;
            for_each_edge(&e, [&S,&e,this,&uniqueLock]() {

                if (S.find(e.first) != S.end() && S.find(e.second) != S.end()/*&&
                !static_cast<T&>(*this).g[e.first].isDeleted && !static_cast<T&>(*this).g[e.second].isDeleted*/) {

                    if (degree_induced_graph(e.first) < degree_induced_graph(e.second))
                        S.erase(e.first);
                    else
                        S.erase(e.second);

                }

            });
            //cout << "S2: ";
            //stampaSet(S);
            for(auto it = S.begin(); it != S.end(); ++it){
                //cout << "I1: ";
                I.insert(*it);
                //stampaSet(I);
                if(!static_cast<T&>(*this).g[*it].isDeleted){
                    static_cast<T&>(*this).g[*it].isDeleted = true;
                    cvn--;
                    //cout << "CVN1 " << cvn << endl;
                    //stampaGrafo();
                    assert(cvn >= 0);

                }
                /*cout << "Seconda " << endl;
                stampaGrafo();*/
                node nei;
                for_each_neigh(*it,&nei,[this,&nei,&cvn]() {
                    if (!static_cast<T &>(*this).g[nei].isDeleted ){
                        static_cast<T &>(*this).g[nei].isDeleted = true;
                        cvn--;
                        //cout << "CVN2 " << cvn << endl;
                        //stampaGrafo();
                        assert(cvn >= 0);
                    }
                });
            }
            if(cvn<=0) {
                //cout << "ci siamo " << endl;
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
                uniqueLock.unlock();
                break;
            }
            uniqueLock.unlock();
        }

    };
    while(BigC != 0){
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

        // Coloring nodes in I
        /*cout << "Ora coloro ";
        stampaSet(I);*/

        //TODO questa operazione si può fare in paralello
        for(node node: I){
            static_cast<T&>(*this).g[node].color = current_color;
        }
        current_color++;
        node curr;
        BigC = static_cast<T&>(*this).N;
        //TODO questa operazione si può fare in paralello
        for_each_vertex(&curr,[this,&curr,&BigC]{
           if(static_cast<T&>(*this).g[curr].color == -1){
               static_cast<T&>(*this).g[curr].isDeleted = false;
           }//non trovato
           else BigC--;
        });

        cout << "BigC >> " << BigC << endl;
        //stampaGrafo();
        cvn = BigC;
        I.clear();
    }
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


    auto f = [this,&neigh,&sizeOfI](int min, int max, int i) {

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
}

template
class my_graph::Graph<my_graph::GraphCSR>;

template
class my_graph::Graph<my_graph::GraphAdjMatrix>;

template
class my_graph::Graph<my_graph::GraphAdjList>;