#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H
#include "main.h"
/*
Created by Quang Nguyen
Updated on Oct, 26, 2021
*/

class ValidMachine{
public:
    ValidMachine(string str);
    ~ValidMachine();
    string line, name, type, isStatic; // For INSERT
    string returnType, param; //For insert fucntion
    string identifier, value; //For ASSIGN
    
    const regex valid_insertVar = regex ("^INSERT[ ]([a-z][a-zA-Z0-9_]*)[ ](?:string|number)[ ](?:true|false)");
    const regex valid_insertFunc = regex ("^INSERT[ ]([a-z][a-zA-Z0-9_]*)[ ]\\((?:|(?:number|string)(?:,(?:number|string))*)\\)->(?:number|string)[ ](?:true|false)$");
    const regex valid_lookup = regex  ("^LOOKUP[ ][a-z][a-zA-Z0-9_]*$");
    // const regex valid_identifer = regex ("^[a-z][a-zA-Z0-9_]*$");
    // const regex valid_number = regex  ("^\\d+$");
    // const regex valid_string = regex  ("^'[\\dA-Za-z\\s]+'$");
    const regex valid_assign  = regex {R"(^ASSIGN ([a-z]\w*) (\d+|'[\dA-Za-z\s]*'|[a-z]\w*|[a-z]\w*\((?:|(?:\d+|'[\dA-Za-z\s]*'|[a-z]\w*)(?:,(?:\d+|'[\dA-Za-z\s]*'|[a-z]\w*))*)\))$)"};
    //string getLine(){return this->line;}
    bool isInsertVar();
    bool isInsertFunc();
    bool isLookup();
    bool isAssign();
    void parseInsert(bool Func);
    void parseAssign();
    void parseLookup();

};

class Identifier{
public:
    Identifier(string name, string type);
    Identifier();
    void assignLevel(int level);
    string getName(){return this->name;}
    string getType(){return this->type;}
    string getValue(){return this->value;}
    int getLevel(){return this->level;}
    string name;
    string type;
    string value;
    string returnType; //for assign
    int level;
};

class Node{
public:
    Node( Identifier& identifier);
    Node();
    ~Node();
    Node*parent = nullptr;
    Node*left = nullptr;
    Node*right = nullptr;
    Identifier data;
    string *paramList = nullptr;
    int numParam;
};
class SplayTree{
public:
    SplayTree();
    Node *root = nullptr;
    int count;
    ~SplayTree();
    Node* find(const string& name, int level, int& num_comp);
    Node* TreeMax(Node*);
    void insert_node(Node*, Node*, int &comp);
    void deleteNodeSplay(Node*);
    void rightRotate(Node* node);
    void leftRotate(Node* node) ;
    void rightZZ(Node* node);
    void leftZZ(Node* node);
    void leftRoll(Node* node);
    void rightRoll(Node* node);
    void splay(Node* node, int &splay);
    void destroy(Node* node);
};

class LixtNode{
public:
    LixtNode();
    LixtNode(Node* data, int level);
    ~LixtNode();
    Node* data;
    int level;
    LixtNode* next = nullptr;
};

class HangDoi{
public:
    HangDoi();
    ~HangDoi();
    int size;
    LixtNode* front = nullptr;
    LixtNode* rear = nullptr;
    LixtNode* levelTrack = nullptr;
    void append(Node* data, int level);
    Node* pop_front();
    bool isEmpty();
};

class SymbolTable {
public:
    SymbolTable();
    ~SymbolTable();
    int currentLevel;
    SplayTree *tree = nullptr;
    HangDoi *hangdoi = nullptr;
    void run(string filename);
    void insert(const string& line) const;
    Node* lookup(const string& name, int destLevel,  int& num_comp);
    void assign(const string& line, int& comp, int& splay);
    void print(Node*, bool&);
    void new_scope() ;
    void end_scope();
    void handle_exception_insert(const string& line) const;
    void handle_end_file() const;
    const string process(const string& line);
    void printIn(Node*, bool& flag);
    void printPost(Node*, bool& flag);
};
#endif