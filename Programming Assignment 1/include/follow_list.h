#ifndef FOLLOW_LIST_H
#define FOLLOW_LIST_H

// Forward declaration of User
struct User;

struct FollowNode {
    User* user;      // Pointer to another User
    FollowNode* next;

    FollowNode(User* u) : user(u), next(nullptr) {}
};

struct FollowList {
    FollowNode* head;

    FollowList() : head(nullptr) {}
    ~FollowList();

    void addFollowing(User* u);
    bool removeFollowing(int userID);
    User* findFollowing(int userID);
    void displayFollowing() const;
};

#endif // FOLLOW_LIST_H
