#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H
#include "main.h"

class identifier_name {
public:
    identifier_name(): ID(""), type(""), level(0) {}
    identifier_name(string ID, string type, string value, int level): ID(ID), type(type), level(level) {}
    string ID;
    string type;
    int level;
};

class identifier_node {
public:
    identifier_node(): data(identifier_name()), parent(nullptr), left_child(nullptr), right_child(nullptr) {}
    identifier_node(identifier_name data, identifier_node* left_child,
                    identifier_node* right_child, identifier_node* parent):
                    data(data), left_child(left_child), right_child(right_child), parent(parent) {}

    identifier_name data;
    identifier_node* parent;
    identifier_node* left_child;
    identifier_node* right_child;
};

class Tree {
public:
    identifier_node* root;

public:
    Tree(): root(nullptr) {};

    ~Tree() {};
public:

    static identifier_node* right_rol(identifier_node* h);

    static identifier_node* left_rol(identifier_node* h);

    int splay(identifier_node* h);

    void insert_tree(const identifier_name& newID, const string& line);

    void assign_tree(string ID, string det_type, string value, string line);

    void end_level(int level);

    int look_up(const string& ID, int level);

    void print(identifier_node* &node);
};

class SymbolTable {
public:
    SymbolTable() {}

    static void run(const string& filename);
};
#endif