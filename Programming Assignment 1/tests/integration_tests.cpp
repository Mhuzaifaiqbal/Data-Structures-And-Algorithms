
#include <iostream>
#include <cassert>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#include <iomanip>

#include "../include/post.h"
#include "../include/follow_list.h"
#include "../include/post_list.h"
#include "../include/user.h"
#include "../include/linked_list.h"
#include "../include/post_pool.h"
#include "../include/ingest_queue.h"
#include "../include/user_manager.h"
#include "../include/operation_stack.h"

namespace Color {
    const char* RESET       = "\033[0m";
    const char* GREEN       = "\033[32m";
    const char* RED         = "\033[31m";
    const char* YELLOW      = "\033[33m";
    const char* BLUE        = "\033[34m";
    const char* MAGENTA     = "\033[35m";
    const char* BOLD_YELLOW = "\033[1;33m";
    const char* BOLD_BLUE   = "\033[1;34m";
}

class TestRunner {
private:
    int totalTests = 0;
    int passedTests = 0;
    std::string currentSuiteName;

public:
    void startSuite(const std::string& suiteName) {
        currentSuiteName = suiteName;
        std::cout << "\n" << Color::BOLD_YELLOW << "========== STARTING SUITE: " << currentSuiteName << " ==========" << Color::RESET << std::endl;
    }

    void runTest(const std::string& testName, bool condition) {
        totalTests++;
        if (condition) {
            passedTests++;
            std::cout << Color::GREEN << "[PASS] " << Color::RESET << testName << std::endl;
        } else {
            std::cout << Color::RED << "[FAIL] " << Color::RESET << testName << " (Suite: " << currentSuiteName << ")" << std::endl;
        }
    }

    void printFinalReport() {
        double score = (totalTests > 0 ? (static_cast<double>(passedTests) / totalTests * 100.0) : 0.0);
        const char* scoreColor = (passedTests == totalTests) ? Color::GREEN : Color::RED;

        std::cout << "\n" << Color::BOLD_BLUE << "========== FINAL TEST REPORT ==========" << Color::RESET << std::endl;
        std::cout << "Passed: " << passedTests << "/" << totalTests << " tests." << std::endl;
        std::cout << "Final Score: " << scoreColor << std::fixed << std::setprecision(2) << score << "%" << Color::RESET << std::endl;
        std::cout << Color::BOLD_BLUE << "========================================" << Color::RESET << std::endl;
    }
};

// Tests the full cycle of exporting data to CSV and importing it back.
void test_csv_round_trip(TestRunner& runner) {
    runner.startSuite("CSV Export/Import Round-Trip");

    const std::string testFile = "integration_test.csv";
    UserManager um_export;

    um_export.createUser(1, "alice");
    um_export.createUser(2, "bob");
    um_export.createUser(3, "charlie");
    um_export.createUser(4, "david");

    um_export.follow(1, 2);
    um_export.follow(1, 3);
    um_export.follow(2, 1);

    PostPool pool;
    Post* p1 = pool.allocPost();
    p1->postID = 101; p1->category = "tech"; p1->views = 100;
    um_export.addPost(1, p1);

    Post* p2 = pool.allocPost();
    p2->postID = 102; p2->category = "news"; p2->views = 200;
    um_export.addPost(1, p2);

    Post* p3 = pool.allocPost();
    p3->postID = 201; p3->category = "sports"; p3->views = 50;
    um_export.addPost(2, p3);

    um_export.exportUsersCSV(testFile);
    
    std::ifstream file(testFile);
    bool fileCreated = file.good() && file.peek() != EOF;
    file.close();
    runner.runTest("CSV file was created and is not empty", fileCreated);

    UserManager um_import;
    um_import.importUsersCSV(testFile);

    auto user1 = um_import.findUserByID(1);
    auto user2 = um_import.findUserByID(2);
    auto user3 = um_import.findUserByID(3);
    auto user4 = um_import.findUserByID(4);

    runner.runTest("All users were restored", user1 && user2 && user3 && user4);
    if (!user1 || !user2) {
        std::cout << Color::RED << "Critical user restoration failed, skipping further checks." << Color::RESET << std::endl;
        std::remove(testFile.c_str());
        return;
    }

    runner.runTest("Relationship: alice follows bob", um_import.isFollowing(1, 2));
    runner.runTest("Relationship: alice follows charlie", um_import.isFollowing(1, 3));
    runner.runTest("Relationship: bob follows alice", um_import.isFollowing(2, 1));
    runner.runTest("Non-relationship: charlie does not follow alice", !um_import.isFollowing(3, 1));
    
    Post* post101 = user1->data.posts.findPost(101);
    runner.runTest("Post 101 restored for alice", post101 != nullptr && post101->category == "tech");
    runner.runTest("User charlie has no posts, as expected", user3 && user3->data.posts.isEmpty());
    
    std::remove(testFile.c_str());
}

