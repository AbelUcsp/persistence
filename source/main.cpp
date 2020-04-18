// Copyright 2020 Roger Peralta Aranibar
#include <cassert>
#include <exception>
#include <iostream>
#include <memory>
#include <vector>

#include "DirectedGraph.hpp"
#include "PartialDirectedGraph.hpp"


int main() {

    ///se crear la estructura v 0 con p = 1
    ADE::Persistence::PartialDirectedGraph<int> my_graph(1, 2, 1);
    ADE::Persistence::PartialNode<int>* root_ptr = my_graph.get_root_ptr();

                                                        /// (value, position-1, version)
    ADE::Persistence::PartialNode<int>* root_v1 = my_graph.insert_root(10, 0, 1);
    ADE::Persistence::PartialNode<int>* root_v2 = my_graph.insert_root(100, 0, 2);

                                                        /// (value, nodo_actual, position-1)
    ADE::Persistence::PartialNode<int> *nodo_1_0 =  my_graph.insert_vertex(2, root_v2,    0);
    ADE::Persistence::PartialNode<int> *nodo_1_1 =  my_graph.insert_vertex(6, root_v2,    1);
    ADE::Persistence::PartialNode<int> *nodo_2_1 =  my_graph.insert_vertex(3, nodo_1_0,   1);

                                                        /// (value, nodo_actual, position-1, version)
    ADE::Persistence::PartialNode<int> *retrived_node3_0 =  my_graph.insert_vertex(37, nodo_2_1,    0, 3);

    /*** para navegar por una determianda version colocamos el root de la version deseada ******
    std::cout << "root_1 ->version_nodo "<< *(*root_ptr)[1][1][0].data_ << std::endl;

    std::cout << "root_ptr->version_nodo  "<< root_ptr->version_nodo << std::endl;
    std::cout << "root_v2->version_nodo  "<< root_v2->version_nodo << std::endl;
    */

                                            ///insertar valores TABAL_MOD (value, nodo_origen)
    ADE::Persistence::PartialNode<int> *INSERT_ = my_graph.insert_value(11, root_v2);
    ADE::Persistence::PartialNode<int> *INSERT_2 = my_graph.insert_value(22, root_v2);
    ADE::Persistence::PartialNode<int> *INSERT_3 = my_graph.insert_value(33, root_v2); ///error tabal llena

    ///( my_graph.current_version - 1 )
    //my_graph.get_root_ptr(2);

    std::cout <<  std::endl << "CURRENT VERSION  "<< my_graph.current_version << std::endl;



  return 0;
}
