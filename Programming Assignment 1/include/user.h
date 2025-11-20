#ifndef USER_H
#define USER_H

#include <string>
#include "post_list.h"
using namespace std;

// Forward declare FollowList to avoid circular dependency
struct FollowList;

struct User {
    int userID;
    string userName;
    PostList posts;         // Linked list of this user's posts
    FollowList* following;  // linked list of followed users (owned)

    User(int id, const string& name);
    
    // Copy constructor
    User(const User& other);
    
    // Copy assignment operator
    User& operator=(const User& other);
    
    // Move constructor
    User(User&& other) noexcept;
    
    // Move assignment operator
    User& operator=(User&& other) noexcept;
    
    ~User();

    void addPost(int postID, const string& category);
    void followUser(User* otherUser);
    void displayFollowing() const;
};

#endif // USER_H