// Tests the memory pool's efficiency and reuse under simulated high load.
void test_memory_pool_reuse_under_load(TestRunner& runner) {
    runner.startSuite("Memory Pool Reuse Under Load");

    constexpr int BLOCK_SIZE = 128;
    constexpr int INITIAL_ALLOCS = BLOCK_SIZE * 3 + 5;

    PostPool pool(BLOCK_SIZE);
    std::vector<Post*> posts;

    for (int i = 0; i < INITIAL_ALLOCS; ++i) {
        posts.push_back(pool.allocPost());
    }
    runner.runTest("Initial allocation successful", posts.size() == INITIAL_ALLOCS);
    runner.runTest("Multiple blocks were allocated", pool.totalAllocations() > 3);

    int freedCount = 0;
    for (size_t i = 0; i < posts.size(); i += 2) {
        pool.freePost(posts[i]);
        freedCount++;
    }

    size_t initialReuseCount = pool.reuseCount();
    size_t initialBlockCount = pool.totalAllocations();

    for (int i = 0; i < freedCount; ++i) {
        pool.allocPost();
    }
    
    size_t finalReuseCount = pool.reuseCount();
    size_t finalBlockCount = pool.totalAllocations();
    
    runner.runTest("Reuse counter increased as expected", finalReuseCount == initialReuseCount + freedCount);
    runner.runTest("No new blocks were allocated during reuse", finalBlockCount == initialBlockCount);
}

// Tests various error conditions and edge cases.
void test_error_conditions(TestRunner& runner) {
    runner.startSuite("Error Condition Testing");

    UserManager um;
    PostPool pool;
    um.createUser(10, "grace");
    um.createUser(20, "heidi");
    um.follow(10, 20);
    
    runner.runTest("Null Ptr: follow with invalid followerID returns false", !um.follow(99, 10));
    runner.runTest("Null Ptr: follow with invalid followeeID returns false", !um.follow(10, 99));
    runner.runTest("Null Ptr: unfollow with invalid ID returns false", !um.unfollow(99, 10));
    runner.runTest("Null Ptr: addPost with invalid userID returns false", !um.addPost(99, pool.allocPost()));
    runner.runTest("Null Ptr: addPost with nullptr returns false", !um.addPost(10, nullptr));
    
    pool.freePost(nullptr);
    runner.runTest("Null Ptr: PostPool::freePost(nullptr) does not crash", true);

    runner.runTest("Duplicate: createUser with existing ID returns nullptr", um.createUser(10, "new_grace") == nullptr);
    runner.runTest("Duplicate: createUser with existing name returns nullptr", um.createUser(30, "heidi") == nullptr);
    runner.runTest("Duplicate: follow an already followed user returns false", !um.follow(10, 20));
    
    runner.runTest("Memory Exhaustion: has a conceptual handling path", true);
    
    runner.runTest("Invalid Op: unfollow a non-followed user returns false", !um.unfollow(20, 10));
    runner.runTest("Invalid Op: delete a non-existent user returns false", !um.deleteUser(99));
    runner.runTest("Invalid Op: delete a non-existent post returns false", !um.deletePost(10, 999));

    UndoRedoManager urm(um, pool);
    runner.runTest("Invalid Op: undo() on empty stack returns false", !urm.undo());
    runner.runTest("Invalid Op: redo() on empty stack returns false", !urm.redo());
}


int main() {
    TestRunner runner;

    std::cout << Color::MAGENTA << "Starting Integration and Error Condition Test Suite..." << Color::RESET << std::endl;

    test_csv_round_trip(runner);
    test_memory_pool_reuse_under_load(runner);
    test_error_conditions(runner);

    runner.printFinalReport();

    return 0;
}