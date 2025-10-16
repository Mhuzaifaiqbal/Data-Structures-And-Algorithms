#include<iostream>
using namespace std;
#include <algorithm>


struct Node {
    int key;
    Node* left;
    Node* right;
    Node(int val) {
        key = val;
        left = right = nullptr;
    }
};



Node* insert(Node* root, int key){
    if(root==nullptr){
        return new Node(key);
    }
    if(key<root->key){
        root->left=insert(root->left,key);
    }
    else{
        root->right=insert(root->right,key);
    }
    return root;
}
bool search(Node* root, int key){
    if(!root){
        return false;
    }
    if(key==root->key){
        return true;
    }
    else if(key<root->key){
        return search(root->left,key);
    }
    else{
        return search(root->right,key);
    }
}
int getheight(Node* root){
    if(!root){
        return -1;
    }
    int lefth=getheight(root->left);
    int righth=getheight(root->right);
    return 1+max(lefth,righth);
}

Node* getMin(Node* root){
    if(root==nullptr){
        return nullptr; 
    }
    while(root->left!=nullptr){
        root=root->left;
    }
    return root;
}

Node* getMax(Node* root){
    if(root==nullptr){
        return nullptr; 
    }
    while(root->right){
        root=root->right;
    }
    return root;
}

int totalNodes(Node* root){
    if(!root){
        return 0;
    }
    int leftn=totalNodes(root->left);
    int rightn=totalNodes(root->right);
    return 1+leftn+rightn;
}

Node* deletenode(Node* root, int key) {
    if (!root)
        return nullptr;

    if (key < root->key)
        root->left = deletenode(root->left, key);
    else if (key > root->key)
        root->right = deletenode(root->right, key);
    else {
        // node found
        if (!root->left && !root->right) {
            delete root;
            return nullptr;
        }
        else if (!root->left) {
            Node* temp = root->right;
            delete root;
            return temp;
        }
        else if (!root->right) {
            Node* temp = root->left;
            delete root;
            return temp;
        }
        else {
            Node* succ = getMin(root->right);
            root->key = succ->key;
            root->right = deletenode(root->right, succ->key);
        }
    }
    return root;
}

int getBalanceFactor(Node* root){
    
    return (getheight(root->left)-getheight(root->right));
        
}

void inorderTraverse(Node* root){
    if(root==nullptr){
        return;
    }
    inorderTraverse(root->left);
    cout<<root->key<<" ";
    inorderTraverse(root->right);
}
void preorderTraverse(Node* root){
    if(!root){
        return;
    }
    cout<<root->key<<" ";
    inorderTraverse(root->left);
    inorderTraverse(root->right);
}
void postorderTraverse(Node* root){
    if(!root){
        return;
    }
    inorderTraverse(root->left);
    inorderTraverse(root->right);
    cout<<root->key<<" ";

}

int main() {
    Node* root = nullptr;

    cout << "Inserting nodes into AVL Tree...\n";
    int keys[] = {9, 10, 12, 13, 14, 15, 16, 17};
    for (int key : keys)
        root = insert(root, key);

    cout << "\nInorder Traversal (sorted): ";
    inorderTraverse(root);

    cout << "\nPreorder Traversal: ";
    preorderTraverse(root);

    cout << "\nPostorder Traversal: ";
    postorderTraverse(root);

    cout << "\n\nTree height: " << getheight(root);
    cout << "\nTotal nodes: " << totalNodes(root);
    cout << "\nBalance factor of root (" << root->key << "): " << getBalanceFactor(root);

    cout << "\nMin value: " << getMin(root)->key;
    cout << "\nMax value: " << getMax(root)->key;

    cout << "\n\nSearching 15: " << (search(root, 15) ? "Found" : "Not Found");
    cout << "\nSearching 50: " << (search(root, 50) ? "Found" : "Not Found");

    cout << "\n\nDeleting 14...\n";
    root = deletenode(root, 14);

    cout << "Inorder after deletion: ";
    inorderTraverse(root);
    cout << "\nTree height after deletion: " << getheight(root);

    cout << "\n\nDeleting 16...\n";
    root = deletenode(root, 16);

    cout << "Inorder after deletion: ";
    inorderTraverse(root);
    cout << "\nTree height after deletion: " << getheight(root);

    cout << "\n\nAll operations successfully tested!\n";
    return 0;
}

