#pragma once
#include <string>
#include <cstdint>
using namespace std;
using UserID = int;
using PostID = int;

struct Post {
    PostID postID;
    string category;
    int views;
    string content;

    // metadata for linking inside lists (optional if wrapper node used)
    Post(PostID id = 0, const string& cat = "", int v = 0, const string& content="")
      : postID(id), category(cat), views(v), content(content) {}
};
