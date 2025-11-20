#ifndef USER_MANAGER_H
#define USER_MANAGER_H

#include <string>   
#include <ostream> 

#include "linked_list.h"
#include "post.h"
using namespace std;

// Forward declarations to avoid circular includes
struct User;
struct Post;

class UserManager {
    public:
    UserManager();
    ~UserManager();

    // user lifecycle
    LinkedList<User>::Node* createUser(int userID, const string& username);
    bool deleteUser(int userID); // removes user and all references

    // follow operations
    bool follow(int followerID, int followeeID);
    bool unfollow(int followerID, int followeeID);
    bool isFollowing(int followerID, int followeeID) const;

    // post management (Post* obtained from PostPool)
    bool addPost(int userID, Post* post); // attach post to user's post list
    bool deletePost(int userID, PostID postID); // remove and free post via pool

    // lookups
    LinkedList<User>::Node* findUserByID(int userID);
    LinkedList<User>::Node* findUserByName(const string& username);

    // export / import
    void exportUsersCSV(const string& path) const;
    void importUsersCSV(const string& path); // tests can use

    // debugging helpers
    void dumpAllUsers(ostream& out) const;

    private:
    LinkedList<User> users; 
};

#endif