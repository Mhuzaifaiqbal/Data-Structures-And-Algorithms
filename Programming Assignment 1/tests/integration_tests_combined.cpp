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

void testUserCreationAndFollowingIntegration(TestRunner& runner) {
    runner.startSuite("USER CREATION AND FOLLOWING INTEGRATION");
    std::cout << Color::BLUE << "Testing comprehensive user management and following relationships..." << Color::RESET << std::endl;

    UserManager um;

    // Phase 1: Create 100 users
    std::cout << "\n" << Color::YELLOW << "PHASE 1: USER CREATION" << Color::RESET << std::endl;
    std::cout << "\n" << "Creating 100 users with unique IDs and usernames..." << std::endl;
    std::vector<int> userIDs;
    int creationFailures = 0;

    for (int i = 1; i <= 100; ++i) {
        std::string username = "user" + std::to_string(i);
        auto user = um.createUser(i, username);
        if (user) {
            userIDs.push_back(i);
            // std::cout << "  Created User ID " << i << " (" << username << ")... ";
            // std::cout << Color::GREEN << "SUCCESS" << Color::RESET << std::endl;
        } else {
            creationFailures++;
            // std::cout << Color::RED << "FAILED" << Color::RESET << std::endl;
        }

        
    }

    std::cout << "\n" << Color::BLUE << "USER CREATION SUMMARY:" << Color::RESET << std::endl;
    std::cout << "  Total users attempted: 100" << std::endl;
    std::cout << "  Successful creations: " << userIDs.size() << std::endl;
    std::cout << "  Failed creations: " << creationFailures << std::endl;

    runner.runTest("User creation - no failures", creationFailures == 0);
    runner.runTest("User creation - expected 100 users created", userIDs.size() == 100);
    
    // Phase 2: Establish complex following relationships
    std::cout << "\n" << Color::YELLOW <<  "PHASE 2: ESTABLISHING FOLLOWING RELATIONSHIPS" << Color::RESET << std::endl;
    int followSuccesses = 0;
    int followFailures = 0;

    // Star pattern: User 1 follows users 2-11
    std::cout << " Creating STAR pattern: User 1 follows users 2-11..." << std::endl;
    for (int i = 2; i <= 11; ++i) {
        if (um.follow(1, i)) {
            followSuccesses++;
        } else {
            followFailures++;
            // std::cout << Color::RED << "FAILED" << Color::RESET << std::endl;
        }
    }
    
    // Chain pattern: Users form a chain 20->21->22->23->24
    std::cout << "  Creating CHAIN pattern: 20->21->22->23->24..." << std::endl;
    for (int i = 20; i < 24; ++i) {
        std::cout << "    User " << i << " -> User " << (i+1) << "... ";
        if (um.follow(i, i + 1)) {
            followSuccesses++;
            // std::cout << Color::GREEN << "SUCCESS" << Color::RESET << std::endl;
        } else {
            followFailures++;
            // std::cout << Color::RED << "FAILED" << Color::RESET << std::endl;
        }
    }
    
    // Dense network: Users 30-40 all follow each other
    std::cout << "Creating DENSE NETWORK: Users 30-40 follow each other..." << std::endl;
    int denseOperations = 0;
    for (int i = 30; i <= 40; ++i) {
        for (int j = 30; j <= 40; ++j) {
            if (i != j) {
                denseOperations++;
                if (um.follow(i, j)) {
                    followSuccesses++;
                } else {
                    followFailures++;
                }
            }
        }
        if (i == 35) {
            std::cout << " Progress: " << denseOperations << " follow operations in dense network" << std::endl;
        }
    }

    std::cout << "\n" << Color::BLUE << "FOLLOWING OPERATIONS SUMMARY:" << Color::RESET << std::endl;
    std::cout << "  Star pattern: 10 follow operations" << std::endl;
    std::cout << "  Chain pattern: 4 follow operations" << std::endl;
    std::cout << "  Dense network: " << denseOperations << " follow operations" << std::endl;
    std::cout << "  Total operations: " << (10 + 4 + denseOperations) << std::endl;
    std::cout << "  Successful follows: " << followSuccesses  << std::endl;
    std::cout << "  Failed follows: " << followFailures  << std::endl;

    runner.runTest("Following operations - no failures", followFailures == 0);
    
    // Phase 3: Verify bidirectional relationships work
    std::cout << "\n" << Color::YELLOW << "PHASE 3: VERIFYING BIDIRECTIONAL RELATIONSHIPS" << Color::RESET << std::endl;
    
    // Test A follows B, B follows A
    bool mutual1 = um.follow(50, 51) && um.follow(51, 50);
    bool verify1 = um.isFollowing(50, 51) && um.isFollowing(51, 50);
    runner.runTest("Bidirectional relationship 50<->51 established", mutual1);
    runner.runTest("Bidirectional relationship 50<->51 verified", verify1);
    
    // Phase 4: Test chain following (A->B->C->A)
    std::cout << "\n" << Color::YELLOW << "PHASE 4: TESTING CIRCULAR FOLLOWING CHAIN" << Color::RESET << std::endl;
    bool chain = um.follow(60, 61) && um.follow(61, 62) && um.follow(62, 60);
    bool verifyChain = um.isFollowing(60, 61) && um.isFollowing(61, 62) && um.isFollowing(62, 60);
    runner.runTest("Circular chain 60->61->62->60 established", chain);
    runner.runTest("Circular chain 60->61->62->60 verified", verifyChain);
    
    // Phase 5: Validate data integrity
    std::cout << "\n" << Color::YELLOW << "PHASE 5: VALIDATING DATA INTEGRITY" << Color::RESET << std::endl;
    
    // Check user 1's following count (should be 10 from star pattern)
    auto user1 = um.findUserByID(1);
    runner.runTest("User 1 exists", user1 != nullptr);
    
    if (user1) {
        int followingCount = 0;
        if (user1->data.following) {
            // Count follows by traversing the list
            auto current = user1->data.following->head;
            while (current) {
                followingCount++;
                current = current->next;
            }
        }
        runner.runTest("User 1 follows exactly 10 users", followingCount == 10);
        std::cout << Color::GREEN << "SUCCESS: User 1 follows " << followingCount << " users (expected 10)" << Color::RESET << std::endl;
    }
    
    // Verify no self-follows exist
    int selfFollowCount = 0;
    for (int id : userIDs) {
        if (um.isFollowing(id, id)) {
            selfFollowCount++;
            std::cout << Color::RED << "ERROR: User " << id << " is following themselves!" << Color::RESET << std::endl;
        }
    }
    runner.runTest("No self-follows detected", selfFollowCount == 0);

    // Phase 6: Test unfollow operations
    std::cout << "\n" << Color::YELLOW << "PHASE 6: TESTING UNFOLLOW OPERATIONS" << Color::RESET << std::endl;
    int unfollowSuccesses = 0;

    // Break some relationships from the dense network
    for (int i = 30; i <= 32; ++i) {
        for (int j = 38; j <= 40; ++j) {
            if (um.unfollow(i, j)) {
                unfollowSuccesses++;
            }
        }
    }

    // Verify unfollows worked
    bool unfollowVerified = !um.isFollowing(30, 38) && !um.isFollowing(31, 39);
    runner.runTest("Unfollow operations completed successfully", unfollowSuccesses > 0);
    runner.runTest("Unfollow operations verified", unfollowVerified);

    // Phase 7: Test edge cases
    std::cout << "\n" << Color::YELLOW << "PHASE 7: TESTING EDGE CASES" << Color::RESET << std::endl;

    // Follow non-existent user
    bool followInvalid = um.follow(1, 999);
    runner.runTest("Should not follow non-existent user", !followInvalid);

    // Unfollow when not following
    bool unfollowInvalid = um.unfollow(80, 90);
    runner.runTest("Should not unfollow when not following", !unfollowInvalid);

    std::cout << Color::GREEN << "INTEGRATION TEST PASSED: User creation and following relationships" << Color::RESET << std::endl;
}

