#include "SymbolTable.h"
/*
Created by Quang Nguyen
Updated on Oct, 26, 2021
*/
//========== HangDoi ====================
LixtNode::LixtNode(){this->next = nullptr; this->level = 0;}
LixtNode::LixtNode(Node* data, int level){this->data = data; this->next = nullptr; this->level = level;}
LixtNode::~LixtNode(){}
HangDoi::HangDoi(){this->size = 0; this->front = this->rear = nullptr; this->levelTrack = nullptr;}
void HangDoi::append(Node* name, int level){
    LixtNode *add_node;
    if(this->front == nullptr){ //Add the first node
        add_node = new LixtNode(name, level);
        this->front = add_node;
        this->rear = add_node;
        this->levelTrack = this->front;
    }
    else{
        add_node = new LixtNode(name, level);
        if(this->front->level < level){
            add_node->next = this->front;
            this->front = add_node;
            levelTrack = this->front;
        }
        else if(this->front->level == level){ //If append in the same level then put it back
            if(this->rear->level == level){
                this->rear->next = add_node;
                this->rear = this->rear->next;   
            }
            else if (level > this->rear->level){
                add_node->next = this->levelTrack->next;
                this->levelTrack->next = add_node;
                this->levelTrack = this->levelTrack->next;
            }
        }
    }
    this->size++;
}
bool HangDoi::isEmpty(){
    if(this->front == nullptr) return true;
    return false;
}
Node* HangDoi::pop_front(){
    //If not empty yet.
    if(!this->front)    return nullptr;
    Node* tmp_data;
    if(this->front == levelTrack){
        if(levelTrack)  levelTrack = levelTrack->next;
    }
    LixtNode *tmp = this->front;
    this->front = this->front->next;
    if(!this->front){
        this->rear = nullptr; this->levelTrack = nullptr;
    }
    tmp_data = tmp->data;
    delete tmp;

    this->size--;
    return tmp_data;
}

