#include "SymbolTable.h"


//========== Utils ====================
void tokenizeIns(const string& line, string &name, string &type, string&isStatic, string&returnType)
{
    auto firstSpace = line.find(' ');
    auto secondSpace = line.find(' ',firstSpace+1);
    auto thirdSpace = line.find(' ', secondSpace+1);
    auto arrowIdx = line.find("->");
    name = line.substr(firstSpace+1, secondSpace-firstSpace - 1);
    type = line.substr(secondSpace+1, thirdSpace-secondSpace-1);
    isStatic = line.substr(thirdSpace+1);
    if(arrowIdx < line.size())
        returnType = line.substr(arrowIdx, thirdSpace-arrowIdx-2);
    
}
void tokenizeAss(const string& line, string& identifier, string& value)
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
SplayTree::SplayTree() {this->root = nullptr; }
SplayTree::~SplayTree() {destroy(this->root);}
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
SymbolTable::~SymbolTable(){delete this->tree;} //HANDLE END FILE
void SymbolTable::run(string filename)
{
    string line;
    ifstream infile(filename, ios::in);
    if(infile.is_open())
    {
        while(getline(infile, line)){
            //Doing the main function
            pair<string,int>command = process(line);
            if(command.first == "INSERT"){
                    this->handle_exception_insert(line, this->tree->root);
                    this->insert(line);
                    
            }
            //else if (command.first == "ASSIGN");
            else if (command.first == "BEGIN")  this->new_scope();
            else if (command.first == "END"){
                this->end_scope(this->tree->root);
                this->currentLevel--;
            }
            else if (command.first == "LOOKUP"){
                 this->lookup(line, this->tree->root);
             }
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
}

pair<string, int> SymbolTable::process(const string& line){ //CHECK LỖI NGỮ NGHĨA
    //Check all REGEX MATCH
    static const regex valid_insert("^INSERT[ ]([a-z][a-zA-Z0-9_]*)[ ](?:string|number)[ ](?:true|false)");
    static const regex valid_assign("^ASSIGN[ ][a-z][a-zA-Z0-9_]*[ ](?:[a-z][a-zA-Z0-9_]*|\\d+|'[\\dA-Za-z\\s]+')$");
    static const regex valid_lookup("^LOOKUP[ ][a-z][a-zA-Z0-9_]*$");
    pair<string, int> command;
    //IF INSERT
    if(regex_match(line, valid_insert) == 1){ 
        handle_exception_insert(line, this->tree->root);
        command.first = "INSERT"; command.second = 1;
        return command;
    }
     
    //IF ASSIGN
    // else if (regex_match(line, valid_assign)){ 
    //     int type = handle_exception_assign(line);
    //     command.first = "ASSIGN"; command.second = type;
    //     return command;
    // } 

    //IF LOOKUP
    else if (regex_match(line, valid_lookup)){
        command.first = "LOOKUP"; command.second = 1;
        return command;
    }
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

void SymbolTable::insert(const string& line) const{
    int comp = 0, splay = 0, initLevel = 0;
    string name="", type="", isStatic="", returnType="";k
    tokenizeIns(line, name, type, isStatic, returnType);

    if(isStatic == "false") 
        initLevel = this->currentLevel;
    if(type != "string" && type != "number"){
        type = "func";
    }
    
    Identifier a = Identifier(name, type, "", returnType);
    a.setLevel(initLevel);
    Node* add = new Node(a);

    this->tree->insert_node(this->tree->root, add, comp);
    while(add->getParent() != nullptr)
        this->tree->splay(add, splay);
    this->tree->root = add;
    cout << comp << " " << splay << endl;
    comp = 0; splay = 0;
}
// void SymbolTable::assign(string line, int type){
   
//     cout <<"success";
//INCLUDING LOOKUP, ASSIGN, BEGIN, END  
//}
void SymbolTable::lookup(const string&  line, Node* root) 
{
    int destLevel = this->currentLevel;
    string name="", type="", isStatic="", returnType="";
    tokenizeIns(line, name, type, isStatic, returnType);
    Node*tmp;
    while(destLevel != -1){
        tmp = this->tree->find(name, this->tree->root, destLevel);
        if(tmp == nullptr) destLevel--;
        else break;
    }
    if(destLevel == -1) throw Undeclared(line);
    else cout <<tmp->getData().getLevel() << endl;
}
void SymbolTable::new_scope(){this->currentLevel += 1;}
void SymbolTable::end_scope(Node* start)
{
    auto curLev = this->currentLevel;

    if(curLev == 0) throw UnknownBlock();
    if(start == nullptr)
        return;
    end_scope(start->getLeft());
    end_scope(start->getRight());
    if(start->getData().getLevel() == curLev && start->getData().getLevel() != 0){
        this->tree->deleteNode(start);
        return;
    }
}
void SymbolTable::print(Node* node, bool& flag) const
{
    if(node)
    {
        flag = true;

        cout << node->getData().getName() <<"//"<<node->getData().getLevel()    ;
        if(node->getLeft()){
            cout <<" ";
            print(node->getLeft(), flag);
        }
        if(node->getRight()){
            cout << " ";
            print(node->getRight(), flag);
        }
    }
    else return;

}

// int SymbolTable::handle_exception_assign(string line) //CHECK LỖI LOGIC TRƯỚC KHI THỰC HIỆN ASSIGN
// {
//     static const regex valid_identifer("^[a-z][a-zA-Z0-9_]*$");
//     static const regex valid_number("^\\d+$");
//     static const regex valid_string("^'[\\dA-Za-z\\s]+'$");
//     //static const regex valid_func ("");
//     return -1;
// }
void SymbolTable::handle_exception_insert(const string& line, Node* root) //CHECK LỖI LOGIC TRƯỚC KHI THỰC HIỆN INSERT
{
    if(root)
    {
        //static const regex valid_func(""); //Incomplete
        auto start = line.find(' ');
        string name, type, isStatic, returnType;
        tokenizeIns(line, name, type, isStatic, returnType);
        int check_level = (isStatic == "true")?0:this->currentLevel; //Init level first

        //Check INVALID INSTRUCTION (FUNCTION IN LEVEL != 0)
        //if(regex_match(type, valid_func)){ //If INSERT a function
         //   if(check_level != 0)
          //      throw InvalidDeclaration(line);
        //}
        //Check REDECLARATION
        if(check_level == root->getData().getLevel()) //If the same level then compare the name
        {
            if(name.compare(root->getData().getName()) < 0) //Find in the left
                handle_exception_insert(line, root->getLeft());
            else if(name.compare(root->getData().getName()) > 0)
                handle_exception_insert(line, root->getRight());
            else //FOUND Identifier in the same level
                throw Redeclared(line);
        }
        if(check_level > root->getData().getLevel())
            handle_exception_insert(line, root->getRight());
        if(check_level < root->getData().getLevel())
            handle_exception_insert(line, root->getLeft());
    }
}
void SymbolTable::handle_end_file() const
{
    if(this->currentLevel != 0){
        //delete this->tree;
        throw UnclosedBlock(this->currentLevel);
    }
}

//==================== Splay Tree =====================================

void SplayTree::destroy(Node *node) {
    if(node == nullptr)
        return;
    destroy(node->getLeft());
    destroy(node->getRight());
    delete node;
    return;
}
Node* SplayTree::find(const string& name, Node* node, int destLevel)
{
    if(node){
        if(node->getData().getName() == name && node->getData().getLevel() == destLevel)
        {
            return node;
        }
        if(node->getData().getLevel() < destLevel)
            find(name, node->getRight(), destLevel);
        else if(node->getData().getLevel() > destLevel)
            find(name, node->getLeft(), destLevel);
        else{
            if(node->getData().getName().compare(name) < 0)
                find(name, node->getRight(), destLevel);
            else if(node->getData().getName().compare(name) > 0)
                find(name, node->getLeft(), destLevel);
        }
    }
}
void SplayTree::rightRotate(Node* node)
{
    Node* tmpLeft = node->getLeft(); //x
    Node* tmpParent = node->getParent(); //w
    Node* tmpLeftRight = tmpLeft->getRight(); //z    

    if(tmpParent)
    {
        if(tmpParent->getLeft() == node)    tmpParent->setLeft(tmpLeft);
        else    tmpParent->setRight(tmpLeft);
    }
    if(tmpLeftRight)    tmpLeftRight->setParent(node);
        
    node->setLeft(tmpLeftRight);
    tmpLeft->setRight(node);
    node->setParent(tmpLeft);
    tmpLeft->setParent(tmpParent);
}
void SplayTree::leftRotate(Node* node)
{
    Node* tmpParent = node->getParent(); //w
    Node* tmpRight = node->getRight(); // y
    Node* tmpRightLeft = tmpRight->getLeft(); //z

    if(tmpParent)
    {
        if(tmpParent->getLeft() == node)    tmpParent->setLeft(tmpRight);
        else  tmpParent->setRight(tmpRight);
    }
    if(tmpRightLeft)    tmpRightLeft->setParent(node);

    node->setRight(tmpRightLeft);
    tmpRight->setLeft(node);
    tmpRight->setParent(tmpParent);
    node->setParent(tmpRight);
}
void SplayTree::leftZZ(Node* node)
{  
    Node*tmp = node->getLeft();
    leftRotate(tmp);
    rightRotate(node);   
}
void SplayTree::rightZZ(Node* node)
{
    Node*tmp = node->getRight();
    rightRotate(tmp);
    leftRotate(node);
}
void SplayTree::leftRoll(Node* node)
{
    Node* tmp = node->getRight();
    leftRotate(node);
    leftRotate(tmp);
}
void SplayTree::rightRoll(Node* node)
{
    Node*tmp = node->getLeft();
    rightRotate(node);
    rightRotate(tmp);
}
void SplayTree::splay(Node* node, int &splay)
{
    if(node->getParent() == nullptr)
        return;
    else
    {
        Node* tmpParent = node->getParent(); //y
        
        if(tmpParent->getParent() == nullptr)
        {   
            if(tmpParent->getLeft() == node && tmpParent->getLeft() != nullptr)  
                rightRotate(tmpParent);
            else    
                leftRotate(tmpParent);
            splay++;
        }
        else
        {
            Node*tmp = tmpParent->getParent(); //z
            if(tmp->getLeft() == tmpParent && tmp->getLeft() != nullptr){
                if(tmpParent->getLeft() == node && tmpParent->getLeft() != nullptr)
                    rightRoll(tmp);
                else   
                    leftZZ(tmp);
            }
            else{
                if(tmpParent->getRight() == node && tmpParent->getRight() != nullptr)
                    leftRoll(tmp);
                else
                    rightZZ(tmp);
                    
            }
            splay++;
        }
    }
}
void SplayTree::insert_node(Node* root, Node*add, int& comp)
{
    if(this->root == nullptr)
    {
        this->root = add;
        return;
    }
    if(add->getData().getLevel() < root->getData().getLevel())
    {
        comp++;
        if(root->getLeft() == nullptr){
            this->addLeft(root, add);
            return;
        }
        else    insert_node(root->getLeft(), add, comp);

    }
    else if(add->getData().getLevel() > root->getData().getLevel())
    {
        comp++;
        if(root->getRight() == nullptr){
            this->addRight(root, add);
            return;
        }
        else   insert_node(root->getRight(), add, comp);
    }
    else if(add->getData().getLevel() == root->getData().getLevel())//Then compare the identifier name
    { 
        
        auto check = add->getData().getName().compare(root->getData().getName());
        if(check < 0) // ADD to left
        {
            comp++;
            if(root->getLeft() == nullptr){
                this->addLeft(root, add);
                return;
            }
            else     insert_node(root->getLeft(), add, comp);
        }
        else if(check > 0)
        {
            comp++;
            if(root->getRight() == nullptr){
                this->addRight(root, add);
                return;
            }
            else     insert_node(root->getRight(), add, comp);
        }
    }
}
void SplayTree::deleteNode(Node*tobeDel)
{
    if(tobeDel == nullptr) //Base case
        return;
    if(tobeDel->getLeft() == nullptr && tobeDel->getRight() == nullptr) //Delete a internal node (leaf node)
    {
        if(tobeDel == this->root) //If we delete a root
        {
            delete this->root;    //Delete it first
            this->root = nullptr; //Set it to null
            
        }
        else
        {
            Node* tmpParent = tobeDel->getParent();  //If it was a leaf
            if(tobeDel == tmpParent->getLeft())      //If it is a left child then its parent has no left child
                tmpParent->setLeft(nullptr);
            else    tmpParent->setRight(nullptr);       //Similarly, when it is a right child
            delete tobeDel;
            tobeDel = nullptr;
            
        }
    }
    else //If it has either left or right subtree or both
    {
        if(tobeDel->getRight() != nullptr) //Just existing right subtree
        {
            Node* tmpSucc = TreeMin(tobeDel->getRight());
            tobeDel->setData(tmpSucc->getData());
            deleteNode(tmpSucc);
        }
        else //Existing left subtree
        {
            Node *tmpProc = TreeMax(tobeDel->getLeft());
            tobeDel->setData(tmpProc->getData());
            deleteNode(tmpProc);
        }
    }
}
Node *SplayTree::TreeMin(Node *node)
{
    Node*tmp = node;
    while(tmp->getLeft())
        tmp = tmp->getLeft();
    return tmp;
}

Node *SplayTree::TreeMax(Node *node) {
    Node*tmp = node;
    while(tmp->getRight())
        tmp = tmp->getRight();
    return tmp;
}

Node *SplayTree::TreeSucc(Node *node) {
    Node*tmp = node;
    if(node->getRight())
        return TreeMin(node->getRight());
    else
    {
        Node* tmpParent = tmp->getParent();
        while(tmpParent && tmp == tmpParent->getRight())
        {
            tmp = tmpParent;
            tmpParent = tmpParent->getParent();
        }
        return tmpParent;
    }
}

Node *SplayTree::TreeProc(Node *node) {
    Node*tmp = node;
    if(node->getLeft())
        return TreeMax(node->getLeft());
    else
    {
        Node* tmpParent = node->getParent();
        while(tmpParent && tmp == tmpParent->getLeft())
        {
            tmp = tmpParent;
            tmpParent = tmpParent->getParent();
        }
        return tmpParent;
    }
}

void SplayTree::addLeft(Node*& node, Node*& add){
    node->setLeft(add);
    add->setParent(node);
}

void SplayTree::addRight(Node*& node, Node*& add){
    node->setRight(add);
    add->setParent(node);
}

void SplayTree::inorder(Node *root) {
    if(!root) return;
    if(root->getLeft())
        inorder(root->getLeft());
    cout << root->getData().getName()<<"//"<<root->getData().getLevel() <<" ";
    if(root->getRight())
        inorder(root->getRight());
}

int SplayTree::countNode(Node* node){
    if(node == nullptr) return 0;
    return 1+countNode(node->getLeft()) + countNode(node->getRight());
}

//================ List =========================
DLL::DLL():head(nullptr),tail(nullptr){}
DLL::~DLL(){this->destroy();}
DLL::ListNode::ListNode(string data):next(nullptr),prev(nullptr), data(data){}
DLL::ListNode::~ListNode(){}
bool DLL::isEmpty(){
    if(this->head ==nullptr)    return true;
    else return false;
}
void DLL::append(string str){
    ListNode*add = new ListNode(str);
    if(this->head == nullptr){
        this->head = add;
        this->tail = add;
    }
    else{
        this->tail->next = add;
        add->prev = this->tail;
        this->tail = this->tail->next;
    }
}
void DLL::addfront(string str){
    ListNode*add = new ListNode(str);
    if(this->head == nullptr){
        this->head=  add;
        this->tail = add;
    }
    else{
        this->head->prev = add;
        add->next = this->head;
        this->head = this->head->prev;
    }
}
string DLL::pop_front(){
    if(!this->isEmpty()){
        ListNode* tmp = this->head;
        this->head = this->head->next;
        string ans = tmp->data;
        delete tmp;
        return ans;
    }
}



//================ NODE =======================
Node* Node::getLeft() const {return this->left;}
Node* Node::getRight() const {return this->right;}
Node* Node::getParent() const {return this->parent;}
void Node::setParent(Node *parent_node)  {this->parent = parent_node;}
void Node::setRight(Node *right_node) {this->right = right_node;}
void Node::setLeft(Node *left_node)  {this->left = left_node;}
Identifier Node::getData() const {return this->data;}
//================ IDENTIFIER ==================
Identifier::Identifier(string name, string type, string value, string returnType) {
    this->name = std::move(name);   this->type = std::move(type);
    this->value = std::move(value); this->returnType = std::move(returnType);
    this->level = 0;
}
void Identifier::setLevel(int level_scope){this->level = level_scope;}
Identifier::Identifier(){this->level = 0;}
Identifier::~Identifier() = default;