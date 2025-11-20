#include "../include/ingest_queue.h"
#include <iostream>

using namespace std;

IngestQueue::IngestQueue(size_t capacity) {
    this->capacity=capacity;
    head_idx=0;
    tail_idx=0;
    count=0;
    buffer = new Post*[capacity];
    for (size_t i = 0; i < capacity; i++) {
        buffer[i] = nullptr;
    }
    
}

IngestQueue::~IngestQueue() {
    delete[] buffer;
   
}

bool IngestQueue::enqueue(Post* p) {
    if(count==capacity){
        return false;
    }
    buffer[tail_idx]=p;
    tail_idx=(tail_idx+1)%capacity;
    count++;
    return true;
    
}

Post* IngestQueue::dequeue() {
    if(count==0){//empty
        return nullptr;
    }
    Post *p=buffer[head_idx];
    buffer[head_idx]=nullptr;
    head_idx=(head_idx+1)%capacity;
    count--;
    return p;

   
}

size_t IngestQueue::size() const {//////what is this?----------------------
    return count;
    
}

bool IngestQueue::empty() const {
    if(count==0){
        return true;
    }
    else{
        return false;
    }
    
}

size_t IngestQueue::dequeueBatch(Post** out_array, size_t max_k) {//khair hoye
    if(count==0){
        return 0;
    }
    size_t to_remove=max_k;
    if(count<max_k){
        to_remove=count;

    }
    //remove from buffer by incrememnting head in a circular manner however store in out
    for(size_t i=0;i<to_remove;i++){
        out_array[i]=buffer[head_idx];
        buffer[head_idx]=nullptr;
        head_idx=(head_idx+1)%capacity;
        count--;
    }
    return to_remove;
    
}

// Efficiently removes a batch of posts from the queue. This is more
// performant than dequeuing one by one. It calculates the number of items to
// dequeue (the minimum of the current count and max k), copies that many Post
// pointers into the provided out array, and updates the head idx and count
// accordingly. It returns the number of items that were successfully dequeued.