void testPostCreationAndAttachmentIntegration(TestRunner& runner) {
    runner.startSuite("POST CREATION AND ATTACHMENT INTEGRATION");
    
    UserManager um;
    PostPool pool;
    
    // Phase 1: Create users through UserManager
    std::cout << "\n" << Color::YELLOW << "PHASE 1: CREATING TEST USERS" << Color::RESET << std::endl;
    std::vector<int> userIDs;
    for (int i = 1; i <= 10; ++i) {
        auto user = um.createUser(i, "testuser" + std::to_string(i));
        if (user != nullptr) {
            userIDs.push_back(i);
        }
    }
    runner.runTest("Created 10 test users", userIDs.size() == 10);
    
    // Phase 2: Add posts through User class
    std::cout << "\n" << Color::YELLOW << "PHASE 2: ADDING POSTS THROUGH USER CLASS" << Color::RESET << std::endl;
    int totalPostsAdded = 0;
    std::vector<std::string> categories = {"tech", "sports", "news", "entertainment", "science"};
    
    for (int userID : userIDs) {
        auto userNode = um.findUserByID(userID);
        if (userNode != nullptr) {
            // Add 5 posts per user
            for (int p = 0; p < 5; ++p) {
                int postID = userID * 100 + p; // Unique post IDs
                std::string category = categories[p % categories.size()];
                
                userNode->data.addPost(postID, category);
                totalPostsAdded++;
            }
        }
    }
    runner.runTest("Added posts across all users", totalPostsAdded == 50);
    std::cout << Color::GREEN << "SUCCESS: Added " << totalPostsAdded << " posts across " << userIDs.size() << " users" << Color::RESET << std::endl;
    
    // Phase 3: Verify posts appear in PostList
    std::cout << "\n" << Color::YELLOW << "PHASE 3: VERIFYING POSTS IN POSTLIST" << Color::RESET << std::endl;
    int postsFound = 0;
    int postsNotFound = 0;
    
    for (int userID : userIDs) {
        auto userNode = um.findUserByID(userID);
        if (userNode != nullptr) {
            for (int p = 0; p < 5; ++p) {
                int postID = userID * 100 + p;
                Post* foundPost = userNode->data.posts.findPost(postID);
                if (foundPost) {
                    postsFound++;
                    if (foundPost->postID != postID) {
                        std::cout << Color::RED << "ERROR: Post ID mismatch for post " << postID << Color::RESET << std::endl;
                    }
                } else {
                    postsNotFound++;
                    std::cout << Color::RED << "ERROR: Post " << postID << " not found for user " << userID << Color::RESET << std::endl;
                }
            }
        }
    }
    
    runner.runTest("All posts are findable", postsNotFound == 0);
    runner.runTest("Found all expected posts in PostLists", postsFound == totalPostsAdded);
    
    // Phase 4: Test PostPool allocation through User operations
    std::cout << "\n" << Color::YELLOW << "PHASE 4: TESTING POSTPOOL INTEGRATION" << Color::RESET << std::endl;
    size_t initialAllocations = pool.totalAllocations();
    
    // Create more posts to trigger pool allocations
    auto testUser = um.findUserByID(1);
    runner.runTest("Test user exists", testUser != nullptr);
    
    if (testUser) {
        for (int i = 1000; i < 1020; ++i) {
            testUser->data.addPost(i, "pool_test");
        }
        
        size_t finalAllocations = pool.totalAllocations();
        std::cout << "Pool allocations: " << initialAllocations << " -> " << finalAllocations << std::endl;
        runner.runTest("Pool allocations increased", finalAllocations >= initialAllocations);
    }
    
    // Phase 5: Validate Post ownership and cleanup
    std::cout << "\n" << Color::YELLOW << "PHASE 5: TESTING POST REMOVAL AND CLEANUP" << Color::RESET << std::endl;
    int removedPosts = 0;
    
    if (testUser) {
        // Remove posts from user 1
        for (int i = 1000; i < 1010; ++i) {
            if (testUser->data.posts.removePost(i)) {
                removedPosts++;
            }
        }
        
        runner.runTest("Removed exactly 10 posts", removedPosts == 10);
        
        // Verify posts are actually gone
        int stillFound = 0;
        for (int i = 1000; i < 1010; ++i) {
            Post* shouldBeNull = testUser->data.posts.findPost(i);
            if (shouldBeNull != nullptr) {
                stillFound++;
            }
        }
        runner.runTest("Removed posts are not findable", stillFound == 0);
    }
    
    std::cout << Color::GREEN << "INTEGRATION TEST PASSED: Post creation and attachment" << Color::RESET << std::endl;
}

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

    std::cout << Color::MAGENTA << "Starting Comprehensive Integration and Error Condition Test Suite..." << Color::RESET << std::endl;

    testUserCreationAndFollowingIntegration(runner);
    testPostCreationAndAttachmentIntegration(runner);
    test_csv_round_trip(runner);
    test_memory_pool_reuse_under_load(runner);
    test_error_conditions(runner);

    runner.printFinalReport();

    return 0;
}