// Copyright 2020 Roger Peralta Aranibar
#ifndef SOURCE_PARTIALDIRECTEDGRAPH_HPP_
#define SOURCE_PARTIALDIRECTEDGRAPH_HPP_

#include <utility>

#include "DirectedGraph.hpp"
namespace ADE {
namespace Persistence {

template <typename Type>
class PartialNode;

template <typename Type, typename Node = PartialNode<Type>>
class TableMods{
public:
    int version;
    int tamanho;
    int indice;
    std::pair<Type, Node*> *table;

    TableMods(){}
    TableMods(int cant){
        table = new std::pair<Type, Node*>[cant];
        tamanho = cant;
        indice = 0;
        for(int i=0; i<cant; ++i)
            table[i] = std::pair<Type, Node*>(0, nullptr);
    }

    setValue(Type const data){
        table[indice].first = data;
        indice++;
        tamanho--;
    }

    setPointer(Node* u){
        table[indice].second = u;
        indice++;
        tamanho--;
    }

    bool size_(){
        return tamanho>0;
    }

};

template <typename Type>
class PartialNode : public Node<Type> {
 public:
  typedef Type data_type;
  std::size_t in_ptrs_size;
  PartialNode *back_pointer;
  TableMods<Type> mods;
  unsigned int version_nodo;

  PartialNode(data_type data, std::size_t const& out_ptrs_size,
              std::size_t const& in_ptrs_size=3)
      : Node<Type>(data, out_ptrs_size)
        {
            this->in_ptrs_size = in_ptrs_size;
            back_pointer = nullptr;
        }

  void set_in_ptrs(std::size_t const& in_ptrs_size_){ ///add
      mods = TableMods<Type>(static_cast<int>(in_ptrs_size_) + static_cast<int>(in_ptrs_size_));
      in_ptrs_size=in_ptrs_size_;}




  data_type get_data() { return Node<Type>::data_; }

  data_type get_data(unsigned int version) { return *Node<Type>::data_; }

  bool set_data(data_type const data) {
    *Node<Type>::data_ = data;
    return true;
  }

  bool set_ptr(PartialNode* ptr, unsigned int id) {
    Node<Type>::forward_ = ptr;
    return true;
  }


  PartialNode& operator[](std::size_t id) const {
    return *dynamic_cast< PartialNode* >( &(Node<Type>::operator[](id)) );
  }

};

template <typename Type, typename Node = PartialNode<Type>>
class PartialDirectedGraph : public DirectedGraph<Type, Node> {
 public:
  typedef Type data_type;

  PartialDirectedGraph(data_type const data, std::size_t const& out_ptrs_size,
                       std::size_t const& in_ptrs_size)
      : DirectedGraph<Type, Node>(data, out_ptrs_size),
        in_ptrs_size_(in_ptrs_size),
        current_version(0)
  { DirectedGraph<Type, Node>::root_ptr_->set_in_ptrs(in_ptrs_size);
        versiones = new Node*[20];
        versiones[0] = DirectedGraph<Type, Node>::root_ptr_;
        dynamic_cast< Node* >(DirectedGraph<Type, Node>::root_ptr_)->version_nodo = 0;
  }

  Node* get_root_ptr() {
      Node* current = versiones[0];
      std::cout << "Root V_0 DATA : "<< *current->data_ << std::endl;
      for(int i=0; i<(2*in_ptrs_size_) ;++i){
        std::cout << current->mods.table[i].first << "  " << current->mods.table[i].second  << std::endl;
      }

    return DirectedGraph<Type, Node>::get_root_ptr();
  }

  Node* get_root_ptr(unsigned int version) {
      Node* current = versiones[version];
      std::cout << "Root V_"<< version<<" DATA : "<< *current->data_ << std::endl;
      for(int i=0; i<(2*in_ptrs_size_) ;++i){
        std::cout << current->mods.table[i].first << "  " << current->mods.table[i].second  << std::endl;
      }
   return current;
  }

  /*** INSERT ROOT VERSION*/
  Node* insert_root(data_type const data, std::size_t position, unsigned int version) {
      if( version == versiones[version-1]->version_nodo + 1 ){
          ++current_version;
          PartialNode<Type>* tmp = dynamic_cast< Node* >(DirectedGraph<Type, Node>::root_ptr_);
          PartialNode<Type>* ROOT = new PartialNode<Type>( data, tmp->out_ptrs_size_, tmp->in_ptrs_size);
          versiones[version] =  ROOT;
          ROOT->version_nodo = current_version;
          DirectedGraph<Type, Node>::root_ptr_ = ROOT;
          ROOT->set_in_ptrs(tmp->in_ptrs_size);
          return ROOT;
      }
      else{
            std::cout <<"ERROR version invalida "  << data << std::endl;
            return nullptr;
      }
  }


