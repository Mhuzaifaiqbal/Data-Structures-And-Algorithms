#include "../include/user_manager.h"
#include "../include/user.h"
#include "../include/follow_list.h"
#include "../include/post_pool.h"
#include <fstream>
#include <sstream>
#include <iostream>
using namespace std;

UserManager::UserManager() {
    
}

UserManager::~UserManager() {
    users.clear();
}

LinkedList<User>::Node* UserManager::createUser(int userID, const string& username) {
    //okay so before creating we need to understand the fact that if userid or name already exists
    //then its a duplicate and we have to return a nullpointer
    if(findUserByID(userID)!=nullptr || findUserByName(username)!=nullptr ){
        return nullptr;
    }
    User newuser(userID,username);
    //now we have to add it to the  internal users LinkedList and verify the following pointers validity
    LinkedList<User>::Node* newnode=users.push_back(newuser);
    if(newnode==nullptr){
        return nullptr;
    }
    return newnode;

    
}

bool UserManager::deleteUser(int userID) {
    LinkedList<User>::Node* tofindnode=findUserByID(userID);
    if(tofindnode==nullptr){
        return false;
    }    
    LinkedList<User>::Node* current=users.head();
    while(current!=nullptr){
        if(&(current->data)==&(tofindnode->data)){
            if(current->data.following!=nullptr){
                current->data.following->removeFollowing(userID);
            }

        }
        current=current->next;
    }
    users.remove(tofindnode);
    return true;
}

bool UserManager::follow(int followerID, int followeeID) {
    //unable to follow themselves
    if(followeeID==followerID){
        return false;
    }

    LinkedList<User>::Node* follower_node = findUserByID(followerID);
    LinkedList<User>::Node* follownode = findUserByID(followeeID);
    if (follower_node==nullptr || follownode==nullptr){ 
        return false;
    }

    User& follower = follower_node->data;
    User& followee = follownode->data;

    if (follower.following==nullptr) {
        return false;
        }

    if (follower.following->findFollowing(followeeID) != nullptr){ 
        return false;
    }
    follower.following->addFollowing(&followee);
    return true;
    
    
}

bool UserManager::unfollow(int followerID, int followeeID) {//failing---------i was returning true in all cases blindly 
    LinkedList<User>::Node* follower_node = findUserByID(followerID);
    // if (follower_node==nullptr) {
    //     return false;
    // }
    // User& follower = follower_node->data;
    // if (follower.following==nullptr){ 
    //     return false;
    // }
    
    // follower.following->removeFollowing(followeeID);
    // return true;
    if (follower_node==nullptr){ 
        return false;
    }
    User& follower = follower_node->data;
    if (follower.following==nullptr){
         return false;
    }
    if(follower.following->removeFollowing(followeeID)){
        return true;
    }
    return false;
    
}

bool UserManager::isFollowing(int followerID, int followeeID) const {
    // LinkedList<User>::Node* followerNode =findUserByID(followerID);
    // if (!followerNode) return false;
    //due to const, the usermanager cannot be modified like follow method
    //so lets manually iterate to find the follower id
    LinkedList<User>::Node* current = users.head();
    while(current!=nullptr){
        if(current->data.userID==followerID){
            // User& follower=current->data;
            const User& follower=current->data;
            // we need to write const because our function can only read
            //if we dont use const than we are implying that we might modify follower which is not possible in this case.
            if(follower.following==nullptr){
                return false;
            }
            if(follower.following->findFollowing(followeeID)){
                return true;
            }
            
        }
        current=current->next;
    }
    return false;
    


    
}

bool UserManager::addPost(int userID, Post* post) {
    
    if(post==nullptr){
        return false;
    }
    LinkedList<User>::Node* newnode=findUserByID(userID);
    if(newnode==nullptr){
        return false;
    }
    User& huzaifa=newnode->data;
    //we can use the functionality of post list
    huzaifa.posts.addPost(*post);
    return true;

    
}

bool UserManager::deletePost(int userID, PostID postID) {
    //user postlist functions 
    LinkedList<User>::Node* removenode=findUserByID(userID);
    if(removenode==nullptr){
        return false;
    }
    User& huzaifa=removenode->data;
    if(!huzaifa.posts.findPost(postID)){
        return false;

    }
    huzaifa.posts.removePost(postID);
    return true;
    
    
}

LinkedList<User>::Node* UserManager::findUserByID(int userID) {
    LinkedList<User>::Node* findnode=users.head();
    while(findnode!=nullptr){
        if(findnode->data.userID==userID){
            return findnode;
        }
        findnode=findnode->next;
    }
    return nullptr;
    
}

