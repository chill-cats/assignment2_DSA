#include "SymbolTable.h"


//========== Utils ====================
void tokenizeIns(string line, string &name, string &type, string&isStatic)
{
    auto firstSpace = line.find(' ');
    auto secondSpace = line.find(' ',firstSpace+1);
    auto thirdSpace = line.find(' ', secondSpace+1);
    
    name = line.substr(firstSpace+1, secondSpace-firstSpace - 1);
    type = line.substr(secondSpace+1, thirdSpace-secondSpace-1);
    isStatic = line.substr(thirdSpace+1);
}
void tokenizeAss(string line, string& identifier, string& value)
{
    auto firstSpace = line.find(' ');
    auto secondSpace = line.find(' ',firstSpace+1);
    
    identifier = line.substr(firstSpace+1, secondSpace-firstSpace - 1);
    value = line.substr(secondSpace+1);
}
//======== Initialization =================
SymbolTable::SymbolTable()
{
    this->tree = new SplayTree;
    this->currentLevel = 0;
}
SplayTree::SplayTree() {
    this->root = nullptr;
}
SplayTree::~SplayTree() {
    destroy(this->root);
}
Node::Node() {
    parent = nullptr;
    left = nullptr;
    right = nullptr;
    
}
Node::Node(const Identifier& identifier) {
    parent = nullptr;
    left = nullptr;
    right = nullptr;
    data = identifier;
}
Node::~Node(){
    parent = nullptr;
    left = nullptr;
    right = nullptr;
    
}
//=========== Symbol Table =============
SymbolTable::~SymbolTable()//HANDLE ENDFILE
{
    delete this->tree;
}
void SymbolTable::run(string filename)
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
            
            //else if (command.first == "ASSIGN");
            else if (command.first == "BEGIN")
                this->new_scope();
            else if (command.first == "END"){
                this->end_scope(this->tree->root);
                this->currentLevel--;
            }
            // else if (command.first == "LOOKUP")
            //     this->lookup(line);
            else if (command.first == "PRINT"){
                bool flag = false;
                this->print(this->tree->root, flag);
                if(flag)    cout << endl;

            }
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
    static const regex valid_insert("^INSERT[ ]([a-z][a-zA-Z0-9_]*)[ ](?:string|number)");
    static const regex valid_assign("^ASSIGN[ ][a-z][a-zA-Z0-9_]*[ ](?:[a-z][a-zA-Z0-9_]*|\\d+|'[\\dA-Za-z\\s]+')$");
    // static const regex valid_begin("^BEGIN$");
    // static const regex valid_end("^END$");
    // static const regex valid_print("^PRINT$");
    static const regex valid_lookup("^LOOKUP[ ][a-z][a-zA-Z0-9_]*$");
    //static const regex valid_rprint("^RPRINT$");
    pair<string, int> command;
    //IF INSERT
    if(regex_match(line, valid_insert) == 1){ 
        //handle_exception_insert(line, this->tree->root);
        command.first = "INSERT"; command.second = 1;
        return command;
    }
     
    //IF ASSIGN
    // else if (regex_match(line, valid_assign)){ 
    //     int type = handle_exception_assign(line);
    //     command.first = "ASSIGN"; command.second = type;
    //     return command;
    // } 

    // //IF LOOKUP
    // else if (regex_match(line, valid_lookup)){
    //     command.first = "LOOKUP"; command.second = 1;
    //     return command;
    // }
    // //From this - USE string compare to check
    // //IF BEGIN
    else if (line == "BEGIN"){
        command.first = "BEGIN"; command.second = 1;
        return command;
    }
    // //IF END
    else if(line == "END"){
        command.first = "END"; command.second = 1;
        return command;
    }

    // //IF PRINT
    else if(line == "PRINT"){
        command.first = "PRINT"; command.second = 1;
        return command;
    }
    else throw InvalidInstruction(line);
}

void SymbolTable::insert(string line){
    int comp = 0, splay = 0;
    string name, type, isStatic;
    tokenizeIns(line, name, type, isStatic);
    if(type != "string" && type != "number")
        type = "func";
    Identifier a = Identifier(name, type, "");
    a.setLevel(this->currentLevel);
    Node* add = new Node(a);
    this->tree->insert_node(this->tree->root, add, comp);
    this->tree->splay(add, splay);
    cout << comp << " " << splay << endl;
    comp = 0; splay = 0;
}
// void SymbolTable::assign(string line, int type){
   
//     cout <<"success";
//INCLUDING LOOKUP, ASSIGN, BEGIN, END  
//}
// void SymbolTable::lookup(string line) const
// {
//     string name, type, isStatic;
//     tokenizeIns(line, name, type, isStatic);
//     Node*tmp = this->tree->find(name, this->tree->root);
//     if(tmp == nullptr) throw Undeclared(line); //NOT FOUND
//     else
//         cout << tmp->level; //FOUND
// }
void SymbolTable::new_scope()
{
    this->currentLevel += 1;
}
void SymbolTable::end_scope(Node* start)
{
    if(this->currentLevel == 0) throw UnknownBlock();
    if(start == nullptr)
        return;
    end_scope(start->left);
    end_scope(start->right);
    if(start->data.level == this->currentLevel){
        this->tree->deleteNode(start);
        return;
    }
}
void SymbolTable::print(Node* node, bool& flag)
{
    if(node)
    {   
        flag = true;
        cout << node->data.name <<"//"<<node->data.level;
        if(node->left){
            cout <<" ";
            print(node->left, flag);
        }
        if(node->right){
            cout << " ";
            print(node->right, flag);
        }
    }
}

