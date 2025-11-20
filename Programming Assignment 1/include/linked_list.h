#pragma once
#include <functional> 
#include <cstddef>    
using namespace std;

template<typename T>
class LinkedList {
public:
    struct Node {
        T data;
        Node* prev;
        Node* next;
        Node(const T& d): data(d), prev(nullptr), next(nullptr) {}
    };

    LinkedList();
    ~LinkedList();

    Node* push_back(const T& val);   // O(1)
    Node* push_front(const T& val);  // O(1)
    void insert_after(Node* pos, const T& val); // O(1)
    void remove(Node* node); // O(1)
    Node* find(function<bool(const T&)> pred); // O(n)
    Node* head() const;
    Node* tail() const;
    size_t size() const;    // O(1)
    void clear();           // frees nodes

private:
    Node* _head;
    Node* _tail;
    size_t _size;
};
