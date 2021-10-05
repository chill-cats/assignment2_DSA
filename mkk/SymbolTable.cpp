#include "SymbolTable.h"

void SymbolTable::run(const string& filename) {
    Tree splay_tree;
    int level = 0;

    regex insert(R"(^INSERT[ ]([a-z]\w*)[ ](string|number)$)");
    regex assign(R"(^ASSIGN[ ]([a-z]\w*)[ ]([a-z]\w*|\d+|'[a-zA-Z\d ]*')$)");
    regex lookup(R"(^LOOKUP[ ]([a-z]\w*))");

    ifstream myfile;
    myfile.open(filename);
    if (myfile.is_open()) {
        string s;
        while (getline(myfile, s)) {
            smatch data;

            if (regex_match(s, data, insert)) {
                //identifier_name newID(data[1], data[2], "", level);


            } else if (regex_match(s, data, assign)) {


            } else if (s == "BEGIN") {
                level++;

            } else if (s == "END") {
                if (level == 0) {
                    throw UnknownBlock();
                }

                level--;

            } else if (s == "PRINT") {


            } else if (regex_match(s, data, lookup)) {
                if (splay_tree.look_up(data[1], level) == nullptr) {
                    throw Undeclared(s);
                } else {
                    cout << splay_tree.look_up(data[1], level) << endl;
                }

            } else {
                throw InvalidInstruction(s);
            }
        }
        if (level != 0) {
            throw UnclosedBlock(level);
        }
    }
    myfile.close();
}

identifier_node* Tree::right_rol(identifier_node *h) {
    auto left_h = h->left_child;
    h->left_child = left_h->right_child;
    if (left_h->right_child != nullptr) {
        left_h->right_child->parent = h;
    }
    left_h->right_child = h;
    if (h->parent == nullptr) {
        h->parent = left_h;
        left_h->parent = nullptr;
    } else if (h == h->parent->right_child) {
        h->parent->right_child = left_h;
        left_h->parent = h->parent;
        h->parent = left_h;
    } else if (h == h->parent->left_child) {
        h->parent->left_child = left_h;
        left_h->parent = h->parent;
        h->parent = left_h;
    }
    return left_h;
}

identifier_node* Tree::left_rol(identifier_node *h) {
    auto right_h = h->right_child;
    h->right_child = right_h->left_child;
    if (right_h->left_child != nullptr) {
        right_h->left_child->parent = h;
    }
    right_h->left_child = h;
    if (h->parent == nullptr) {
        h->parent = right_h;
        right_h->parent = nullptr;
    } else if (h == h->parent->right_child) {
        h->parent->right_child = right_h;
        right_h->parent = h->parent;
        h->parent = right_h;
    } else if (h == h->parent->left_child) {
        h->parent->left_child = right_h;
        right_h->parent = h->parent;
        h->parent = right_h;
    }
    return right_h;
}

int Tree::splay(identifier_node* h) {                           // num_splay
    if (h->parent == nullptr) {
        return 0;
    }
    int count = 0;
    while (h->parent != nullptr) {
        if (h->parent == this->root) {
            if (h == h->parent->left_child) {
                h = this->right_rol(h->parent);
            } else {
                h = this->left_rol(h->parent);
            }
            count++;
        } else {
            auto *par = h->parent;
            auto *gra = par->parent;
            if (h == par->left_child && par == gra->left_child) {
                par = this->right_rol(gra);
                h = this->right_rol(par);
            } else if (h == par->right_child && par == gra->right_child) {
                par = this->left_rol(gra);
                h = this->left_rol(par);
            } else if (h == par->left_child && par == gra->right_child) {
                h = this->right_rol(par);
                h = this->left_rol(h->parent);
            } else {
                h = this->left_rol(par);
                h = this->right_rol(h->parent);
            }
            count++;
        }
    }
    return count;
}

