#include "SymbolTable.h"


//=========== Symbol Table =============
SymbolTable::SymbolTable()
{
    this->tree = new SplayTree;
    this->currentLevel = 0;
}

SymbolTable::~SymbolTable()//HANDLE ENDFILE
{
    delete this->tree;
}
void SymbolTable::run(const string& filename)
{
    string line;
    ifstream infile(filename);
    if(infile.is_open()){
        while(getline(infile, line)){
            //Doing the main function
            pair<string,int>command = process(line);
            if(command.first == "INSERT"){
                if(this->tree->root == nullptr){
                    this->insert(line);
                }
                else{
                    this->handle_exception_insert(line, this->tree->root);
                    this->insert(line);
                }
            }
            else if (command.first == "ASSIGN");
            else if (command.first == "BEGIN");
            else if (command.first == "END"){
                this->end_scope(this->tree->root);
                this->currentLevel--;
            }
            else if (command.first == "LOOKUP");
            else if (command.first == "PRINT");
        }
        handle_end_file();
    }
    //HANDLE END OF FILE
    infile.close();
    //cout << "success";
    //CLEAN GARBAGE #goes here
}

pair<string, int> SymbolTable::process(string line){ //CHECK LỖI NGỮ NGHĨA
    //Check all REGEX MATCH
    static const regex valid_insert("^INSERT[ ]([a-z][a-zA-Z0-9_]*)[ ](string|number|\\((?:|(?:number|string)(?:,(?:number|string))*)\\)->(?:number|string))[ ](true|false)$)");
    static const regex valid_assign("^ASSIGN[ ][a-z][a-zA-Z0-9_]*[ ](?:[a-z][a-zA-Z0-9_]*|\\d+|'[\\dA-Za-z\\s]+')$");
    // static const regex valid_begin("^BEGIN$");
    // static const regex valid_end("^END$");
    // static const regex valid_print("^PRINT$");
    static const regex valid_lookup("^LOOKUP[ ][a-z][a-zA-Z0-9_]*$");
    //static const regex valid_rprint("^RPRINT$");
    pair<string, int> command;
    //IF INSERT
    if(regex_match(line, valid_insert) == 1){ 
        handle_exception_insert(line, this->tree->root);
        command.first = "INSERT"; command.second = 1;
        return command;
    }
     
    //IF ASSIGN
    else if (regex_match(line, valid_assign)){ 
        int type = handle_exception_assign(line);
        command.first = "ASSIGN"; command.second = type;
        return command;
    } 

    //IF LOOKUP
    else if (regex_match(line, valid_lookup)){
        command.first = "LOOKUP"; command.second = 1;
        return command;
    }
    //From this - USE string compare to check
    //IF BEGIN
    else if (line == "BEGIN"){
        command.first = "BEGIN"; command.second = 1;
        return command;
    }
    //IF END
    else if(line == "END"){
        command.first = "END"; command.second = 1;
        return command;
    }

    //IF PRINT
    else if(line == "PRINT"){
        command.first = "PRINT"; command.second = 1;
        return command;
    }
    else throw InvalidInstruction(line);
}

void SymbolTable::insert(string line){
    int comp = 0, splay = 0;
    auto start = line.find(' ');
    string name = line.substr(start + 1, line.find(" ", start + 1) - start - 1); //Incomplete
    string type = line.substr(line.find(' ', start + 1) + 1); //Incomplete
    Identifier a = Identifier(name, type, "");
    Node* add = new Node(a);

    this->tree->insert_node(this->tree->root, add, comp);
    this->tree->splay(add, splay);
    cout << comp << " " << splay;
}
void SymbolTable::assign(string line, int type){
   
    cout <<"success";
    
}
void SymbolTable::lookup(string line) const
{
    int start = line.find(" ");
    string name = line.substr(start + 1, line.find(" ", start + 1) - start - 1);
    Node*tmp = this->tree->find(name, this->tree->root);
    if(tmp == nullptr) throw Undeclared(line); //NOT FOUND
    else
        cout << tmp->level; //FOUND
}
void SymbolTable::new_scope()
{
    this->currentLevel += 1;
}
void SymbolTable::end_scope(Node* start)
{
    if(start == nullptr)
        return;
    end_scope(start->left);
    end_scope(start->right);
    if(start->level == this->currentLevel)
        this->tree->deleteNode(start);
}
void SymbolTable::print(Node* root)
{
    if(root)
    {
        cout << root->data.name <<"//"<<root->level;
        if(root->left){
            cout <<" ";
            print(root->left);
        }
        if(root->right){
            cout << " ";
            print(root->right);
        }
    }
}

