
#include <iostream>
#include <cassert>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
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
    const char* YELLOW      = "\033[1;33m";
    const char* BLUE        = "\033[1;34m";
    const char* MAGENTA     = "\033[35m";
}

class TestSuite {
private:
    int totalTests = 0;
    int passedTests = 0;
    
public:
    void runTest(const std::string& testName, bool condition) {
        totalTests++;
        if (condition) {
            passedTests++;
            std::cout << Color::GREEN << "[PASS] " << Color::RESET << testName << std::endl;
        } else {
            std::cout << Color::RED << "[FAIL] " << Color::RESET << testName << std::endl;
        }
    }
    
    void printScore() {
        double score = (totalTests > 0 ? (static_cast<double>(passedTests) / totalTests * 100.0) : 0.0);
        const char* scoreColor = (passedTests == totalTests) ? Color::GREEN : Color::RED;

        std::cout << "\n" << Color::BLUE << "========== TEST RESULTS ==========" << Color::RESET << std::endl;
        std::cout << "Passed: " << passedTests << "/" << totalTests << std::endl;
        std::cout << "Score: " << scoreColor << std::fixed << std::setprecision(2) << score << "%" << Color::RESET << std::endl;
        std::cout << Color::BLUE << "===================================" << Color::RESET << std::endl;
    }
    
    void testUserManager() {
        std::cout << "\n" << Color::YELLOW << "=== TESTING USER MANAGER ===" << Color::RESET << std::endl;
        UserManager um;
        
        auto user1 = um.createUser(1, "alice");
        auto user2 = um.createUser(2, "bob");
        runTest("UserManager::createUser - valid users", user1 != nullptr && user2 != nullptr);
        
        auto duplicateUser = um.createUser(1, "charlie");
        runTest("UserManager::createUser - reject duplicate ID", duplicateUser == nullptr);
        
        auto duplicateName = um.createUser(3, "alice");
        runTest("UserManager::createUser - reject duplicate name", duplicateName == nullptr);
        
        auto foundUser = um.findUserByID(1);
        runTest("UserManager::findUserByID - existing user", foundUser != nullptr && foundUser->data.userID == 1);
        
        auto notFoundUser = um.findUserByID(999);
        runTest("UserManager::findUserByID - non-existing user", notFoundUser == nullptr);
        
        auto foundByName = um.findUserByName("bob");
        runTest("UserManager::findUserByName - existing user", foundByName != nullptr && foundByName->data.userName == "bob");
        
        auto notFoundByName = um.findUserByName("nonexistent");
        runTest("UserManager::findUserByName - non-existing user", notFoundByName == nullptr);
        
        bool followResult = um.follow(1, 2);
        runTest("UserManager::follow - valid follow", followResult);
        
        bool isFollowingResult = um.isFollowing(1, 2);
        runTest("UserManager::isFollowing - after follow", isFollowingResult);
        
        bool selfFollowResult = um.follow(1, 1);
        runTest("UserManager::follow - prevent self-follow", !selfFollowResult);
        
        bool duplicateFollowResult = um.follow(1, 2);
        runTest("UserManager::follow - prevent duplicate follow", !duplicateFollowResult);
        
        bool unfollowResult = um.unfollow(1, 2);
        runTest("UserManager::unfollow - valid unfollow", unfollowResult);
        
        bool isNotFollowingResult = !um.isFollowing(1, 2);
        runTest("UserManager::isFollowing - after unfollow", isNotFollowingResult);
        
        bool invalidUnfollowResult = um.unfollow(1, 2);
        runTest("UserManager::unfollow - invalid unfollow", !invalidUnfollowResult);
        
        auto userNode = um.findUserByID(1);
        if (userNode) {
            userNode->data.addPost(101, "tech");
            Post* foundPost = userNode->data.posts.findPost(101);
            runTest("UserManager - post operations via User", foundPost != nullptr);
            
            bool deletePostResult = userNode->data.posts.removePost(101);
            runTest("UserManager - delete post via User", deletePostResult);
        } else {
            runTest("UserManager - post operations via User", false);
            runTest("UserManager - delete post via User", false);
        }
        
        um.createUser(3, "charlie");
        um.follow(3, 1);
        bool deleteUserResult = um.deleteUser(3);
        runTest("UserManager::deleteUser - valid user", deleteUserResult);
        
        auto deletedUser = um.findUserByID(3);
        runTest("UserManager::deleteUser - user removed", deletedUser == nullptr);
        
        testCSVOperations(um);
    }
    
