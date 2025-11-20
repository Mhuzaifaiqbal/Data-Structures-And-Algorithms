#include "../include/operation_stack.h"
#include <cstddef> // size_t
using namespace std;

UndoRedoManager::UndoRedoManager(UserManager& um, PostPool& pool):userManager(um),postPool(pool) {
    
}
    

void UndoRedoManager::beginTransaction() {
    // if(inTransaction){

    // }
    transactionMarkers.push(undoStack.size());
    
}

void UndoRedoManager::commitTransaction() {
    if (!transactionMarkers.empty()) {
        transactionMarkers.pop();
    }
    redoStack.clear();
    trashList.clear();
}

void UndoRedoManager::rollbackTransaction() {
//     struct OpFrame {
//     OpType type;
//     int userID;
//     int postID;
//     string snapshot_username_or_content; // for edit/delete restores
// };
     if (transactionMarkers.empty()) {
        return;
    }

    size_t marker = transactionMarkers.top();
    transactionMarkers.pop();

    while (undoStack.size() > marker) {
        OpFrame frame = undoStack.back();
        undoStack.pop_back();

        switch (frame.type) {
            case OpType::CREATE_USER:
                userManager.deleteUser(frame.userID);
                break;
            case OpType::DELETE_USER:
                userManager.createUser(frame.userID, frame.snapshot_username_or_content);
                break;
            case OpType::CREATE_POST:
                userManager.deletePost(frame.userID, frame.postID);
                break;
            case OpType::DELETE_POST: {
                auto newPost = postPool.allocPost();
                newPost->postID = frame.postID;
                newPost->category = frame.snapshot_username_or_content;
                newPost->views = 0;
                userManager.addPost(frame.userID, newPost);
                break;
            }
            case OpType::FOLLOW:
                userManager.unfollow(frame.userID, frame.postID);
                break;
            case OpType::UNFOLLOW:
                userManager.follow(frame.userID, frame.postID);
                break;
            case OpType::EDIT_POST:
                break;
        }
          trashList.push_back(nullptr);
    }

    redoStack.clear();// so this was my fault, we have to clear redo stack after rollback
}

void UndoRedoManager::record(const OpFrame& f) {
    undoStack.push_back(f);
    
    redoStack.clear();//again to clear redo stack 
    
}
//bhai ye kaisay hona------------------------------------

bool UndoRedoManager::undo() {
    // if(i)
    if (undoStack.empty()) {
        return false; //if there is nothing to undo then we can returj false;
    }
    
    OpFrame frame = undoStack.back();
    undoStack.pop_back();
    switch (frame.type) {
        case OpType::DELETE_USER:
            userManager.createUser(frame.userID, frame.snapshot_username_or_content);
            redoStack.push_back(frame);
            return true;
            
        case OpType::CREATE_POST:
            if (userManager.deletePost(frame.userID, frame.postID)) {
                redoStack.push_back(frame);
                return true;
            }
            break;
            
        case OpType::DELETE_POST: {
            Post* newPost = postPool.allocPost();
            if (newPost) {
                newPost->postID = frame.postID;
                size_t pos1 = frame.snapshot_username_or_content.find(':');
                size_t pos2 = frame.snapshot_username_or_content.find(':', pos1 + 1);
                
                if (pos1 != string::npos && pos2 != string::npos) {
                    newPost->category = frame.snapshot_username_or_content.substr(0, pos1);
                    newPost->views = stoi(frame.snapshot_username_or_content.substr(pos1 + 1, pos2 - pos1 - 1));
                    newPost->content = frame.snapshot_username_or_content.substr(pos2 + 1);
                } else {
                    newPost->content = frame.snapshot_username_or_content;
                    newPost->category = "";
                    newPost->views = 0;
                }
                
                if (userManager.addPost(frame.userID, newPost)) {
                    redoStack.push_back(frame);
                    return true;
                }
            }
            break;
        }
        
        case OpType::FOLLOW:
            if (userManager.unfollow(frame.userID, frame.postID)) {
                redoStack.push_back(frame);
                return true;
            }
            break;
            
        case OpType::UNFOLLOW:
            if (userManager.follow(frame.userID, frame.postID)) {
                redoStack.push_back(frame);
                return true;
            }
            break;
            
        default:
            break;
    }
    
    undoStack.push_back(frame);
    return false;
}

bool UndoRedoManager::redo() {
         if (redoStack.empty()) {
        return false;
    }
    
    OpFrame frame = redoStack.back();
    redoStack.pop_back();
    
    switch (frame.type) {
        case OpType::DELETE_USER:
            if (userManager.deleteUser(frame.userID)) {
                undoStack.push_back(frame);
                return true;
            }
            break;
            
        case OpType::CREATE_POST: {
            Post* newPost = postPool.allocPost();
            if (newPost) {
                newPost->postID = frame.postID;
                size_t pos1 = frame.snapshot_username_or_content.find(':');
                size_t pos2 = frame.snapshot_username_or_content.find(':', pos1 + 1);
                
                if (pos1 != string::npos && pos2 != string::npos) {
                    newPost->category = frame.snapshot_username_or_content.substr(0, pos1);
                    newPost->views = stoi(frame.snapshot_username_or_content.substr(pos1 + 1, pos2 - pos1 - 1));
                    newPost->content = frame.snapshot_username_or_content.substr(pos2 + 1);
                } else {
                    newPost->content = frame.snapshot_username_or_content;
                    newPost->category = "";
                    newPost->views = 0;
                }
                
                if (userManager.addPost(frame.userID, newPost)) {
                    undoStack.push_back(frame);
                    return true;
                }
            }
            break;
        }
        
        case OpType::DELETE_POST:
            if (userManager.deletePost(frame.userID, frame.postID)) {
                undoStack.push_back(frame);
                return true;
            }
            break;
            
        case OpType::FOLLOW:
            if (userManager.follow(frame.userID, frame.postID)) {
                undoStack.push_back(frame);
                return true;
            }
            break;
            
        case OpType::UNFOLLOW:
            if (userManager.unfollow(frame.userID, frame.postID)) {
                undoStack.push_back(frame);
                return true;
            }
            break;
            
        default:
            break;
    }
//else we have to push it back    
    redoStack.push_back(frame);
    return false;
    
}