int SymbolTable::handle_exception_assign(string line) //CHECK LỖI LOGIC TRƯỚC KHI THỰC HIỆN ASSIGN
{
    static const regex valid_identifer("^[a-z][a-zA-Z0-9_]*$");
    static const regex valid_number("^\\d+$");
    static const regex valid_string("^'[\\dA-Za-z\\s]+'$");
    static const regex valid_func ("");
    return -1;
}
void SymbolTable::handle_exception_insert(string line, Node* root) //CHECK LỖI LOGIC TRƯỚC KHI THỰC HIỆN INSERT
{
    if(root)
    {
        static const regex valid_func(""); //Incomplete

        auto start = line.find(' ');
        string name = line.substr(start + 1, line.find(' ', start + 1) - start - 1); //Incomplete
        string type = line.substr(line.find(' ', start + 1) + 1); //Incomplete
        string isStatic = "true"; //Incomplete

        int check_level = (isStatic == "true")?0:this->currentLevel;

        //Check INVALID INSTRUCTION (FUNCTION IN LEVEL != 0)
        if(regex_match(type, valid_func)){ //If INSERT a function
            if(check_level != 0)
                throw InvalidDeclaration(line);
        }

        //Check REDECLARATION
        if(check_level == root->level) //If the same level then compare the name
        {
            if(name.compare(root->data.name) < 0) //Find in the left
                handle_exception_insert(line, root->left);
            else if(name.compare(root->data.name) > 0)
                handle_exception_insert(line, root->right);
            else //FOUND Identifier in the same level
                throw Redeclared(line);
        }
        if(check_level > root->level)
            handle_exception_insert(line, root->right);
        if(check_level < root->level)
            handle_exception_insert(line, root->left);
    }
}
void SymbolTable::handle_end_file(){
    if(this->currentLevel != 0)
    {
        delete this->tree;
        throw UnclosedBlock(this->currentLevel);
    }
}

//==================== Splay Tree =====================================
SplayTree::SplayTree() {
    this->root = new Node;
}
SplayTree::~SplayTree() {
    destroy(this->root);
}
void SplayTree::destroy(Node *node) {
    if(node == nullptr)
        return;
    destroy(node->left);
    destroy(node->right);
    delete node;
}
Node* SplayTree::find(string name, Node* node)
{
    if(node == nullptr)
        return nullptr;
    auto check = node->data.name.compare(name);
    if(check == 0)  return node;
    if(check < 0)   find(name, node->right);
    if(check > 0)   find(name, node->left);
}

void SplayTree::rightRotate(Node* node)
{
    Node* tmpLeft = node->left; //x
    Node* tmpParent = node->parent; //w
    Node* tmpLeftRight = tmpLeft->right; //z    

    if(tmpParent->left == node)
        tmpParent->left = tmpLeft;
    else
        tmpParent->right = tmpLeft;
    
    tmpLeftRight->parent = node;
    node->left = tmpLeftRight;
    tmpLeft->parent = tmpParent;
    tmpLeft->right = node;
    node->parent = tmpLeft;
}
void SplayTree::leftRotate(Node* node)
{
    Node* tmpParent = node->parent; //w
    Node* tmpRight = node->right; // y
    Node* tmpRightLeft = tmpRight->left; //z

    if(tmpParent->left == node)
        tmpParent->left = tmpRight;
    else    tmpParent->right = tmpRight;
    
    tmpRightLeft->parent = node;
    node->right = tmpRightLeft;
    
    tmpRight->left = node;
    node->parent = tmpRight;

    tmpRight->parent = tmpParent;
    node->parent = tmpRight;

}

void SplayTree::leftZZ(Node* node)
{
    Node*tmp = node->left;
    leftRotate(tmp);
    rightRotate(node);
}

void SplayTree::rightZZ(Node* node)
{
    Node*tmp = node->right;
    rightRotate(tmp);
    leftRotate(node);
}

void SplayTree::leftRoll(Node* node)
{
    Node* tmp = node->right;
    leftRotate(node);
    leftRotate(tmp);
}

void SplayTree::rightRoll(Node* node)
{
    Node*tmp = node->left;
    rightRotate(node);
    rightRotate(tmp);
}