int Tree::insert_tree(const identifier_name& newID) {           // num_com
    int num_compare = 0;
    auto *new_node = new identifier_node(newID, nullptr, nullptr, nullptr);
    if (this->root == nullptr) {
        this->root = new_node;
        return -1;
    }

    identifier_node *h;
    for (h = this->root; ; ) {
        num_compare++;
        if (new_node->data.level > h->data.level) {             //for level higher root
            if (h->right_child == nullptr) {
                h->right_child = new_node;
                new_node->parent = h;
                break;
            }
            h = h->right_child;

        } else if (new_node->data.level < h->data.level) {      //for level lower
            if (h->left_child == nullptr) {
                h->left_child = new_node;
                new_node->parent = h;
                break;
            }
            h = h->left_child;

        } else {                                                //compare ID
            if (newID.ID == h->data.ID) {
                return -1;
            } else if (newID.ID.compare(h->data.ID) == -1) {
                if (h->left_child == nullptr) {
                    h->left_child = new_node;
                    new_node->parent = h;
                    break;
                }
                h = h->left_child;

            } else if (newID.ID.compare(h->data.ID) == 1) {
                if (h->right_child == nullptr) {
                    h->right_child = new_node;
                    new_node->parent = h;
                    break;
                }
                h = h->right_child;
            }
        }
    }
    return num_compare;
}

void Tree::assign_tree(const string& ID, string det_type, int level) {
    auto *h = this->look_up(ID, level);                     // crisis of BK ass
    if (h != nullptr) {

    }
    //throw Undeclared(line);
}

identifier_node* Tree::find_max(identifier_node *node) {
    if (node->left_child == nullptr) {
        return nullptr;
    }
    auto *n = node->left_child;
    while (n->right_child) {
        n = n->right_child;
    }
    return n;
}

void Tree::delete_node(identifier_node *node) {
    auto *par = node->parent;
    if (node->left_child == nullptr) {                      // when reaches condition to delete
        if (node->right_child == nullptr) {                 // if it singled
            if (par == nullptr) {                           // delete root when it singled
                delete node;
                this->root = nullptr;
            } else if (par->left_child == node) {
                delete node;
                par->left_child = nullptr;
            } else {
                delete node;
                par->right_child = nullptr;
            }
        } else {                                            // if it has right child
            if (par == nullptr) {                           // delete root when it not singled
                this->root = node->right_child;
                delete node;
            } else if (par->left_child == node) {
                par->left_child = node->right_child;
                node->right_child->parent = par;
                delete node;
            } else {
                par->right_child = node->right_child;
                node->right_child->parent = par;
                delete node;
            }
        }
    } else {
        auto *max = find_max(node);                         // max is the biggest member
        auto tmp = max->data;                               // |
        max->data = node->data;                             // |  swap node and
        node->data = tmp;                                   // |    run delete_node again
        node = max;                                         // |
        delete_node(node);
    }
}

void Tree::end_level(identifier_node *node, int level) {
    if (node == nullptr) {
        return;
    }

    if (node->data.level == level) {
        delete_node(node);
    }
    // run till left_ end
    end_level(node->left_child, level);

    // run till right_ end
    end_level(node->right_child, level);
}

identifier_node* Tree::look_up(const string& ID, int level) {   // NEED MORE INFORMATION~~~~~~~~~~~~~
    if (this->root == nullptr) {
        return nullptr;
    }

    for (auto *h = this->root; ; ) {
        if (level > h->data.level) {                            //for level higher root
            if (h->right_child == nullptr) {
                break;
            }
            h = h->right_child;

        } else if (level < h->data.level) {                     //for level lower
            if (h->left_child == nullptr) {
                break;
            }
            h = h->left_child;

        } else {                                                //compare ID
            if (ID == h->data.ID) {
                return h;

            } else if (ID.compare(h->data.ID) == -1) {
                if (h->left_child == nullptr) {
                    break;
                }
                h = h->left_child;

            } else if (ID.compare(h->data.ID) == 1) {
                if (h->right_child == nullptr) {
                    break;
                }
                h = h->right_child;
            }
        }
    }
    if (level == 0) {
        return nullptr;
    } else {
        return look_up(ID, level - 1);
    }
}

void Tree::print(identifier_node* &node) {
    if (node == nullptr) {
        return;
    }

    if (node == this->root) {
        cout << node->data.ID << "//" << node->data.level;
    } else {
        cout << " " << node->data.ID << "//" << node->data.level;
    }
    // run till left is null
    print(node->left_child);

    // return to pre step and print in right
    print(node->right_child);
}