HangDoi::~HangDoi(){
    while(this->front){
        LixtNode *tmp = this->front;
        this->front = this->front->next;
        delete tmp;
    }
    this->size = 0;
}
//========== Utils ====================
ValidMachine::ValidMachine(string str) {
    this->line = str;
    this->name=this->type=this->isStatic=this->value = "";
}
ValidMachine::~ValidMachine() = default;
bool ValidMachine::isInsertFunc()   {return regex_match(this->line, this->valid_insertFunc);}
bool ValidMachine::isInsertVar()    {return regex_match(this->line, this->valid_insertVar);}
bool ValidMachine::isLookup()       {return regex_match(this->line, this->valid_lookup);}
bool ValidMachine::isAssign()       {return regex_match(this->line, this->valid_assign);}
void ValidMachine::parseLookup() {
    auto firstSpace = this->line.find(' ');
    this->name = this->line.substr(firstSpace+1);
}
void ValidMachine::parseInsert(bool Func) {
    auto firstSpace = this->line.find(' ');
    auto secondSpace = this->line.find(' ',firstSpace+1);
    auto thirdSpace = this->line.find(' ', secondSpace+1);
    this->name = line.substr(firstSpace+1, secondSpace-firstSpace - 1);
    this->isStatic = line.substr(thirdSpace+1);
    if(Func){
        auto arrowIdx = line.find("->");
        this->param = line.substr(line.find('(')+1, arrowIdx-2-line.find('('));
        this->returnType  = line.substr(arrowIdx+2, thirdSpace-arrowIdx-2);
    }
    else    
        type = line.substr(secondSpace+1, thirdSpace-secondSpace-1);
}
void ValidMachine::parseAssign() {
    auto firstSpace = this->line.find(' ');
    auto secondSpace = this->line.find(' ',firstSpace+1);
    this->identifier = this->line.substr(firstSpace+1, secondSpace-firstSpace - 1);
    this->value = this->line.substr(secondSpace+1);
}
//======== Initialization =================
SymbolTable::SymbolTable()
{
    this->tree = new SplayTree; this->hangdoi = new HangDoi;
    this->currentLevel = 0;
}
SplayTree::SplayTree() {this->root = nullptr; this->count = 0;}
SplayTree::~SplayTree() {destroy(this->root); this->root = nullptr;}
Node::Node() {
    parent = nullptr;
    left = nullptr;
    right = nullptr;
    this->numParam = 0;
    this->paramList = nullptr;
}
Node::Node(Identifier& identifier) {
    parent = nullptr;
    left = nullptr;
    right = nullptr;
    data = identifier;
    this->numParam = 0;
    this->paramList = nullptr;
}
Node::~Node(){
    if(this->paramList) delete[] this->paramList;
    parent = nullptr;
    left = nullptr;
    right = nullptr;
}
//=========== Symbol Table =============
SymbolTable::~SymbolTable(){
    delete this->hangdoi;
    delete this->tree;
} //HANDLE END FILE
void SymbolTable::run(string filename)
{
    string line;
    ifstream infile(filename, ios::in);
    if(infile.is_open())
    {

        while(getline(infile, line)){
            //Doing the main function
            const string command = process(line);
            if(command == "INSERT"){
                this->insert(line);
                this->tree->count++;
                
            }
            else if (command == "ASSIGN"){
                int ass_comp = 0, ass_splay = 0;
                this->assign(line, ass_comp, ass_splay);
                cout << ass_comp << " " << ass_splay << endl;
            }
            else if (command == "BEGIN")
                this->new_scope();

            else if (command== "END"){this->end_scope();}

            else if (command == "LOOKUP"){
                
                if(!this->tree->root)   throw Undeclared(line);
                int comp = 0, splay= 0, currentLevel = this->currentLevel;
                ValidMachine tester = ValidMachine(line);
                tester.parseLookup();
                const string name = tester.name;
                Node*tmp = this->lookup(name,currentLevel,comp);
                if(tmp == nullptr) throw Undeclared(line);
                else{
                    cout << tmp->data.level<<"\n";
                    this->tree->splay(tmp, splay);
                }
            } 

            else if (command == "PRINT"){
                bool printed = false;
                this->print(this->tree->root, printed);
                if(printed == true) 
                    cout << "\n";
            }
        }
        handle_end_file();
    }
    //HANDLE END OF FILE
    infile.close();
}
const string SymbolTable::process(const string& line){ //CHECK LỖI NGỮ NGHĨA
    //Check all REGEX MATCH
    ValidMachine tester = ValidMachine(line);
    string command;
    //IF INSERT
    if(tester.isInsertFunc() || tester.isInsertVar()){
        handle_exception_insert(line);
        command = "INSERT"; 
        return command;
    }
     
    else if (tester.isAssign()){ 
        command = "ASSIGN";
        return command;
    } 

    else if (tester.isLookup()){
        command = "LOOKUP";
        return command;
    }

    else if (line == "BEGIN"){
        command = "BEGIN";
        return command;
    }
    
    else if(line == "END"){
        command = "END"; 
        return command;
    }

    else if(line == "PRINT"){
        command = "PRINT"; 
        return command;
    }
    else throw InvalidInstruction(line);
}

