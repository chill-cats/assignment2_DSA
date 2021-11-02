#include "SymbolTable.h"

void SymbolTable::run(const string& filename) {
    Tree splay_tree;
    splay_tree.root = nullptr;
    LinkedLisst tracking_lisst;
    tracking_lisst.head = nullptr;
    int level = 0;

    regex insert(R"(^INSERT ([a-z]\w*)[ ](\(((string|number)(,(string|number))*)*\)->(string|number)|string|number)[ ](true|false)$)");
    regex assign(R"(^ASSIGN ([a-z]\w*) (([a-z]\w*)\((('[a-zA-Z\d ]*'|\d+|[a-z]\w*)(,('[a-zA-Z\d ]*'|\d+|[a-z]\w*))*)*\)|'[a-zA-Z\d ]*'|\d+|[a-z]\w*)$)");
    regex lookup(R"(^LOOKUP[ ]([a-z]\w*))");

    ifstream myfile;
    myfile.open(filename);
    if (myfile.is_open()) {
        string s;
        while (getline(myfile, s)) {
            smatch data;

            if (regex_match(s, data, insert)) {
                identifier_name newID(data[1], data[2], level, data[8], 0, 0);
                string out = splay_tree.insert_tree(newID);

                if (out == "invaliddeclaration")    throw InvalidDeclaration(s);
                else if (out == "redeclared")       throw Redeclared(s);

                cout << splay_tree.root->data.num_com << " " << splay_tree.root->data.num_splay << endl;
                tracking_lisst.insert_lisst(splay_tree.root->data.ID, splay_tree.root->data.level);

            } else if (regex_match(s, data, assign)) {
                string out = splay_tree.assign_tree(tracking_lisst, data[1], data[2], level);

                if (out == "mismatch")              throw TypeMismatch(s);
                if (out == "undeclared")            throw Undeclared(s);

                cout << out << endl;

            } else if (s == "BEGIN") {
                level++;

            } else if (s == "END") {
                if (level == 0)                     throw UnknownBlock();

                splay_tree.end_level(tracking_lisst, level);
                tracking_lisst.delete_level(level);
                level--;

            } else if (s == "PRINT") {
                if (splay_tree.root != nullptr) {
                    splay_tree.print(splay_tree.root);
                    cout << endl;
                }

            } else if (regex_match(s, data, lookup)) {
                string out = splay_tree.look_up(data[1], level);

                if (out == "no")                    throw Undeclared(s);

                else cout << splay_tree.root->data.level << endl;

            } else                                  throw InvalidInstruction(s);
        }
        if (level != 0)                             throw UnclosedBlock(level);
    }
    myfile.close();
}

void Tree::right_rol(identifier_node* &h) {
    auto left_h = h->left_child;
    h->left_child = left_h->right_child;
    if (left_h->right_child != nullptr)     left_h->right_child->parent = h;
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
}

void Tree::left_rol(identifier_node* &h) {
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
}

int Tree::splay(identifier_node* &h) {                           // num_splay
    if (h->parent == nullptr)       return 0;

    while (h->parent != nullptr) {
        if (h->parent == this->root) {
            auto *par = h->parent;
            if (h == par->left_child) {
                this->right_rol(par);
                h = par->parent;
            } else {
                this->left_rol(par);
                h = par->parent;
            }
        } else {
            auto *par = h->parent;
            auto *gra = par->parent;
            if (h == par->left_child && par == gra->left_child) {
                this->right_rol(gra);
                this->right_rol(par);
            } else if (h == par->right_child && par == gra->right_child) {
                this->left_rol(gra);
                this->left_rol(par);
            } else if (h == par->left_child && par == gra->right_child) {
                this->right_rol(par);
                this->left_rol(gra);
            } else if (h == par->right_child && par == gra->left_child) {
                this->left_rol(par);
                this->right_rol(gra);
            }
        }
    }
    this->root = h;                                                         // update root
    return 1;
}

