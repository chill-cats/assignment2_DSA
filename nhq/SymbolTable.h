#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H
#include "main.h"

class Identifier{
public:
    Identifier(string name, string type, string value);
    Identifier();
    ~Identifier();
    string name;
    string type;
    string value;
    bool isStatic{};
};

class Node{
public:
    Node(const Identifier& identifier);
    Node();
    ~Node();
    Node*parent = nullptr;
    Node*left = nullptr;
    Node*right = nullptr;
    int level;
    Identifier data;
    void setLevel(int level);
};

class SplayTree{
public:
    SplayTree();
    Node *root = nullptr;
    ~SplayTree();
    Node*find(string name, Node*root);
    void insert_node(Node*, Node*, int &comp);
    void deleteNode(Node*&);
    Node* TreeMin(Node*);
    Node* TreeMax(Node*);
    Node* TreeSucc(Node*);
    Node* TreeProc(Node*);
    void rightRotate(Node* node);
    void leftRotate(Node* node) ;
    void rightZZ(Node* node);
    void leftZZ(Node* node);
    void leftRoll(Node* node);
    void rightRoll(Node* node);
    void splay(Node* node, int &splay);
    void destroy(Node* node);
    void inorder(Node* root); //Testing
};
class SymbolTable {
public:
    SymbolTable();
    ~SymbolTable();
    int currentLevel;
    SplayTree *tree = nullptr;
    void run(const string& filename);
    void insert(string line);
    void lookup(string line) const;
    void assign(string line, int type);
    void print(Node*);
    void new_scope();
    void end_scope(Node* start);

    void handle_exception_insert(string line, Node* node);
    int handle_exception_assign(string line);
    void handle_end_file();
    void cleanup();
    pair<string,int> process(string line);

};
#endif