LinkedList<User>::Node* UserManager::findUserByName(const string& username) {
    LinkedList<User>::Node* findnode=users.head();
    while(findnode!=nullptr){
        if(findnode->data.userName==username){
            return findnode;
        }
        findnode=findnode->next;
    }
    return nullptr;
    
}

void UserManager::exportUsersCSV(const string& path) const {
     ofstream out(path);
    if (!out.is_open()) {
        cerr << "Error, file cannot be opened\n";
        return;
    }
    //we have to follow the format userID,username,followeeID1|followeeID2|...,postID1:category1:views1|postID2:category2:views
    out << "userID,username,followees1|followees2....,posts and the content and stuff\n";
//the idea is to iterate first over the follow list, for each follower finding all their posts and writing all the content to the file in the same way the question demands
//so nested loop should work
    LinkedList<User>::Node* current = users.head();
    while (current!=nullptr) {
        const User& u = current->data;

        out << u.userID << "," << u.userName << ",";

        if (u.following!=nullptr) {
            auto starting = u.following->head;  //iteratig over the follow list, using auto so compiler figures the datatype itself
            while (starting) {
                out << starting->user->userID;  
                if (starting->next) out << "|"; 
                starting = starting->next;
            }
        }
        out << ",";

        auto post_find = u.posts.head; // iterating over postlist now because we need to display it accordingly as said in the question, same format
        while (post_find) {
            out << post_find->post->postID << ":"<< post_find->post->category << ":"<< post_find->post->views;
            if (post_find->next) {
                out << "|";
            }
            post_find = post_find->next;
        }

        out << "\n"; 
        current = current->next;
    }

    out.close();
}

void UserManager::importUsersCSV(const string& path) {
      ifstream in(path);
    if (!in.is_open()) {
        cout << "file could not be opened for use\n";
        return;
    }
    users.clear();//. It first clears any existing users-as said by the question
//     It then performs a first pass to create all users and add their posts. A
// second pass is used to establish following relationships (ensuring all users exist
// before attempting to follow). It carefully parses CSV fields, handling empty
// values, and uses createUser and follow methods for proper initialization
    

    string line;
    if (!getline(in, line)){
         return;
    }
    //when we are importing users from a CSV file, each line contains  user’s information and their posts, and the IDs of the users they follow e.t.c 
    // The problem is that users may appear in the file in any order not necessary that all are present in memory or have been created, so a user might reference another user as a followee before that followee has been even created in memory. 
    // If we tried to immediately establish their follow relationships while reading each line, some follow() calls would fail because the target user does not yet exist.
    //  To address this issue, we will use a temporary structure which will store the follower’s ID along with a list of followee IDs. 
    struct FollowTemp {
        int followerID;
        vector<int> followeeIDs;
    };
    vector<FollowTemp> followBuffer;

    // the first pass is to create all users and add their posts
    while (getline(in, line)) {
        if (line.empty()) continue;

        stringstream ss(line);
        string userIDStr, username, followeesStr, postsStr;

        getline(ss, userIDStr, ',');
        getline(ss, username, ',');


        getline(ss, followeesStr, ',');

        getline(ss, postsStr, ',');

        int userID = stoi(userIDStr);
        LinkedList<User>::Node* userNode = createUser(userID, username);
        if (userNode==nullptr){
             continue; 
        }
             User& newuser = userNode->data;

        // Add posts
        if (!postsStr.empty()) {
            stringstream ps(postsStr);
            string postItem;
            while (getline(ps, postItem, '|')) {
                stringstream postSS(postItem);
                string postIDStr, category, viewsStr;

                getline(postSS, postIDStr, ':');
                getline(postSS, category, ':');
                getline(postSS, viewsStr, ':');
                int postID = stoi(postIDStr);
                int views = stoi(viewsStr);
                Post* p = new Post(postID, category, views); // Post constructor directly
                newuser.posts.addPost(*p);
            }
        }
//saving the followees
//continue from here tmr-------------------------------------------------------------------------------------------------
        if (!followeesStr.empty()) {
            vector<int> followeeIDs;
            stringstream fs(followeesStr);
            string fid;
            while (getline(fs, fid, '|')) {
                followeeIDs.push_back(stoi(fid));
            }
            followBuffer.push_back({userID, followeeIDs});
        }
    }

    // second pass starting
   for (size_t i = 0; i < followBuffer.size(); ++i) {
    int followerID = followBuffer[i].followerID;
    for (size_t j = 0; j < followBuffer[i].followeeIDs.size(); ++j) {
        int followeeID = followBuffer[i].followeeIDs[j];
        follow(followerID, followeeID); 
    }
}

    in.close();
    
}