void SymbolTable::insert(const string& line) const{
    ValidMachine tester = ValidMachine(line);
    bool isFunc = tester.isInsertFunc();
    tester.parseInsert(isFunc); //If insert a function
    string type = "", returnType = "", name = tester.name;
    int comp = 0, splay = 0, initLevel = 0;

    if(tester.isStatic == "false") //If not static then xet the current level
        initLevel = this->currentLevel;

    //Check type of identifier
    if(tester.type != "string" && tester.type != "number")  type = "func"; //type of identifier
    else    type = tester.type;
    Identifier a = Identifier(name, type);
    a.assignLevel(initLevel);
    Node* add = new Node(a);

    if(type == "func"){
        string param = tester.param;
        add->data.returnType = tester.returnType;
        //Count the number of param
        if(param != ""){
            int paramSize = param.size();
            for(int i = 0; i < paramSize; i++){
                if(param[i] == ',') add->numParam++;
            }
            add->paramList = new string[++add->numParam];
        }
        //Add param to parameter 
        if(add->numParam == 1)  add->paramList[0] = param;
        if(add->numParam > 1){
            int start = 0, comma = 0, num = add->numParam, idx =0 ;
            while(num >= 1){
                comma = param.find(',', start);
                string tmp = param.substr(start, comma-start);
                start = comma+1;
                num--;
                add->paramList[idx] = tmp;
                idx++;
            }
        }
    }
    this->tree->insert_node(this->tree->root, add, comp);
    this->tree->splay(add, splay);
    if(initLevel != 0)  
        this->hangdoi->append(add, initLevel);
    cout << comp << " " << splay << endl; 
}

