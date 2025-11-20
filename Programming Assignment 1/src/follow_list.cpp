#include "../include/follow_list.h"
#include "../include/user.h"
#include <iostream>
using namespace std;

FollowList::~FollowList() {
    FollowNode* current=head;
    while(current!=nullptr){
        FollowNode*nextnode=current->next;
        delete current;
        current=nextnode;
    }

    head=nullptr;

    
}

void FollowList::addFollowing(User* u) {
    if(u==nullptr){
        return;

    }
    // if(u->userID==u->userID){
    //     return; //cant follow themselves

    // }
    // we have to check for duplicates as the question asks us to do
    FollowNode* current=head;
    while(current!=nullptr){
        if(current->user!=nullptr){
            if(current->user->userID==u->userID){
                return;//since a duplciate
            }
            
            // current=current->next;
        
        }
        current=current->next;
    }
    //as said we need to enter at start so pretty simple
    FollowNode* newNode = new FollowNode(u);
    newNode->next = head;
    head = newNode;
    
}

bool FollowList::removeFollowing(int userID) {
    //a simple removal of nodes function
    //we have to check 3 cases, if its head, tail or somewhere in middle
    if(!head){//given head doesnt exist
        return false;
    }
    
    FollowNode* current = head;
    FollowNode* previous = nullptr;// as the question said-> to keep track of my previous node

    if (current->user && current->user->userID == userID) {
        head = head->next; 
        delete current;
        return true;
    }

    //
    while (current) {
        if (current->user && current->user->userID == userID) {
            previous->next = current->next; 

            delete current;
            return true;
        }
        previous = current;
        current = current->next;
    }
    return false  ;  
}


User* FollowList::findFollowing(int userID) {
    //simple traversal
    FollowNode* searchnode=head;
    if(!searchnode){
        return nullptr;
    }
    while(searchnode){
        if(searchnode->user && searchnode->user->userID==userID){
            return searchnode->user;
        }
        else{
            searchnode=searchnode->next;
        }

    }
    return nullptr;
}

void FollowList::displayFollowing() const {
    if(head==nullptr){
        cout<<"No followed user\n";
        return;
    }
    cout<<"The following list is:-\n";
    int i=1;
    FollowNode* displaynode=head;
    while(displaynode){
        if(displaynode!=nullptr){
            cout<<i<<" "<<displaynode->user->userName<<" (ID:"<<displaynode->user->userID<<")\n";
        }
        else{
            cout<<"Invalid user\n";

        }
        i++;
        displaynode=displaynode->next;
    }

   
}