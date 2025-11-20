#include "../include/user.h"
#include "../include/follow_list.h"
#include <iostream>
using namespace std;

User::User(int id, const string& name) {
    userID=id;
    userName=name;
    following=new FollowList();
    // posts=PostList();

    
}

// Copy constructor
User::User(const User& other)  {
    userID=other.userID;
    userName=other.userName;
    posts=other.posts;
    following=new FollowList();
    // Don't copy the following relationships - they should be rebuilt separately
    // This prevents circular dependency issues and dangling pointers
}

// Copy assignment operator
User& User::operator=(const User& other) {
    
    if (this == &other){
         return *this;
    } 

    userID = other.userID;
    userName = other.userName;
    posts = other.posts;
    delete following;
    following = new FollowList();
    return *this;
}

// Move constructor
User::User(User&& other) noexcept {
    //essentially i can copy into another variable and then remove from original but this
    //will be very expensive and slower compared to t=std::move. because lets say we have two pointers
    //a and b. a points to a memory, if we perform a copy operation, b will be alocated a new memory with duplicate
    //items, but if we move, simply b will be given the ownership of a and a will now point to null
    //(src: https://stackoverflow.com/questions/27888873/copy-vs-stdmove-for-ints)
    userID=std::move(other.userID);
    userName=std::move(other.userName);
    posts=std::move(other.posts);
    following=std::move(other.following);
    other.following=nullptr;
    //another important point to note is that moving is only a shortcut and more efficient method
    //for resource heavy objects however not ints because in the ccase of ints, its the same as copying



}

// Move assignment operator
User& User::operator=(User&& other) noexcept {
    if(this==&other){
        return *this;
    }
    this->userID=std::move(other.userID);//as i explained above, no different from this->userid=other.userid
    this->userName=std::move(other.userName);
    this->posts=std::move(other.posts);
    delete following;
    following=std::move(other.following);
    other.following=nullptr;
    return *this;


   
}

User::~User() {
    delete following;
    following=nullptr;
    
}

void User::addPost(int postID, const string& category) {
    Post newpost(postID,category);
    posts.addPost(newpost);
    
}

void User::followUser(User* otherUser) {
    if(otherUser==nullptr || otherUser->userID==this->userID ||following==nullptr){
        return;
    }
    following->addFollowing(otherUser);
    
}

void User::displayFollowing() const {
    if(following!=nullptr){
        following->displayFollowing();

    }
    else{
        cout<<"Following list is not available\n";
    }
    
}