#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H
#include "main.h"

class identifier_name {
public:
    identifier_name(): ID(""), type(""), level(0), static_check("") {}
    identifier_name(string &ID, string &type, int &level,
                    string &static_check): ID(ID), type(type), level(level), static_check(static_check) {}
    string ID;
    string type;
    int level;
    string static_check;
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

    int insert_tree(const identifier_name& newID);

    void assign_tree(const string& ID, string det_type, int level);

    static identifier_node* find_max(identifier_node *node);

    void delete_node(identifier_node *node);

    void end_level(identifier_node *node, int level);

    identifier_node* look_up(const string& ID, int level);

    void print(identifier_node* &node);
};

class SymbolTable {
public:
    SymbolTable() {}

    static void run(const string& filename);
};
#endif