string Tree::insert_tree(identifier_name &newID) {
    if (newID.static_check == "true") {                         //check if static true ? false
        newID.level = 0;
    }

    if ((newID.type != "string" && newID.type != "number") && newID.level != 0) {
        return "invaliddeclaration";
    }

    auto *new_node = new identifier_node(newID, nullptr, nullptr, nullptr);
    if (this->root == nullptr) {
        this->root = new_node;
        return "success";
    }
    int num_com = 0;
    identifier_node *h;
    for (h = this->root; ; ) {
        num_com++;
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
                delete new_node;
                return "redeclared";
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
    new_node->data.num_com = num_com;
    new_node->data.num_splay = this->splay(new_node);
    return "success";
}

string type_of_value(const string& value) {
    regex number(R"(\d+)");
    regex string(R"('[a-zA-Z\d ]*')");
    regex id(R"([a-z]\w*)");
    if (regex_match(value, number))         return "number";
    if (regex_match(value, string))         return "string";
    if (regex_match(value, id))             return "ID";
    return "invalid";
}

void LinkedLisst::insert_lisst(string ID, int level) {
    if (level == 0) return;

    auto *new_node = new Node(move(ID), level, nullptr, nullptr);
    if (this->head == nullptr) {
        this->head = new_node;
        this->tail = this->head;
        this->curent_level = this->head;
        this->size = 1;
        return;
    }
    this->size++;
    if (this->head->level != level) {
        new_node->next = this->head;
        this->head->prev = new_node;
        this->head = this->head->prev;
        this->curent_level = this->head;
    } else {
        if (this->curent_level == this->tail) {
            this->tail->next = new_node;
            new_node->prev = this->tail;
            this->tail = this->tail->next;
            this->curent_level = this->tail;
        } else {
            auto *tmp = this->curent_level->next;
            this->curent_level->next = new_node;
            new_node->next = tmp;
            tmp->prev = new_node;
            new_node->prev = this->curent_level;
            this->curent_level = this->curent_level->next;
        }
    }
}

void LinkedLisst::delete_level(const int& level) {
    if (this->head == nullptr) return;
    auto *h = this->head;
    while (h->level == level) {
        if (h == this->tail) {
            delete h;
            this->head = nullptr;
            this->tail = nullptr;
            return;
        }
        this->head = this->head->next;
        delete h;
        h = this->head;
        this->head->prev = nullptr;
        this->curent_level = this->head;
    }
    while (this->curent_level->next) {
        if (this->curent_level->next->level != this->curent_level->level) break;
        this->curent_level = this->curent_level->next;
    }
}

string Tree::assign_tree(LinkedLisst& lisst, const string& ID, const string& value, int& level) {
    if (this->root == nullptr) {                            // ASSIGN (ID) (value)
        return "undeclared";
    }
    int num_com = 0, num_splay = 0;
    regex function(R"(([a-z]\w*)(\((('[a-zA-Z\d ]*'|\d+|[a-z]\w*)(,('[a-zA-Z\d ]*'|\d+|[a-z]\w*))*)*\))$)");
    smatch data;
    if (regex_match(value, data, function)) {           // for function
        string out = this->look_up(data[1], level);
        if (out == "no") {
            return "undeclared";
        }
        num_com+=this->root->data.num_com;
        num_splay+=this->root->data.num_splay;
        string func = this->root->data.type;
        if (func == "number" || func == "string") {
            return "mismatch";
        }
        LinkedLisst Lisst;
        Lisst.head = nullptr;
        for (int i = 0; i < (int) func.length(); i++) {     // token (type),(type),(type)
            if (func[i] != ',' && func[i] != '(' && func[i] != '-' && func[i] != '>' && func[i] != ')') {
                int j;
                for (j = i + 1; j < (int) func.length(); j++) {
                    if (func[j] == ',' || func[j] == ')') {
                        break;
                    }
                }
                Lisst.insert_lisst(func.substr(i, j - i), 1);
                i = i + (j - i);
            }
        }                                                   // lisst tail is the result if right check for all variable(s)
        string var = data[2];
        LinkedLisst Lisst1;                                 // lisst of value(s)
        for (int i = 0; i < (int) var.length(); i++) {
            if (var[i] != ',' && var[i] != '(' && var[i] != ')') {
                int j;
                for (j = i + 1; j < (int) var.length(); j++) {
                    if (var[j] == ',' || var[j] == ')') {
                        break;
                    }
                }
                Lisst1.insert_lisst(var.substr(i, j - i), 1);
                i = i + (j - i);
            }
        }
        if (Lisst.size != Lisst1.size + 1) {                // if number of variables is not the same
            return "mismatch";
        }
        if (Lisst.size != 1) {
            auto *h = Lisst.head, *h1 = Lisst1.head;
            for (int i = 0; i < Lisst1.size; i++) {
                if (h1 == nullptr) break;
                if (type_of_value(h1->ID) == "ID") {
                    string out1 = this->look_up(h1->ID, level);
                    if (out1 == "no") {
                        return "undeclared";
                    } else if (this->root->data.type != h->ID) {
                        return "mismatch";
                    }
                    num_com+=this->root->data.num_com;
                    num_splay+=this->root->data.num_splay;
                } else if (type_of_value(h1->ID) != h->ID) {
                    return "mismatch";
                }
                h = h->next;
                h1 = h1->next;
            }
        }
        string out1 = this->look_up(ID, level);              // node assigned
        if (out1 == "no") {                              // if invalid node
            return "undeclared";
        }
        num_com+=this->root->data.num_com;
        num_splay+=this->root->data.num_splay;
        if (this->root->data.type != Lisst.tail->ID) {          // if type not same
            return "mismatch";
        }
    } else if (type_of_value(value) == "ID") {                 // value is another ID
        string out = this->look_up(value, level);      //lookup ID_value node
        if (out == "no") {
            return "undeclared";
        }
        if (this->root->data.type != "string" && this->root->data.type != "number") {
            return "mismatch";
        }
        num_com+=this->root->data.num_com;
        num_splay+=this->root->data.num_splay;
        string type_check = this->root->data.type;      // type of ID_value
        string out1 = this->look_up(ID, level);       // lookup ID node
        if (out1 == "no") {
            return "undeclared";
        }
        num_com+=this->root->data.num_com;
        num_splay+=this->root->data.num_splay;
        if (type_check != this->root->data.type) {
            return "mismatch";
        }

    } else {
        string out1 = this->look_up(ID, level);
        if (out1 == "no") {
            return "undeclared";
        }
        num_com+=this->root->data.num_com;
        num_splay+=this->root->data.num_splay;
        if (type_of_value(value) != this->root->data.type) {
            return "mismatch";
        }
    }
    string output1 = to_string(num_com) + " " + to_string(num_splay);
    return output1;
}

void Tree::delete_tree(identifier_node* &node) {
    if (node == nullptr) return;

    delete_tree(node->left_child);
    delete_tree(node->right_child);

    delete node;
}

identifier_node* Tree::find_max(identifier_node* &node) {
    while (node->right_child != nullptr) node = node->right_child;
    return node;
}

void Tree::end_level(LinkedLisst &lisst, int level) {
    if (this->root == nullptr) return;

    auto *t = lisst.head;
    while (t && t->level == level) {
        this->look_up(t->ID, level);
        if (this->root->right_child == nullptr && this->root->left_child == nullptr) {
            delete this->root;
            this->root = nullptr;
        } else if (this->root->right_child == nullptr) {
            this->root = this->root->left_child;
            delete this->root->parent;
            this->root->parent = nullptr;
        } else if (this->root->left_child == nullptr) {
            this->root = this->root->right_child;
            delete this->root->parent;
            this->root->parent = nullptr;
        } else {
            auto *left = this->root->left_child;
            auto *right = this->root->right_child;
            delete this->root;
            this->root = left;
            left->parent = nullptr;
            auto *max = this->find_max(left);
            this->splay(max);
            this->root->right_child = right;
            right->parent = this->root;
        }
        t = t->next;
    }
}

string Tree::look_up(const string& ID, int level) {
    if (this->root == nullptr) {
        return "no";
    }
    for (int i = level; i >= 0; i--) {
        int count = 0;
        for (auto *h = this->root;;) {
            count++;
            if (i > h->data.level) {                //for level higher root
                if (h->right_child == nullptr) {
                    break;
                }
                h = h->right_child;

            } else if (i < h->data.level) {         //for level lower
                if (h->left_child == nullptr) {
                    break;
                }
                h = h->left_child;

            } else {                                //compare ID
                if (ID == h->data.ID) {
                    h->data.num_com = count;
                    h->data.num_splay = this->splay(h);
                    return "find";

                } else if (ID.compare(h->data.ID) < 0) {
                    if (h->left_child == nullptr) {
                        break;
                    }
                    h = h->left_child;

                } else if (ID.compare(h->data.ID) > 0) {
                    if (h->right_child == nullptr) {
                        break;
                    }
                    h = h->right_child;
                }
            }
        }
    }
    return "no";
}

void Tree::print(identifier_node* &node) {
    if (node == nullptr)    return;

    if (node == this->root) cout << node->data.ID << "//" << node->data.level;
    else                    cout << " " << node->data.ID << "//" << node->data.level;

    print(node->left_child);
    print(node->right_child);
}

LinkedLisst::~LinkedLisst() {
    while (this->head) {
        auto *h = this->head;
        this->head = this->head->next;
        delete h;
    }
}

Tree::~Tree() {
    delete_tree(this->root);
    this->root = nullptr;
}