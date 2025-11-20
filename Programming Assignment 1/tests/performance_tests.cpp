//Stress tests for memory pool efficiency and following list traversal performance. 
#include <iostream>
#include <cassert>
#include <vector>
#include <string>
#include <chrono> 
#include <iomanip>
#include <cstdlib>
#include <ctime>

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

streambuf* originalBuffer = cout.rdbuf();
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

// ========== PERFORMANCE TESTS (Timing Focus) ==========

// Tests the raw allocation speed of the PostPool against a performance threshold.
void test_memory_pool_efficiency(TestRunner& runner) {
    runner.startSuite("Memory Pool Efficiency (Performance)");

    constexpr int NUM_ALLOCATIONS = 10000;
    constexpr int TIME_THRESHOLD_MICROSECONDS = 5000; // 5 ms threshold for 10,000 allocations
    std::cout << "\n" << Color::BLUE << "Testing " << NUM_ALLOCATIONS << " allocations..." << Color::RESET << std::endl;
    std::cout << "Performance Threshold: " << TIME_THRESHOLD_MICROSECONDS << " μs" << std::endl;

    std::vector<Post*> poolPosts;
    poolPosts.reserve(NUM_ALLOCATIONS);

    PostPool pool;

    // Time allocations from the pool
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < NUM_ALLOCATIONS; ++i) {
        poolPosts.push_back(pool.allocPost());
    }
    auto end = std::chrono::high_resolution_clock::now();

    // Calculate duration
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    // Output the results
    std::cout << Color::YELLOW << "PostPool Performance:" << Color::RESET << std::endl;
    std::cout << "  Time: " << duration.count() << " μs" << std::endl;
    std::cout << "  Time per allocation: " << std::fixed << std::setprecision(3) << (duration.count() / 1000.0) / NUM_ALLOCATIONS << " ms" << std::endl;

    // The test passes if the pool is faster than the threshold
    bool meetsPerformanceThreshold = duration.count() < TIME_THRESHOLD_MICROSECONDS;
    runner.runTest("PostPool completes bulk allocation under time threshold", meetsPerformanceThreshold);
    
    if (!meetsPerformanceThreshold) {
        std::cout << Color::RED << "  Performance threshold exceeded by " << (duration.count() - TIME_THRESHOLD_MICROSECONDS) << " μs" << Color::RESET << std::endl;
    }

    // Cleanup
    for (Post* p : poolPosts) {
        pool.freePost(p);
    }
    poolPosts.clear();

    runner.runTest("Memory Pool Efficiency test completed without crash", true);
}

