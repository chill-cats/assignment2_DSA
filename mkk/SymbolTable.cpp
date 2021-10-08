#include "SymbolTable.h"

void SymbolTable::run(const string& filename) {
    Tree splay_tree;
    int level = 0;

    regex insert(R"(^INSERT ([a-z]\w*)[ ](\(((string|number)(,(string|number))*)*\)->(string|number)|string|number)[ ](true|false)$)");
    regex assign(R"(^ASSIGN ([a-z]\w*) (([a-z]\w*)\(('[a-zA-Z\d ]*'|\d+|[a-z]\w*)(,('[a-zA-Z\d ]*'|\d+|[a-z]\w*))*\)|'[a-zA-Z\d ]*'|\d+)$)");
    regex lookup(R"(^LOOKUP[ ]([a-z]\w*))");

    ifstream myfile;
    myfile.open(filename);
    if (myfile.is_open()) {
        string s;
        while (getline(myfile, s)) {
            smatch data;

            if (regex_match(s, data, insert)) {
                identifier_name newID(data[1], data[2], level, data[8], 0);
                auto *new_node = splay_tree.insert_tree(newID);
                if (new_node->data.type == "invaliddeclaration") {
                    throw InvalidDeclaration(s);
                } else if (new_node->data.type == "redeclared") {
                    throw Redeclared(s);
                }
                int num_splay = splay_tree.splay(new_node);
                cout << new_node->data.num_com << " " << num_splay << endl;

            } else if (regex_match(s, data, assign)) {
                string out = splay_tree.assign_tree(data[1], data[2], level);
                if (out == "mismatch") {
                    throw TypeMismatch(s);
                }
                if (out == "undeclared") {
                    throw Undeclared(s);
                }
                cout << out << endl;
            } else if (s == "BEGIN") {
                level++;

            } else if (s == "END") {
                if (level == 0) {
                    throw UnknownBlock();
                }
                splay_tree.end_level(splay_tree.root, level);
                level--;

            } else if (s == "PRINT") {
                splay_tree.print(splay_tree.root);
                cout << endl;

            } else if (regex_match(s, data, lookup)) {
                if (splay_tree.look_up(data[1], level) == nullptr) {
                    throw Undeclared(s);
                } else {
                    auto *tmp = splay_tree.look_up(data[1], level);
                    cout << tmp->data.level << endl;
                    splay_tree.splay(tmp);
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
        this->root = h;
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
    this->root = h;                                                         // update root
    return count;
}

identifier_node* Tree::insert_tree(const identifier_name& newID) {           // num_com
    int num_compare = 0;
    auto *new_node = new identifier_node(newID, nullptr, nullptr, nullptr);
    if (this->root == nullptr) {
        this->root = new_node;
        return new_node;
    }

    if (newID.static_check == "true") {                         //check if static true ? false
        new_node->data.level = 0;
    }

    if ((newID.type != "string" && newID.type != "number") && newID.level != 0) {
        new_node->data.type = "invaliddeclaration";
        return new_node;
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
                new_node->data.type = "redeclared";
                return new_node;
            } else if (newID.ID.compare(h->data.ID) < 0) {
                if (h->left_child == nullptr) {
                    h->left_child = new_node;
                    new_node->parent = h;
                    break;
                }
                h = h->left_child;

            } else if (newID.ID.compare(h->data.ID) > 0) {
                if (h->right_child == nullptr) {
                    h->right_child = new_node;
                    new_node->parent = h;
                    break;
                }
                h = h->right_child;
            }
        }
    }
    new_node->data.num_com = num_compare;
    return new_node;
}

string type_of_value(const string& value) {
    regex number(R"(\d+)");
    regex string(R"('[a-zA-Z\d ]*')");
    regex id(R"([a-z]\w*)");
    if (regex_match(value, number))
        return "number";
    if (regex_match(value, string))
        return "string";
    if (regex_match(value, id))
        return "id";
    return "invalid";
}

string Tree::assign_tree(const string& ID, const string& value, int level) {
    if (this->root == nullptr) {
        return "undeclared";
    }

    regex function(R"(([a-z]\w*)(\(('[a-zA-Z\d ]*'|\d+|[a-z]\w*)(,('[a-zA-Z\d ]*'|\d+|[a-z]\w*))*\))$)");
    smatch data;
    if (regex_match(value, data, function)) {           // for function
        auto *find = look_up(data[1], 0);         // find func name
        if (find == nullptr) {
            return "undeclared";
        }
        string func = find->data.type;   //get original func
        if (func == "string" || func == "number") {
            return "mismatch";
        }

        LinkedLisst Lisst;                                  // lisst of type
        for (int i = 0; i < (int) func.length(); i++) {     // token (type),(type),(type)
            if (func[i] != ',' && func[i] != '(' && func[i] != '-' && func[i] != '>') {
                int j;
                for (j = i + 1; j < (int) func.length(); j++) {
                    if (func[j] == ',' || func[j] == ')') {
                        break;
                    }
                }
                Lisst.insert_lisst(func.substr(i, j - i));
                i = i + (j - i);
            }
        }                                                   // lisst tail is the result if right check for all variable(s)
        string var = data[2];
        LinkedLisst Lisst1;                                 // lisst of value(s)
        for (int i = 0; i < (int) var.length(); i++) {
            if (var[i] != ',' && var[i] != '(') {
                int j;
                for (j = i + 1; j < (int) var.length(); j++) {
                    if (var[j] == ',' || var[j] == ')') {
                        break;
                    }
                }
                Lisst1.insert_lisst(var.substr(i, j - i));
                i = i + (j - i);
            }
        }
        if (Lisst1.size != Lisst.size - 1) {                // if number of variables is not the same
            return "mismatch";
        }
        auto *h = Lisst.head, *h1 = Lisst1.head;
        int count = 0;
        for (int i = 0; i < Lisst1.size; i++) {
            if (type_of_value(h1->data) == "ID") {
                auto *tmp = this->look_up(h1->data, level);
                if (tmp == nullptr) {
                    return "undeclared";
                } else {
                    if (tmp->data.type != h->data) {
                        return "mismatch";
                    }
                }
                count = count + tmp->data.num_com;
            } else if (type_of_value(h1->data) != h->data) {
                return "mismatch";
            }
            h = h->next;
            h1 = h1->next;
        }
        auto *node = this->look_up(ID, level);              // node assigned
        if (node == nullptr) {                              // if invalid node
            return "undeclared";
        }
        if (node->data.type != Lisst.tail->data) {          // if type not same
            return "mismatch";
        }
        int comp = find->data.num_com + node->data.num_com + count;
        int splay = this->splay(node);
        string output = to_string(comp) + " " + to_string(splay);
        return output;
    } else {                                                // if not function
        auto *look = this->look_up(ID, level);              // ID assigned
        if (look == nullptr) {                              // if no ID
            return "undeclared";
        }
        int count1 = 0;
        if (type_of_value(value) == "ID") {                 // value is another ID
            auto *tmp1 = this->look_up(value, level);
            if (tmp1 == nullptr) {                          // if no ID assign
                return "undeclared";
            }
            if (tmp1->data.type != look->data.type) {       // if ID assign and ID assigned type not same
                return "mismatch";
            }
            count1 = count1 + tmp1->data.num_com;
        } else {
            if (type_of_value(value) != look->data.type) {
                return "mismatch";
            }
        }
        int splay1 = this->splay(look);
        string output1 = to_string(look->data.num_com + count1) + " " + to_string(splay1);
        return output1;
    }
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
        auto *max = find_max(node);                         // max is the biggest member// |  swap node and
        node->data = max->data;                             // |    update data
        node = max;                                         // |
        delete_node(node);                                  // run delete_node again
    }
}

void Tree::end_level(identifier_node *node, int level) {
    if (node == nullptr) {
        return;
    }

    end_level(node->left_child, level);

    end_level(node->right_child, level);

    if (node->data.level == level) {
        delete_node(node);
    }
}

identifier_node* Tree::look_up(const string& ID, int level) {
    if (this->root == nullptr) {
        return nullptr;
    }

    int count = 0;
    for (auto *h = this->root; ; ) {
        if (level > h->data.level) {                            //for level higher root
            if (h->right_child == nullptr) {
                break;
            }
            h = h->right_child;
            count++;

        } else if (level < h->data.level) {                     //for level lower
            if (h->left_child == nullptr) {
                break;
            }
            h = h->left_child;
            count++;

        } else {                                                //compare ID
            if (ID == h->data.ID) {
                count++;
                h->data.num_com = count;
                return h;

            } else if (ID.compare(h->data.ID) < 0) {
                if (h->left_child == nullptr) {
                    break;
                }
                h = h->left_child;
                count++;

            } else if (ID.compare(h->data.ID) > 0) {
                if (h->right_child == nullptr) {
                    break;
                }
                h = h->right_child;
                count++;
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

    print(node->left_child);

    print(node->right_child);
}