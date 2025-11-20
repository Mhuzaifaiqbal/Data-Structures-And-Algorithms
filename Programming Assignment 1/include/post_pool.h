#ifndef POST_POOL_H
#define POST_POOL_H

#include <cstddef>   // size_t
#include <vector>    // vector for block storage
#include "post.h"    // Full definition of Post is required for alloc/free
using namespace std;

class PostPool {
public:
    explicit PostPool(size_t block_size = 4096); // number of Post objects per allocation chunk
    ~PostPool();

    Post* allocPost();         // returns pointer to Post (uninitialized or default-initialized)
    void freePost(Post* p);    // returns to free-list

    size_t totalAllocations() const; // number of 'new' chunk allocations
    size_t reuseCount() const;       // number of times a freed post was reused

    void purge(); // release all memory (for teardown)

private:
    size_t block_size;
    vector<Post*> blocks;      // each block is an array of Posts
    vector<Post*> free_list;   // stack of freed Post* ready to be reused
    size_t current_block_index; // index into the last block for the next allocation

    size_t alloc_count;   // how many chunks allocated
    size_t reuse_count;   // how many reused

    void allocateBlock();      // helper: allocate a new block of Posts
};

#endif // POST_POOL_H