  /***
  Ingresa valor a un nodo existente o crea otro nodo
  Ademas Imprime toda la tabla de modificaciones para comprobar
  */
  Node* insert_value(data_type const data, Node* origen) {

    if(origen == DirectedGraph<Type, Node>::root_ptr_ && origen->mods.size_() ){ ///caso ROOT

        origen->mods.setValue(data);
        origen->mods.version = origen->version_nodo;

        std::cout <<  "Datos TablaMod del Root->Valor "  << *origen->data_ << std::endl;
        for(int i=0; i<2; ++i)
            std::cout  << origen->mods.table[i].first << std::endl;


    }
    else if( origen->back_pointer != nullptr && origen->mods.size_() ){
        origen->mods.setValue(data);
        origen->mods.version = origen->version_nodo;
        std::cout <<  "Datos TablaMod del Nodo->Valor "  << *origen->data_ << std::endl;
        for(int i=0; i<2; ++i)
            std::cout  << origen->mods.table[i].first << std::endl;
    }
    else{
        std::cout <<  "Tabla Mofidicaciones llena o no valido "  << data << std::endl;
    }
  }


    /// Insertar solo vertices, para el root existe su funcion especifica
  Node* insert_vertex(data_type const data, Node* u, std::size_t position) {

      if( u->forward_[position] == nullptr && position<u->out_ptrs_size_ && u == DirectedGraph<Type, Node>::root_ptr_ ){
            PartialNode<Type>* tmp = DirectedGraph<Type, Node>::insert_vertex(data, u, position);
            tmp->set_in_ptrs( in_ptrs_size_);
            tmp->mods.version = current_version;
            tmp->version_nodo = current_version;
            tmp->back_pointer = u;
            if( true){
                for(size_t i=0; i< current_version ; ++i ){///actualziar forward ROOOT
                    PartialNode<Type>* ROOT_V = versiones[i];
                    for(size_t j=0; j< DirectedGraph<Type, Node>::out_ptrs_size_ ; ++j ){
                        if( DirectedGraph<Type, Node>::root_ptr_->forward_[j] != nullptr ){ ///si tiene datos, actualizar
                            ROOT_V->forward_[j] = DirectedGraph<Type, Node>::root_ptr_->forward_[j];
                        }
                    }
                }
            }
         return tmp;
      }
      else if(  u->forward_[position] == nullptr && position<u->out_ptrs_size_ && u != DirectedGraph<Type, Node>::root_ptr_  ){ ///actualizar forward_NO_ROOT si existe
            PartialNode<Type>* tmp = DirectedGraph<Type, Node>::insert_vertex(data, u, position);

            tmp->set_in_ptrs( in_ptrs_size_);
            tmp->mods.version = current_version;
            tmp->version_nodo = current_version;
            tmp->back_pointer = u;

            for(size_t i=0; i< DirectedGraph<Type, Node>::out_ptrs_size_ ; ++i ){
                if( u->back_pointer->forward_[i] != nullptr ){ ///si tiene datos, actualizar
                    u->back_pointer->forward_[i]->forward_[position] = tmp;
                }
            }
            return tmp;
      }
      else{
            std::cout << "ERROR Posicion ya ocupada o fuera de rango(out_ptrs_size) o ROOT incorrecto " << data << std::endl;
            return nullptr;
      }
  }///end no version


  /// Solo vertex
  Node* insert_vertex(data_type const data, Node* u, std::size_t position, unsigned int version) {
    if( !u->forward_[position] && version == current_version+1 && position<u->out_ptrs_size_ )
    {
        ++current_version;
        return insert_vertex( data,  u,  position);
    }
    else if( !u->forward_[position] && version == current_version && position < u->out_ptrs_size_  ){ ///NUEVO NODO-no-testeado
        if( ! u->forward_[position] ){
            PartialNode<Type>* tmp = DirectedGraph<Type, Node>::insert_vertex(data, u, position);
            tmp->set_in_ptrs( in_ptrs_size_);
            tmp->mods.version = current_version;
            tmp->version_nodo = current_version;
            tmp->back_pointer = u;

            return tmp;
        }
    }
    else{
        std::cout<< " ERROR No se permite ingresar en versiones inferiores a la actual o posicion incorrecta " << data << std::endl;
        return nullptr;
    }
  }///END INSERT_VERTEX




  void add_edge(Node* u, Node* v, std::size_t position) {
    ++current_version;
    return add_edge(u, v, position, current_version);
  }

 private:


  void add_edge(Node* u, Node* v, std::size_t position, unsigned int version) {
    return DirectedGraph<Type, Node>::add_edge(u, v, position);
  }

    public:
  std::size_t in_ptrs_size_;
  unsigned int current_version;
  Node **versiones;
};

}  // namespace Persistence
}  // namespace ADE

#endif  // SOURCE_PARTIALDIRECTEDGRAPH_HPP_