void SymbolTable::assign(const string& line, int& comp, int& splay){

    ValidMachine tester = ValidMachine(line);
    tester.parseAssign();
    string tobeAssign  = tester.identifier, valueAssign = tester.value;
    
    //If assign a number -> find -> splay
    if(valueAssign[0]  <= '9' && valueAssign[0] >= '0'){ 
        
        int tmpLev = this->currentLevel;
        Node*tmpIdentifier = nullptr;
        tmpIdentifier = this->lookup(tobeAssign, tmpLev, comp);
        if(tmpIdentifier){
            this->tree->splay(tmpIdentifier, splay);
            if(tmpIdentifier->data.type != "number")
                throw TypeMismatch(line); //Found name but not match type    
        }
        else  throw Undeclared(line);
        return;
    }
    
    //If assign a string
    if(valueAssign[0] == '\''){ //Match with the first hyphen
        
        int tmpLev = this->currentLevel;
        Node*tmpIdentifier = nullptr;
        tmpIdentifier = this->lookup(tobeAssign, tmpLev, comp);
        if(tmpIdentifier){
            this->tree->splay(tmpIdentifier, splay);
            if(tmpIdentifier->data.type != "string")
                throw TypeMismatch(line); //Found name but not match type    
        }
        else  throw Undeclared(line);
        return;
    }

    //If call a function to assign for identifier
    if(valueAssign[valueAssign.size()-1] == ')'){
        
        int start = 0, curLevel = this->currentLevel;
        string funcName = valueAssign.substr(start, valueAssign.find('('));
        Node*tmpFuncNode = nullptr;
        //Check the declaration of function name
        tmpFuncNode = this->lookup(funcName, curLevel, comp);
        if(tmpFuncNode){
            this->tree->splay(tmpFuncNode, splay); //Splay node found
            if(tmpFuncNode->data.type != "func")    //if found and type matched
                throw TypeMismatch(line); //If found but type not matched
        }
        else   throw Undeclared(line);
        if(tmpFuncNode){ //Finally, we found then check the param lixt!
            
            start = valueAssign.find('(')+1; int comma = 0, num = tmpFuncNode->numParam, count = 0;
            //Get the real arguments
            string realParam = valueAssign.substr(start, valueAssign.find(')', start) - start);
            if(realParam != ""){ //If the parameter is not empty
                int realParamSize = realParam.size();
                for(int idx = 0; idx < realParamSize; idx++){
                    if(realParam[idx] == ',')   count++; //Count the number of real args
                }
                if(++count != num)   throw TypeMismatch(line); //If no match between args and real args
            }
            start = 0;
            for(int idx = 0 ; idx < num; idx++){ //Check the match in args and real params
                
                comma = realParam.find(',', start);
                string tmpStr = realParam.substr(start, comma-start); //Tokenize each key in args

                //if key is a identifier
                if(tmpStr[0] != '\'' && (tmpStr[0] > '9' || tmpStr[0] < '0') && tmpStr[(int)tmpStr.size()-1] != ')'){
                    
                    Node* paramNode = nullptr; int curLev = this->currentLevel;
                    paramNode = this->lookup(tmpStr, curLev, comp);
                    if(paramNode){
                        this->tree->splay(paramNode, splay);
                        if(paramNode->data.type != tmpFuncNode->paramList[idx]) throw TypeMismatch(line);
                    }
                    if(!paramNode) throw Undeclared(line);
                }
                
                //If key is number
                if(tmpStr[0] <= '9' && tmpStr[0] >=  '0'){
                    if(tmpFuncNode->paramList[idx] != "number")
                        throw TypeMismatch(line);
                    
                }
                //If key is string
                if(tmpStr[0] == '\''){
                    if(tmpFuncNode->paramList[idx] != "string")
                        throw TypeMismatch(line);
                    
                }
                //if key is a function
                if(tmpStr[tmpStr.size() - 1] == ')')    throw TypeMismatch(line);
                start = comma+1;
            }
                Node* tobeAssignNode = nullptr;
                int curLevel = this->currentLevel;
                tobeAssignNode = this->lookup(tobeAssign, curLevel, comp);
                if(tobeAssignNode){
                    this->tree->splay(tobeAssignNode, splay);
                    if(tobeAssignNode->data.type != tmpFuncNode->data.returnType)    throw TypeMismatch(line);
                }
            if(!tobeAssignNode)    throw Undeclared(line);
        
        }
        return;
    
    }
    
    //If assign an identifier which does not have a close bracket at the end
    else{
        int tmpComp = 0;
        int currentLevel = this->currentLevel;
        
        Node* valueNode = nullptr; 
        Node* name_node = nullptr;
        if(!this->tree->root)   throw Undeclared(line);
        if(this->tree->root){
            int valueComp = 0;
            valueNode = this->lookup(valueAssign, currentLevel, valueComp); //If this identifier has not been declared -> throw
            if(!valueNode)  throw Undeclared(line);
            else{
                //cout << "Found: " << valueNode->data.name << endl; //Testing
                if(valueNode->data.type == "func")  throw TypeMismatch(line);
                this->tree->splay(valueNode, splay);
            }
            tmpComp += valueComp;    
        }
        
        if(this->tree->root){
            int idComp = 0;
            name_node = this->lookup(tobeAssign, currentLevel,  idComp);
            if(!name_node)  throw Undeclared(line);
            else{
                this->tree->splay(name_node, splay);
            }    
            tmpComp += idComp;
        }
        comp = tmpComp;
        if(valueNode && name_node){
            if(valueNode->data.type != name_node->data.type)
                throw TypeMismatch(line);
        }
    }
}
Node* SymbolTable::lookup(const string&  name, int destLevel, int& num_comp) 
{
    Node*tmp;
    int tmp_comp = 0;
    while(destLevel != -1){ //SEARCHING Process
        tmp = this->tree->find(name, destLevel, tmp_comp);
        if(!tmp){
            destLevel--;
            tmp_comp = 0;
        } 
        else break;
    }
    num_comp += tmp_comp; //The number of parisons
    return tmp;
}
void SymbolTable::new_scope(){this->currentLevel += 1;}
void SymbolTable::end_scope()
{
    if(this->currentLevel ==0)  throw UnknownBlock();
    if(this->hangdoi->front){
        while(this->hangdoi->front){
            if(hangdoi->front->level == this->currentLevel){
                Node* tobeDel = this->hangdoi->pop_front();
                this->tree->deleteNodeSplay(tobeDel);
            }
            else break;
        }
    }
    this->currentLevel--;            
}
void SymbolTable::print(Node* node, bool& printed)
{
    if(node)
    {   
        printed = true;
        cout << node->data.name <<"//"<<node->data.level;
        if(node->left){
            cout <<" ";
            print(node->left, printed);
        }
        if(node->right){
            cout << " ";
            print(node->right, printed);
        }
    }
}