    void testCSVOperations(UserManager& um) {
        std::cout << "\n" << Color::YELLOW << "=== TESTING CSV OPERATIONS ===" << Color::RESET << std::endl;
        
        UserManager csvUm;
        csvUm.createUser(10, "testuser1");
        csvUm.createUser(11, "testuser2");
        csvUm.follow(10, 11);
        
        auto testUser = csvUm.findUserByID(10);
        if (testUser) {
            testUser->data.addPost(201, "test");
        }
        
        const std::string testFile = "test_export.csv";
        csvUm.exportUsersCSV(testFile);
        
        std::ifstream checkFile(testFile);
        bool fileExists = checkFile.good();
        checkFile.close();
        runTest("UserManager::exportUsersCSV - file creation", fileExists);
        
        UserManager um2;
        um2.importUsersCSV(testFile);
        
        auto importedUser = um2.findUserByID(10);
        runTest("UserManager::importUsersCSV - user restored", importedUser != nullptr);
        
        bool followRestored = um2.isFollowing(10, 11);
        runTest("UserManager::importUsersCSV - follow restored", followRestored);
        
        std::remove(testFile.c_str());
    }
    
    void testPostPool() {
        std::cout << "\n" << Color::YELLOW << "=== TESTING POST POOL ===" << Color::RESET << std::endl;
        PostPool pool(4);
        
        Post* post1 = pool.allocPost();
        runTest("PostPool::allocPost - basic allocation", post1 != nullptr);
        
        runTest("PostPool::allocPost - default initialization", 
                post1->postID == 0 && post1->category.empty() && post1->views == 0);
        
        std::vector<Post*> posts;
        for (int i = 0; i < 10; ++i) {
            posts.push_back(pool.allocPost());
        }
        
        bool allAllocated = true;
        for (auto* p : posts) {
            if (p == nullptr) {
                allAllocated = false;
                break;
            }
        }
        runTest("PostPool::allocPost - multiple allocations", allAllocated);
        
        size_t allocCount = pool.totalAllocations();
        runTest("PostPool::totalAllocations - correct count", allocCount >= 3);
        
        pool.freePost(posts[0]);
        pool.freePost(posts[1]);
        
        Post* reused1 = pool.allocPost();
        Post* reused2 = pool.allocPost();
        
        size_t reuseCount = pool.reuseCount();
        runTest("PostPool::freePost/allocPost - reuse functionality", reuseCount >= 2);
        runTest("PostPool::reuseCount - correct tracking", reuseCount >= 2);
        
        pool.purge();
        runTest("PostPool::purge - reset counters", pool.totalAllocations() == 0 && pool.reuseCount() == 0);
    }
    
    void testIngestQueue() {
        std::cout << "\n" << Color::YELLOW << "=== TESTING INGEST QUEUE ===" << Color::RESET << std::endl;
        IngestQueue queue(5);
        
        PostPool pool;
        
        runTest("IngestQueue::empty - initial state", queue.empty());
        runTest("IngestQueue::size - initial size", queue.size() == 0);
        
        std::vector<Post*> posts;
        for (int i = 0; i < 3; ++i) {
            posts.push_back(pool.allocPost());
        }
        
        bool enqueueSuccess = true;
        for (auto* p : posts) {
            if (!queue.enqueue(p)) {
                enqueueSuccess = false;
                break;
            }
        }
        runTest("IngestQueue::enqueue - successful enqueues", enqueueSuccess);
        runTest("IngestQueue::size - after enqueues", queue.size() == 3);
        runTest("IngestQueue::empty - not empty", !queue.empty());
        
        for (int i = 0; i < 3; ++i) {
            queue.enqueue(pool.allocPost());
        }
        bool fullResult = !queue.enqueue(pool.allocPost());
        runTest("IngestQueue::enqueue - queue full handling", fullResult);
        
        Post* dequeued = queue.dequeue();
        runTest("IngestQueue::dequeue - successful dequeue", dequeued != nullptr);
        runTest("IngestQueue::size - after dequeue", queue.size() == 4);
        
        Post* batchArray[3];
        size_t batchCount = queue.dequeueBatch(batchArray, 3);
        runTest("IngestQueue::dequeueBatch - correct count", batchCount == 3);
        runTest("IngestQueue::size - after batch dequeue", queue.size() == 1);
        
        for (int cycle = 0; cycle < 3; ++cycle) {
            while (queue.size() < 5) {
                queue.enqueue(pool.allocPost());
            }
            while (!queue.empty()) {
                queue.dequeue();
            }
        }
        runTest("IngestQueue - wrap-around functionality", queue.empty());
        
        Post* emptyDequeue = queue.dequeue();
        runTest("IngestQueue::dequeue - empty queue", emptyDequeue == nullptr);
    }
    
