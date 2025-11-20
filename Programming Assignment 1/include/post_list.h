#ifndef POST_LIST_H
#define POST_LIST_H

#include "post.h"

struct PostNode {
    Post* post;      // pointer into Post pool
    PostNode* next;
    PostNode(Post* p): post(p), next(nullptr) {}
};

struct PostList {
    PostNode* head;

    PostList() : head(nullptr) {}
    ~PostList();

    void addPost(const Post& p);
    bool removePost(int postID);
    Post* findPost(int postID);
    void displayPosts() const;
    bool isEmpty() const;
};

#endif
