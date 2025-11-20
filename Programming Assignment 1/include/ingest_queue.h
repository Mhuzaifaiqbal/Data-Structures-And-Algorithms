#ifndef INGEST_QUEUE_H
#define INGEST_QUEUE_H

#include <cstddef>   // size_t
#include "post.h"    // Post definition
using namespace std;

class IngestQueue {
public:
    explicit IngestQueue(size_t capacity = 8192); // capacity = number of Post* entries
    ~IngestQueue();

    // Enqueue a Post* (ownership of Post stays with caller/Pool)
    bool enqueue(Post* p); // returns false if queue full (or auto-resize allowed)
    Post* dequeue();       // returns nullptr if empty
    size_t size() const; 
    bool empty() const;

    // batch helpers
    size_t dequeueBatch(Post** out_array, size_t max_k); // fills caller array up to max_k, returns count

    // disable copy to avoid accidental double-free / shallow buffer copy
    IngestQueue(const IngestQueue&) = delete;
    IngestQueue& operator=(const IngestQueue&) = delete;

private:
    Post** buffer;         // circular buffer (array of Post*)
    size_t capacity;  // capacity of buffer (number of Post* entries)
    size_t head_idx;  // index of next element to pop
    size_t tail_idx;  // index of next location to push
    size_t count;     // current number of elements in queue
};

#endif // INGEST_QUEUE_H