void SymbolTable::handle_exception_insert(const string& line) //CHECK LỖI LOGIC TRƯỚC KHI THỰC HIỆN INSERT
{
    int comp = 0;
    ValidMachine tester = ValidMachine(line);
    tester.parseInsert(false);
    string name = tester.name, isStatic = tester.isStatic;
    int check_level = (isStatic == "true")?0:this->currentLevel; //Init level first

    //Check INVALID INSTRUCTION (FUNCTION IN LEVEL != 0)
    if(tester.isInsertFunc()){ //If INSERT a function
        if(check_level != 0)
            throw InvalidDeclaration (line);
    }
    //Check REDECLARATION
    Node*tmp = this->tree->find(name, check_level, comp); //Check in the same level
    if(tmp) throw Redeclared(line);

}
void SymbolTable::handle_end_file() const
{
    if(this->currentLevel != 0){throw UnclosedBlock(this->currentLevel);}
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
Node* SplayTree::find(const string& name, int destLevel, int& num_comp)
{
    //So sanh level -> So sanh ten
    Node* searchNode = this->root;
    Node*found = nullptr;
    while(searchNode)
    {
        if(searchNode->data.level < destLevel){
            num_comp++;
            searchNode = searchNode->right;
        }   
        else if(searchNode->data.level > destLevel){
            num_comp++;
            searchNode = searchNode->left;
        }
        else{
            if(searchNode->data.name == name){
                num_comp++;
                found = searchNode; break;
            }
            else if(searchNode->data.name.compare(name) > 0){
                num_comp++;
                searchNode = searchNode->left;
            }  
            else{
                num_comp++;
                searchNode = searchNode->right;
            } 
        }
    }
    return found;
}

void SplayTree::rightRotate(Node* node)
{
    if(!node)   return;
    Node* tmpLeft = node->left; //x
    Node* tmpParent = node->parent; //w
    Node* tmpLeftRight = tmpLeft->right; //z    

    if(tmpParent)
    {
        if(tmpParent->left == node)    tmpParent->left = tmpLeft;
        else if(tmpParent->right == node)   tmpParent->right = tmpLeft;
    }
    if(tmpLeftRight)    
        tmpLeftRight->parent = node;
        
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
        if(tmpParent->left == node)    tmpParent->left = tmpRight;
        else if(tmpParent->right == node) tmpParent->right = tmpRight;
    }
    if(tmpRightLeft)    tmpRightLeft->parent = node;

    node->right = tmpRightLeft;
    tmpRight->left = node;
    node->parent = tmpRight;
    tmpRight->parent = tmpParent;
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
        return; //If root then do nothing
    splay += 1;
    while(node->parent != nullptr)
    {
        //cout << "Parent of "<< node->data.name <<"//"<<node->data.level <<" :" << node->parent->data.name <<"//"<<node->parent->data.level<< endl;//Testing
        // cout << "Splay: " << node->data.name <<"//"<<node->data.level << endl;
        Node* tmpParent = node->parent;
        if(tmpParent->parent == nullptr)
        {   
            if(tmpParent->left == node && tmpParent->left != nullptr)  
                rightRotate(tmpParent);
            else    
                leftRotate(tmpParent);
        }
        else
        {
            Node*tmp = tmpParent->parent;
            if(tmp->left == tmpParent && tmp->left != nullptr){
                if(tmpParent->left == node && tmpParent->left != nullptr)
                    rightRoll(tmp);
                else    leftZZ(tmp);
            }
            else if(tmp->right == tmpParent){
                if(tmpParent->right == node && tmpParent->right != nullptr)
                    leftRoll(tmp);
                else    rightZZ(tmp);
            }
        }
        // cout << "After Splay: " ;
        // this->inorder(this->root);
        // cout << endl;
    }
    this->root = node;
    //node->parent = nullptr; // the parent of root is nullptr
    return;
}

