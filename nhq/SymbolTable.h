#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H
#include "main.h"

class Identifier{
public:
    Identifier(string name, string type, string value, string returnType);
    Identifier();
    ~Identifier();
    void setLevel(int level);
    void setName(string _name){this->name = _name;}
    void setValue(string _value){this->value = _value;}
    void setType(string _type){this->type = _type;}
    void setRType(string _return){this->returnType =_return;}
    string getName(){return this->name;}
    string getType(){return this->type;}
    string getValue(){return this->value;}
    int getLevel(){return this->level;}
    
protected:
    string name;
    string type;
    string value;
    string returnType;
    int level;
};

class Node{
public:
    Node(const Identifier& identifier);
    Node();
    ~Node();
    Node* getParent() const;
    Node* getLeft() const;
    Node* getRight() const;
    void setLeft(Node*);
    void setRight(Node*);
    void setParent(Node*);
    Identifier getData() const;
    void setData(Identifier identifier){this->data = identifier;}
private:
    Node*parent = nullptr;
    Node*left = nullptr;
    Node*right = nullptr;
    Identifier data;
    
};

class SplayTree{
public:
    SplayTree();
    Node *root = nullptr;
    ~SplayTree();
    Node*find(string name, Node*root);
    Node* TreeMin(Node*);
    Node* TreeMax(Node*);
    Node* TreeSucc(Node*);
    Node* TreeProc(Node*);

    void insert_node(Node*, Node*, int &comp);
    void addLeft(Node*&, Node*&);
    void addRight(Node*&, Node*&);
    void deleteNode(Node*);
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
    friend class SplayTree;
    SymbolTable();
    ~SymbolTable();
    int currentLevel;
    SplayTree *tree = nullptr;
    void run(string filename);
    void insert(const string& line) const;
    void lookup(const string& line, Node*, bool&);
    void assign(const string& line, int type);
    void print(Node*, bool& flag) const;
    void new_scope() ;
    void end_scope(Node* );

    void handle_exception_insert(const string& line, Node* node);
    int handle_exception_assign(const string& line);
    void handle_end_file() const;
    pair<string,int> process(const string& line);

};
#endif