// Tests the performance of traversing the FollowList for a user with many follows.
void test_follow_list_traversal_performance(TestRunner& runner) {
    runner.startSuite("Follow List Traversal Performance");

    constexpr int NUM_FOLLOWS = 10000;
    std::cout << "\n" << Color::BLUE << "Testing traversal of a list with " << NUM_FOLLOWS << " follows..." << Color::RESET << std::endl;

    UserManager um;
    PostPool pool;

    // Create the main user
    auto mainUser = um.createUser(1, "MainUser");
    runner.runTest("Main user created", mainUser != nullptr);
    if (!mainUser) return;

    // Create many users to follow
    std::cout << "Creating " << NUM_FOLLOWS << " users to follow..." << std::endl;
    cout.rdbuf(nullptr);
    for (int i = 2; i <= NUM_FOLLOWS + 1; ++i) {
        um.createUser(i, "Followee_" + std::to_string(i));
    }

    // Establish all the following relationships
    std::cout << "Establishing following relationships..." << std::endl;
    for (int i = 2; i <= NUM_FOLLOWS + 1; ++i) {
        um.follow(1, i);
    }

    // Verify the count
    int actualFollowCount = 0;
    if (mainUser->data.following) {
        auto current = mainUser->data.following->head;
        while (current != nullptr) {
            actualFollowCount++;
            current = current->next;
        }
    }
    cout.rdbuf(originalBuffer);
    runner.runTest("All follows established correctly", actualFollowCount == NUM_FOLLOWS);
    std::cout << Color::GREEN << "SUCCESS: User follows " << actualFollowCount << " users" << Color::RESET << std::endl;

    // Time the traversal of the follow list
    auto start = std::chrono::high_resolution_clock::now();
    
    int traversedCount = 0;
    if (mainUser->data.following) {
        auto current = mainUser->data.following->head;
        while (current != nullptr) {
            traversedCount++;
            // Simulate accessing user data (like checking if user is active)
            if (current->user) {
                volatile std::string dummy = current->user->userName; // Prevent optimization
            }
            current = current->next;
        }
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    // Output results
    std::cout << Color::YELLOW << "FollowList Traversal Performance:" << Color::RESET << std::endl;
    std::cout << "  Users traversed: " << traversedCount << std::endl;
    std::cout << "  Time: " << duration.count() << " μs" << std::endl;
    std::cout << "  Time per user: " << std::fixed << std::setprecision(3) << (duration.count() / 1000.0) / traversedCount << " ms" << std::endl;

    // The test "passes" if traversal time is reasonable
    bool reasonableTime = duration < std::chrono::milliseconds(50); // 50 ms threshold for 10,000 follows
    bool correctCount = (traversedCount == NUM_FOLLOWS);

    runner.runTest("FollowList traversal time is reasonable", reasonableTime);
    runner.runTest("Traversal found all followed users", correctCount);

    runner.runTest("Follow List Traversal Performance test completed without crash", true);
}

// ========== STRESS TESTS (Capacity Focus) ==========

void stress_test_large_user_list(TestRunner& runner) {
    runner.startSuite("Stress Test: Large User List + Relationships");
    UserManager um1;
    const int numUsers = 11000;
    bool usersOk = true, followOk = true;
    
    // Create a large number of users and relationships
    std::cout << "\n" << Color::BLUE << "Creating " << numUsers << " users with relationships..." << Color::RESET << std::endl;
    cout.rdbuf(nullptr);
    for (int i = 0; i < numUsers; i++) {
        um1.createUser(i, "user_" + std::to_string(i));

        if (i - 1 > 0) um1.follow(i, i - 1);
        if (i - 2 > 0) um1.follow(i, i - 2);
    }

    // Validate user creation + relationships
    for (int i = 0; i < numUsers; i++) {
        if (um1.findUserByID(i) == nullptr) {
            usersOk = false;
            break;
        }
        if (i - 1 > 0 && !um1.isFollowing(i, i - 1)) {
            followOk = false;
            break;
        }
        if (i - 2 > 0 && !um1.isFollowing(i, i - 2)) {
            followOk = false;
            break;
        }
    }
    cout.rdbuf(originalBuffer);
    runner.runTest("Large User List (" + std::to_string(numUsers) + " users)", usersOk);
    runner.runTest("Follow Relationships established correctly", followOk);
}

void stress_test_bulk_post_ingestion(TestRunner& runner) {
    runner.startSuite("Stress Test: Bulk Post Ingestion");

    IngestQueue queue(10000);
    PostPool pool;
    std::vector<Post*> posts;
    posts.reserve(10000);

    // Create posts
    for (int i = 0; i < 10000; ++i) {
        Post* p = pool.allocPost();
        p->postID = i;
        p->category = "cat" + std::to_string(i % 10);
        p->views = 0;
        posts.push_back(p);
    }

    // Enqueue into IngestQueue
    bool enqueueSuccess = true;
    for (auto* p : posts) {
        if (!queue.enqueue(p)) {
            enqueueSuccess = false;
            break;
        }
    }
    runner.runTest("Ingestion Queue Enqueue (10k posts)", enqueueSuccess);

    // Dequeue all posts from IngestQueue
    bool dequeueSuccess = true;
    for (int i = 0; i < 10000; ++i) {
        Post* p = queue.dequeue();
        if (p == nullptr) {
            dequeueSuccess = false;
            break;
        }
    }
    runner.runTest("Ingestion Queue Dequeue (10k posts)", dequeueSuccess);

    // Cleanup
    for (Post* p : posts) {
        pool.freePost(p);
    }
}

int main() {
    TestRunner runner;

    std::cout << Color::MAGENTA << "Starting Combined Performance & Stress Test Suite..." << Color::RESET << std::endl;
    std::cout << Color::MAGENTA << "Part 1: Performance Tests (Timing)" << Color::RESET << std::endl;
    std::cout << Color::MAGENTA << "Part 2: Stress Tests (Capacity)" << Color::RESET << std::endl;


    test_memory_pool_efficiency(runner);
    test_follow_list_traversal_performance(runner);
    stress_test_large_user_list(runner);
    stress_test_bulk_post_ingestion(runner);

    runner.printFinalReport();

    return 0;
}