void SplayTree::insert_node(Node* root, Node*add, int& comp)
{
    if(this->root == nullptr)
    {
        this->root = add;
        return;
    }
    if(add->data.getLevel() < root->data.getLevel())
    {
        comp++;
        if(root->left == nullptr){
            root->left = add;
            add->parent = root;
            return;
        }
        else    insert_node(root->left, add, comp);

    }
    else if(add->data.getLevel() > root->data.getLevel())
    {
        comp++;
        if(root->right == nullptr){
            root->right = add;
            add->parent = root;
            return;
        }
        else   insert_node(root->right, add, comp);
    }
    else//Then compare the identifier name
    { 
        auto check = add->data.getName().compare(root->data.getName());
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

void SplayTree::deleteNodeSplay(Node* tobeDel){
    /*
        1. Splay node cần xóa lên root.
        2. Nếu sau khi splay, chỉ có một trong hai cây con thì không cần thay thế, chỉ việc xóa
            Nếu sau khi splay, có cả hai cây con thì thay thế root bằng right most của sub left.
            Nếu không có cây con nào, thì xóa chính node đó và xet root = nulptr
        3. Cập nhật các node
    */
    if(tobeDel == nullptr){
        // cout << "Null!" << endl; //Testing
        return;
    } 
    int splay = 0;
    // cout << "Tobe splay: " << tobeDel->data.name <<"//"<<tobeDel->data.level << endl; //Testing
    this->splay(tobeDel, splay);
    // cout <<"Deleting " <<tobeDel->data.name<<"//"<<tobeDel->data.level << endl;// Testing
    if(this->root->left == nullptr && this->root->right == nullptr){
        // cout << "Node root " << endl;
        this->root = nullptr;
        delete tobeDel;
        return;
    }   
    if(tobeDel->left && tobeDel->right == nullptr){ //Neu chi co 1 cay con trai
        // cout << "Just left" << endl;
        Node*tmp = this->root;
        this->root = this->root->left;
        this->root->parent = nullptr;
        delete tmp;        
        // cout << "End delete..." << endl;    

        return;
    }
    if(tobeDel->left == nullptr && tobeDel->right){ //Neu chi co 1 cay con phai
        // cout << "Just right" << endl;
        Node*tmp = this->root;
        this->root = this->root->right;
        this->root->parent = nullptr;
        delete tmp;
                // cout << "End delete..." << endl;    

        return;
    }
    if(tobeDel->left && tobeDel->right){ //Neu co ca hai cay con
        // cout << "Both" << endl;
        Node* tmpLeft = this->root->left; 
        Node* tmpRight = this->root->right;
        Node* tmp = this->root;
        Node* nodeMax;
        
        if(tmpLeft->right){
            nodeMax = tmpLeft->right;
            nodeMax = this->TreeMax(nodeMax); // Right most of subleft
            this->splay(nodeMax, splay);
            nodeMax->right = tmpRight;
            if(tmpRight)  
                tmpRight->parent = nodeMax;
            this->root = nodeMax;
            this->root->parent = nullptr;
        }

        else{
            tmpLeft->right = tmpRight;
            if(tmpRight)  tmpRight->parent = tmpLeft;
            this->root = tmpLeft;
            this->root->parent = nullptr;
        }
        delete tmp;
        // cout << "End delete..." << endl;    
        //cout <<"Tree :"; this->inorder(this->root);
        return;
    }
}

Node *SplayTree::TreeMax(Node *node) {
    Node*tmp = node;
    if(tmp){
        while(tmp->right)
            tmp = tmp->right;
    }
    return tmp;
}

//================ IDENTIFIER ==================
Identifier::Identifier(string name, string type) {
    this->name = std::move(name);   this->type = std::move(type);
    this->level = 0; 
}
void Identifier:: assignLevel(int level_scope){this->level = level_scope;}
Identifier::Identifier(){this->level = 0; } Identifier::~Identifier(){}