// int SymbolTable::handle_exception_assign(string line) //CHECK LỖI LOGIC TRƯỚC KHI THỰC HIỆN ASSIGN
// {
//     static const regex valid_identifer("^[a-z][a-zA-Z0-9_]*$");
//     static const regex valid_number("^\\d+$");
//     static const regex valid_string("^'[\\dA-Za-z\\s]+'$");
//     //static const regex valid_func ("");
//     return -1;
// }
void SymbolTable::handle_exception_insert(string line, Node* root) //CHECK LỖI LOGIC TRƯỚC KHI THỰC HIỆN INSERT
{
    if(root)
    {
        //static const regex valid_func(""); //Incomplete

        auto start = line.find(' ');
        string name, type, isStatic;
        tokenizeIns(line, name, type, isStatic);

        int check_level = (isStatic == "true")?0:this->currentLevel;

        //Check INVALID INSTRUCTION (FUNCTION IN LEVEL != 0)
        //if(regex_match(type, valid_func)){ //If INSERT a function
         //   if(check_level != 0)
          //      throw InvalidDeclaration(line);
        //}

        //Check REDECLARATION
        if(check_level == root->data.level) //If the same level then compare the name
        {
            if(name.compare(root->data.name) < 0) //Find in the left
                handle_exception_insert(line, root->left);
            else if(name.compare(root->data.name) > 0)
                handle_exception_insert(line, root->right);
            else //FOUND Identifier in the same level
                throw Redeclared(line);
        }
        if(check_level > root->data.level)
            handle_exception_insert(line, root->right);
        if(check_level < root->data.level)
            handle_exception_insert(line, root->left);
    }
}
void SymbolTable::handle_end_file(){
    if(this->currentLevel != 0)
    {
        //delete this->tree;
        throw UnclosedBlock(this->currentLevel);
    }
}

//==================== Splay Tree =====================================

void SplayTree::destroy(Node *node) {
    if(node == nullptr)
        return;
    destroy(node->left);
    destroy(node->right);
    delete node;
    return;
}
Node* SplayTree::find(string name, Node* node)
{
    if(node == nullptr)
        return nullptr;
    auto check = node->data.name.compare(name);
    if(check < 0)   find(name, node->right);
    if(check > 0)   find(name, node->left);
    if(check == 0)  return node;

}

void SplayTree::rightRotate(Node* node)
{
    Node* tmpLeft = node->left; //x
    Node* tmpParent = node->parent; //w
    Node* tmpLeftRight = tmpLeft->right; //z    

    if(tmpParent)
    {
        if(tmpParent->left == node)
            tmpParent->left = tmpLeft;
        else
            tmpParent->right = tmpLeft;
    }
    if(tmpLeftRight){
        tmpLeftRight->parent = node;
        
    }
    node->left = tmpLeftRight;
    tmpLeft->right = node;
    node->parent = tmpLeft;
    tmpLeft->parent = tmpParent;

}
void SplayTree::leftRotate(Node* node)
{
    
    Node* tmpParent = node->parent; //w
    Node* tmpRight = node->right; // y
    Node* tmpRightLeft = tmpRight->left; //z

    if(tmpParent)
    {
        if(tmpParent->left == node)
            tmpParent->left = tmpRight;
        else    tmpParent->right = tmpRight;
    }
    if(tmpRightLeft)
        tmpRightLeft->parent = node;

    node->right = tmpRightLeft;
    tmpRight->left = node;  
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
    else
    {
        Node* tmpParent = node->parent; //y
        
        if(tmpParent->parent == nullptr)
        {   
            if(tmpParent->left == node && tmpParent->left != nullptr)  
                rightRotate(tmpParent);
            else    
                leftRotate(tmpParent);
            splay++;
        }
        else
        {
            Node*tmp = tmpParent->parent; //z
            if(tmp->left == tmpParent && tmp->left != nullptr){
                if(tmpParent->left == node && tmpParent->left != nullptr)
                    rightRoll(tmp);
                else   
                    leftZZ(tmp);
            }
            else{
                if(tmpParent->right == node && tmpParent->right != nullptr)
                    leftRoll(tmp);
                else
                    rightZZ(tmp);
                    
            }
            splay++;
        }
        this->root = node;
    }
}

void SplayTree::insert_node(Node* root, Node*add, int& comp)
{
    if(this->root == nullptr)
    {
        this->root = add;
        return;
    }
    if(add->data.level < root->data.level)
    {
        comp++;
        if(root->left == nullptr){
            root->left = add;
            add->parent = root;
            return;
        }
        else    insert_node(root->left, add, comp);

    }
    else if(add->data.level > root->data.level)
    {
        comp++;
        if(root->right == nullptr){
            root->right = add;
            add->parent = root;
            return;
        }
        else   insert_node(root->right, add, comp);
    }
    else if(add->data.level == root->data.level)//Then compare the identifier name
    { 
        
        auto check = add->data.name.compare(root->data.name);
        if(check < 0) // ADD to left
        {
            comp++;
            if(root->left == nullptr){
                root->left = add;
                add->parent = root;
                return;
            }
            else     insert_node(root->left, add, comp);
        }
        else if(check > 0)
        {
            comp++;
            if(root->right == nullptr){
                root->right = add;
                add->parent = root;
                return;
            }
            else     insert_node(root->right, add, comp);
        }
    }
}

void SplayTree::deleteNode(Node*tobeDel)
{
    if(tobeDel == nullptr) //Base case
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

//================ IDENTIFIER ==================
Identifier::Identifier(string name, string type, string value) {
    this->name = std::move(name);
    this->type = std::move(type);
    this->value = std::move(value);
    this->level = 0;
}
void Identifier::setLevel(int level){this->level = level;}
Identifier::Identifier(){this->level = 0;}
Identifier::~Identifier() = default;