void SplayTree::splay(Node* node, int &splay)
{
    if(node->parent == nullptr)
        return;
    
    Node* tmpParent = node->parent;
    if(tmpParent->parent == nullptr)
    {
        if(tmpParent->left == node)  rightRotate(tmpParent);
        else    leftRotate(tmpParent);
        splay++;
    }
    else
    {
        Node*tmp = tmpParent->parent;
        if(tmp->left == tmpParent){
            if(tmpParent->left == node)
                rightRoll(tmp);
            else    leftZZ(tmp);
        }
        else{
            if(tmpParent->left == node) rightZZ(tmp);
            else    leftRoll(tmp);
        }
        splay++;
    }
}

void SplayTree::insert_node(Node* root, Node*add, int& comp)
{
    if(add->level < root->level)
    {
        comp++;
        if(root->left == nullptr){
            root->left = add;
            add->parent = root;
        }
        else    insert_node(root->left, add, comp);

    }
    if(add->level > root->level)
    {
        comp++;
        if(root->right == nullptr){
            root->right = add;
            add->parent = root;
        }
        else   insert_node(root->right, add, comp);
    }
    if(add->level == root->level){ //Then compare the identifier name
        comp++;
        auto check = add->data.name.compare(root->data.name);
        if(check < 0) // ADD to left
        {
            comp++;
            if(root->left == nullptr){
                root->left = add;
                add->parent = root;

            }
            else     insert_node(root->left, add, comp);
        }
        else if(check > 0)
        {
            comp++;
            if(root->right == nullptr){
                root->right = add;
                add->parent = root;

            }
            else     insert_node(root->right, add, comp);
        }
    }
}

void SplayTree::deleteNode(Node*&tobeDel)
{
    if(tobeDel == nullptr)
        return;
    if(tobeDel->left == nullptr && tobeDel->right == nullptr) //Delete a internal node (leaf node)
    {
        if(tobeDel == this->root) //If we delete a root
        {
            delete this->root;    //Delete it first
            this->root = nullptr; //Set it to null
        }
        else
        {
            Node* tmpParent = tobeDel->parent;  //If it was a leaf
            if(tobeDel == tmpParent->left)      //If it is a left child then its parent has no left child
                tmpParent->left = nullptr;
            else
                tmpParent->right = nullptr;     //Similarly, when it is a right child
            delete tobeDel;
        }
    }
    else //If it has either left or right subtree or both
    {
        if(tobeDel->left) //Existing left subtree
        {
            Node* tmpProc = TreeMax(tobeDel->left);
            tobeDel->data = tmpProc->data;
            deleteNode(tmpProc);
        }
        else //Just existing right subtree
        {
            Node* tmpSucc = TreeMin(tobeDel->right);
            tobeDel->data = tmpSucc->data;
            deleteNode(tmpSucc);
        }
    }

}

void SplayTree::inorder(Node *root) {
    return ; //Testing
}

Node *SplayTree::TreeMin(Node *node)
{
    Node*tmp = node;
    while(tmp->left)
        tmp = tmp->left;
    return tmp;
}

Node *SplayTree::TreeMax(Node *node) {
    Node*tmp = node;
    while(tmp->right)
        tmp = tmp->right;
    return tmp;
}

Node *SplayTree::TreeSucc(Node *node) {
    Node*tmp = node;
    if(node->right)
        return TreeMin(node->right);
    else
    {
        Node* tmpParent = tmp->parent;
        while(tmpParent && tmp == tmpParent->right)
        {
            tmp = tmpParent;
            tmpParent = tmpParent->parent;
        }
        return tmpParent;
    }
}

Node *SplayTree::TreeProc(Node *node) {
    Node*tmp = node;
    if(node->left)
        return TreeMax(node->left);
    else
    {
        Node* tmpParent = node->parent;
        while(tmpParent && tmp == tmpParent->left)
        {
            tmp = tmpParent;
            tmpParent = tmpParent->parent;
        }
        return tmpParent;
    }
}

//================ NODE =======================
Node::Node() {
    parent = nullptr;
    left = nullptr;
    right = nullptr;
    level = 0;
}
Node::Node(const Identifier& identifier) {
    parent = nullptr;
    left = nullptr;
    right = nullptr;
    level = 0;
    data = identifier;
}
Node::~Node(){
    parent = nullptr;
    left = nullptr;
    right = nullptr;
}
void Node::setLevel(int nodelevel) {
    this->level = nodelevel;
}
//================ IDENTIFIER ==================
Identifier::Identifier(string name, string type, string value) {
    this->name = std::move(name);
    this->type = std::move(type);
    this->value = std::move(value);
}
Identifier::Identifier() = default;
Identifier::~Identifier() = default;