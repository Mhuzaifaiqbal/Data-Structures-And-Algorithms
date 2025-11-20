#include "../include/post_pool.h"
using namespace std;

PostPool::PostPool(size_t block_size) {
    this->block_size=block_size;
    current_block_index=0;
    alloc_count=0;
    reuse_count=0;
    allocateBlock();
   
}

PostPool::~PostPool() {
    purge();
    
}

Post* PostPool::allocPost() {  
    if(free_list.empty()==false){
        Post* ptr=free_list.back();
        free_list.pop_back();
        reuse_count++;
        return ptr;
    }
   //now that if the condition is such that the current block is full, we can allocate a new one so for that we a re goinf to use a function already in the h file
   if(current_block_index==block_size){
    allocateBlock();
   }
   Post* ptr=&blocks.back()[current_block_index];
   current_block_index++;
   return ptr;
}

void PostPool::freePost(Post* p) {
    //essentially just initialize everything to default or clear
    // if(p){
    //     return;

    // }
    // else{
    //     p->postID=0;
    //     p->content="";
    //     p->category="";
    //     p->views=0;
    //     free_list.push_back(p);
    // }

    // --------------------------  Wrote it wrong way(opposite)------------------
    if(p!=nullptr){
        p->postID=0;
        p->category.clear();
        p->views=0;
        p->content.clear();
        free_list.push_back(p);
    }
   
}

size_t PostPool::totalAllocations() const {
    return alloc_count;
    
}

size_t PostPool::reuseCount() const {
    return reuse_count;

    
}

void PostPool::purge() {
    //alright so we need to iterate over the blocks and for that we need to understand the fact that
        // blocks.size actually returns use a size_t so if we were to loop over it we need 
        // size_t i not int i
        size_t max=blocks.size();
    for(size_t i=0; i<max;i++){
        delete[] blocks[i];
    }
    blocks.clear();
    free_list.clear();

    alloc_count=0;
    reuse_count=0;
    current_block_index=0;
    return;
}

void PostPool::allocateBlock() {
    Post* newBlock = new Post[block_size];
    blocks.push_back(newBlock);
    alloc_count++;
    current_block_index = 0; 
   
}