void UserManager::dumpAllUsers(ostream& out) const {
   LinkedList<User>::Node* current = users.head();

    while (current != nullptr) {
        const User& u = current->data;

        out << "UserID: " << u.userID << ", Username: " << u.userName << "\n";

        //std::cout<<"hzaifa\n";
        out << "Followees: ";
        if (u.following != nullptr && u.following->head != nullptr) {
            FollowNode* fNode = u.following->head; 
            while (fNode) {
                out << fNode->user->userID;
                if (fNode->next) out << " | ";
                fNode = fNode->next;
            }
        } else {
            out << "None";
        }
        out << "\n";

        // now posts
        out << "Posts: ";
        if (u.posts.head != nullptr) {
            PostNode* pNode = u.posts.head;
            while (pNode) {
                out << "[" << pNode->post->postID << ": " << pNode->post->category
                    << ", Views: " << pNode->post->views << "]";
                if (pNode->next) out << " | ";
                pNode = pNode->next;
            }
        } else {
            out << "None";
        }

        out << "\n---------------------------------\n";
        current = current->next;
    }
    }
    
    



// 11 UserManager (user manager.h)
// You are expected to implement a UserManager class, which acts as the central
// controller for all user-related operations. Its primary responsibilities include
// managing the user lifecycle (creation, deletion), handling user relationship man￾agement (following, unfollowing), and providing data persistence capabilities
// through CSV import/export.
// 11.1 createUser(int userID, const std::string& username)
// Creates and adds a new user to the system. It first checks for duplicate userID
// and username. If either exists, it returns nullptr. Otherwise, it creates a User
// object, adds it to the internal users LinkedList, verifies the following pointer’s
// validity, and returns a pointer to the newly created user’s node.
// 11.2 deleteUser(int userID)
// Removes a user with the specified userID and cleans up their references through￾out the system. It finds the user’s node by ID, removes this user from all other
// users’ following lists, then removes the user from the main users list. The User
// destructor automatically handles the cleanup of the user’s posts and their own
// following list. Returns true if the user was found and deleted, false otherwise.
// 11.3 follow(int followerID, int followeeID)
// Establishes a following relationship between two users. It prevents self-following,
// validates that both users exist, checks the follower’s following pointer, and ver￾ifies that the follower is not already following the followee. If all checks pass,
// it adds the followee to the follower’s following list. Returns true on success,
// false on failure.
// 11.4 unfollow(int followerID, int followeeID)
// Removes an existing following relationship between two users. It finds the fol￾lower user, validates that their following list exists, and then uses the FollowList::removeFollowing
// method to remove the specified relationship. Returns true on success, false
// on failure.
// 11.5 isFollowing(int followerID, int followeeID)
// Checks if a following relationship exists between two users. It finds the follower
// user, checks if their following list exists, and then uses the FollowList::findFollowing
// method to determine if the followee is in the list. Returns true if the follower
// is following the followee, false otherwise.
// 14
// 11.6 addPost(int userID, Post* post)
// Attaches a Post (obtained from PostPool) to a user’s post list. It finds the user
// by ID, validates the Post pointer, and then uses the User::addPost method
// to add it to the user’s collection of posts. Returns true on success, false on
// failure.
// 11.7 deletePost(int userID, PostID postID)
// Removes a post from a user’s list. It finds the user by ID and then uses the
// PostList::removePost method to remove and properly delete the post. Returns true on success, false on failure.
// 11.8 findUserByID(int userID)
// Locates a user by their unique userID. It utilizes the LinkedList::find method
// with a lambda predicate that compares the userID field of each User object.
// Returns a pointer to the LinkedList<User>::Node if found, nullptr otherwise.
// 11.9 findUserByName(const std::string& username)
// Locates a user by their username. It utilizes the LinkedList::find method
// with a lambda predicate that compares the userName field of each User object.
// Returns a pointer to the LinkedList<User>::Node if found, nullptr otherwise.
// 11.10 exportUsersCSV(const std::string& path)
// Saves all user data to a CSV file at the specified path. The CSV format is:
// userID,username,followeeID1|followeeID2|...,postID1:category1:views1|postID2:category2:views2|....
// It opens the output file, iterates through all users, writing their ID, username, pipe-separated followee IDs, and pipe-separated post entries (with colonseparated fields). It handles empty lists appropriately.
// 11.11 importUsersCSV(const std::string& path)
// Restores user data from a CSV file at the specified path. It first clears any existing users. It then performs a first pass to create all users and add their posts. A
// second pass is used to establish following relationships (ensuring all users exist
// before attempting to follow). It carefully parses CSV fields, handling empty
// values, and uses createUser and follow methods for proper initialization.