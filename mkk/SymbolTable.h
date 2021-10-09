#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H
#include "main.h"

class identifier_name {
public:
    identifier_name(): ID(""), type(""), level(0), static_check(""), num_com(0) {}
    identifier_name(string ID, string type, int level,
                    string static_check, int num_com):
                    ID(ID), type(type), level(level), static_check(static_check), num_com(num_com) {}
    string ID;
    string type;
    int level;
    string static_check;
    int num_com;
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
    /*
    ~Tree() {
        while (this->root) {
            delete_node(this->root);
        }
    };
*/
public:

    static identifier_node* right_rol(identifier_node* h);

    static identifier_node* left_rol(identifier_node* h);

    int splay(identifier_node* h);

    identifier_node* insert_tree(const identifier_name& newID);

    string assign_tree(const string& ID, const string& det_type, int level);

    static identifier_node* find_max(identifier_node *node);

    void delete_node(identifier_node *node);

    void end_level(identifier_node *node, int level);

    identifier_node* look_up(const string& ID, int level);

    void print(identifier_node* &node);
};

class Node {
public:
    string data;
    Node *next{};
};

class LinkedLisst {
public:
    Node* head;
    Node* tail{};
    int size;
public:
    LinkedLisst(): head(nullptr) {};
    ~LinkedLisst() {
        while (this->head) {
            auto *h = this->head;
            this->head = this->head->next;
            delete h;
        }
    };

    void insert_lisst(string data) {
        auto *new_node = new Node;
        new_node->data = move(data);
        new_node->next = nullptr;
        if (this->head == nullptr) {
            this->head = new_node;
            this->tail = this->head;
            this->size = 1;
        } else {
            this->tail->next = new_node;
            this->tail = this->tail->next;
            this->size++;
        }
    }
};

class SymbolTable {
public:
    static void run(const string& filename);
};
#endif