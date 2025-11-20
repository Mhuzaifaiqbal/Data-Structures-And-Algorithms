#include "../include/post_list.h"
#include <iostream>

PostList::~PostList() {
    // we have to  be carefull- remove all pointers using a current pointer
    PostNode* current=head;
    while(current!=NULL){
        PostNode* next= current->next;
        // delete current->next;
        delete current->post;
        delete current;
        current=next;
    }
    head=nullptr;

}

void PostList::addPost(const Post& p) {
    //naya post bana kar us par point karwaan aur increment
    Post* newpost=new Post(p);
    PostNode* newnode=new PostNode(newpost);
    newnode->next=head;
    head=newnode;
    
}

bool PostList::removePost(int postID) {
    PostNode* prev=nullptr;
    PostNode* curr=head;
    while (curr) {
        if (curr->post && curr->post->postID == postID) {
            if (prev) prev->next = curr->next;
            else head = curr->next;
            delete curr->post;
            delete curr;
            return true;
        }
        prev = curr;
        curr = curr->next;
    }
    return false;
    
}

Post* PostList::findPost(int postID) {
    PostNode* current=head;
    while(current!=nullptr){
        if(current->post && current->post->postID==postID){
            return current->post;
        }
        current=current->next;
    }
    //given that we didnt find the post for this id we can return nullptr
    return nullptr;
    
}

void PostList::displayPosts() const {
    //1.list can be empty head==nullptr
    //iterate throughout the list with current and cout
    if(head==nullptr){
        std::cout<<"The list contains no posts\n";
        return;
    }
    PostNode* current=head;
    while(current!=nullptr){
        if(current->post!=nullptr){
            std::cout<<"PostID:"<< current->post->postID<<"\n Category for post:"<< current->post->category<<"\n";
            std::cout<<"Post Views:"<<current->post->views<<"\n Post Content:"<<current->post->content<<"\n";

        }
        current=current->next;

    }
    
}

bool PostList::isEmpty() const {
    if(head==nullptr){
        return true;
    }
    return false;
    
}