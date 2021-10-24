#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H
#include "main.h"

class Node {
public:
    Node(string ID, string type, int level, Node* next, Node* prev): ID(std::move(ID)), type(std::move(type)), level(level), next(next), prev(prev) {}
    string ID;
    string type;
    int level;
    Node *next;
    Node *prev;
};

class LinkedLisst {
public:
    Node* head;
    Node* tail{};
    int size;
public:
    LinkedLisst(): head(nullptr), tail(nullptr), size(0) {};

    ~LinkedLisst() {
        while (this->head) {
            auto *h = this->head;
            this->head = this->head->next;
            delete h;
        }
    };

    void insert_lisst(string ID, string type, int level);
    void delete_level(const int& level);
};

class identifier_name {
public:
    identifier_name(): ID(""), type(""), level(0), static_check(""), num_com(0), num_splay(0) {}
    identifier_name(string ID, string type, int level,
                    string static_check, int num_com, int num_splay):
            ID(ID), type(type), level(level), static_check(static_check), num_com(num_com), num_splay(num_splay) {}
    string ID;
    string type;
    int level;
    string static_check;
    int num_com;
    int num_splay;
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

    ~Tree() {
        delete_tree(this->root);
        this->root = nullptr;
    }
public:

    static void right_rol(identifier_node* &h);
    static void left_rol(identifier_node* &h);
    int splay(identifier_node* &h);
    string insert_tree(identifier_name &newID, int &num_com);
    string assign_tree(LinkedLisst& lisst, const string& ID, const string& value, int& level);
    void delete_tree(identifier_node* &node);
    static identifier_node* find_max(identifier_node* &node);
    void end_level(LinkedLisst &lisst, int level);
    string look_up(const string& ID, int level, int count);
    void print(identifier_node* &node);
};

class SymbolTable {
public:
    static void run(const string& filename);
};
#endif