#include "../include/linked_list.h"
#include <iostream>
#include "../include/user.h"  // Add this so 'User' is complete
using namespace std;

template<typename T>
LinkedList<T>::LinkedList()  {
    _head=NULL;
    _tail=NULL;
    _size=0;
}

template<typename T>
LinkedList<T>::~LinkedList() {
    clear();
    
}

template<typename T>
typename LinkedList<T>::Node* LinkedList<T>::push_back(const T& val) {
   Node* newnode=new Node(val);
   newnode->next=nullptr;
 
   if(_tail==nullptr){
    _head=newnode;
    _tail=newnode;

   }
   else{
    newnode->prev=_tail;
    _tail->next=newnode;
    _tail=newnode;
   }
   _size++;
   return _tail;
    
}

template<typename T>
typename LinkedList<T>::Node* LinkedList<T>::push_front(const T& val) {
    Node* newnode=new Node(val);
    newnode->prev=nullptr;
    if(_head==nullptr){
        _head=newnode;
        _head->prev=nullptr;
        newnode->next=nullptr;
        _tail=newnode;
    }
    else{

        newnode->next=_head;
        _head->prev=newnode;
        _head=newnode;

    }
    _size++;
    return _head;

    
}

template<typename T>
void LinkedList<T>::insert_after(Node* pos, const T& val) {
    //pos is a pointert directly to the node where we need to insert
    if(pos==nullptr){
        std::cout<<"Huzaifa says its An error!";
        return;
    }
    Node* newnode=new Node(val);
     //we need to take into account if it is not at the end 
    newnode->prev=pos;
    newnode->next=pos->next;

    if(pos->next==nullptr){ //if its at the end then assign tail to it
        _tail=newnode;
    }
    else{
        pos->next->prev=newnode;
    }
    pos->next=newnode;

    _size++;

    
}

template<typename T>
void LinkedList<T>::remove(Node* node) {
    if(!node){
        return;
    }
    if(node==_head && node==_tail){
        _head=nullptr;
        _tail=nullptr;
    }
    else if(node==_head){
        node->next->prev=nullptr;
        _head=node->next;
    }
    else if(node==_tail){
        node->prev->next=nullptr;
        _tail=node->prev;
    }
    else{
        node->prev->next=node->next;
        node->next->prev=node->prev;
    }
    delete node;
    _size--;
    
}

template<typename T>
typename LinkedList<T>::Node* LinkedList<T>::find(function<bool(const T&)> pred) {
    Node* current=_head;

    while(current!=nullptr){
        if(pred(current->data)){
            return current;
        }
        current=current->next;
    }
    return nullptr;
}

template<typename T>
typename LinkedList<T>::Node* LinkedList<T>::head() const {
    if(_head==nullptr){
        return nullptr;
    }
    return _head;
}

template<typename T>
typename LinkedList<T>::Node* LinkedList<T>::tail() const {
    if(_tail==nullptr){
        return nullptr;
    }
    return _tail;
}

template<typename T>
size_t LinkedList<T>::size() const {
    return _size;
    
}

template<typename T>
void LinkedList<T>::clear() {
    //Lets traverse the linked list and delete all nodes one by one
    //store each node in current so we dont lsoe access to the next node after we delete 
    Node* current=_head;

    while(current!=NULL){
        Node* nextnode=current->next;
        // current->next=nullptr;
        delete current;
        current=nextnode;
    }
    _head=NULL;
    _tail=NULL;
    _size=0;

    
}

// Explicit template instantiation for commonly used types
template class LinkedList<int>;
template class LinkedList<string>;

// Forward declare User struct for template instantiation
struct User;
template class LinkedList<User>;