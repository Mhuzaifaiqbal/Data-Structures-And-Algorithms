#ifndef OPERATION_STACK_H
#define OPERATION_STACK_H

#include <string>
#include <vector>
#include <stack>

#include "user_manager.h"
#include "post_pool.h"
using namespace std;

enum class OpType {
    CREATE_USER,
    DELETE_USER,
    CREATE_POST,
    DELETE_POST,
    FOLLOW,
    UNFOLLOW,
    EDIT_POST
};

struct OpFrame {
    OpType type;
    int userID;
    int postID;
    string snapshot_username_or_content; // for edit/delete restores
};

class UndoRedoManager {
public:
    UndoRedoManager(UserManager& um, PostPool& pool);

    void beginTransaction();
    void commitTransaction();
    void rollbackTransaction(); // undo back to last marker

    void record(const OpFrame& f);

    bool undo(); // one step
    bool redo(); // one step

private:
    UserManager& userManager;
    PostPool& postPool;

    vector<OpFrame> undoStack;
    vector<OpFrame> redoStack;
    stack<size_t> transactionMarkers;
    vector<void*> trashList;
};

#endif // OPERATION_STACK_H