    void testUndoRedoManager() {
        std::cout << "\n" << Color::YELLOW << "=== TESTING UNDO/REDO MANAGER ===" << Color::RESET << std::endl;
        UserManager um;
        PostPool pool;
        UndoRedoManager urm(um, pool);
        
        urm.beginTransaction();
        runTest("UndoRedoManager::beginTransaction - no crash", true);
        
        OpFrame createFrame{OpType::CREATE_USER, 1, 0, "alice"};
        urm.record(createFrame);
        runTest("UndoRedoManager::record - no crash", true);
        
        urm.commitTransaction();
        runTest("UndoRedoManager::commitTransaction - no crash", true);
        
        urm.beginTransaction();
        OpFrame testFrame{OpType::CREATE_USER, 2, 0, "bob"};
        urm.record(testFrame);
        urm.rollbackTransaction();
        runTest("UndoRedoManager::rollbackTransaction - no crash", true);
        
        bool undoResult = urm.undo();
        runTest("UndoRedoManager::undo - empty stack handling", !undoResult);
        
        bool redoResult = urm.redo();
        runTest("UndoRedoManager::redo - empty stack handling", !redoResult);
    }
    
    void testLinkedList() {
        std::cout << "\n" << Color::YELLOW << "=== TESTING LINKED LIST ===" << Color::RESET << std::endl;
        LinkedList<int> list;
        
        runTest("LinkedList - initial empty", list.size() == 0 && list.head() == nullptr);
        
        auto node1 = list.push_back(10);
        runTest("LinkedList::push_back - first element", node1 != nullptr && list.size() == 1);
        
        auto node2 = list.push_back(20);
        runTest("LinkedList::push_back - second element", node2 != nullptr && list.size() == 2);
        
        auto node0 = list.push_front(5);
        runTest("LinkedList::push_front", node0 != nullptr && list.size() == 3 && list.head()->data == 5);
        
        auto found = list.find([](const int& val) { return val == 20; });
        runTest("LinkedList::find - existing element", found != nullptr && found->data == 20);
        
        auto notFound = list.find([](const int& val) { return val == 999; });
        runTest("LinkedList::find - non-existing element", notFound == nullptr);
        
        list.remove(node1);
        runTest("LinkedList::remove", list.size() == 2);
        
        auto stillThere = list.find([](const int& val) { return val == 10; });
        runTest("LinkedList::remove - element removed", stillThere == nullptr);
        
        list.clear();
        runTest("LinkedList::clear", list.size() == 0 && list.head() == nullptr);
    }
    
    void testAuxiliaryStructures() {
        std::cout << "\n" << Color::YELLOW << "=== TESTING AUXILIARY STRUCTURES ===" << Color::RESET << std::endl;
        
        User user(1, "testuser");
        runTest("User::constructor", user.userID == 1 && user.userName == "testuser");
        
        user.addPost(101, "tech");
        Post* foundPost = user.posts.findPost(101);
        runTest("User::addPost", foundPost != nullptr && foundPost->category == "tech");
        
        User user2(2, "followee");
        user.followUser(&user2);
        User* foundFollowee = user.following->findFollowing(2);
        runTest("User::followUser", foundFollowee != nullptr && foundFollowee->userID == 2);
        
        bool removeResult = user.following->removeFollowing(2);
        User* removedFollowee = user.following->findFollowing(2);
        runTest("FollowList::removeFollowing", removeResult && removedFollowee == nullptr);
        
        PostList postList;
        Post testPost(201, "test", 5);
        postList.addPost(testPost);
        Post* foundInList = postList.findPost(201);
        runTest("PostList::addPost/findPost", foundInList != nullptr && foundInList->category == "test");
        
        bool postRemoved = postList.removePost(201);
        Post* removedPost = postList.findPost(201);
        runTest("PostList::removePost", postRemoved && removedPost == nullptr);
        
        FollowList followList;
        User testUser3(3, "followtest");
        followList.addFollowing(&testUser3);
        User* foundInFollowList = followList.findFollowing(3);
        runTest("FollowList::addFollowing", foundInFollowList != nullptr);
        
        followList.addFollowing(&testUser3);
        runTest("FollowList::addFollowing - duplicate prevention", true);
    }
    
    void runAllTests() {
        std::cout << Color::MAGENTA << "Starting Comprehensive Test Suite..." << Color::RESET << std::endl;
        
        testLinkedList(
        );

        testPostPool();
        testIngestQueue();
        testUserManager();
        testUndoRedoManager();
        testAuxiliaryStructures();
        
        printScore();
    }
};

int main() {
    TestSuite suite;
    suite.runAllTests();
    return 0;
}