#include "SymbolTable.h"

void SymbolTable::run(const std::string &filename) {
    std::ifstream file(filename);
    std::string line;
    while (std::getline(file, line)) {
        auto output = this->processLine(line);
        if (printFlag) {
#ifdef __EMSCRIPTEN__
            cout << output << '\n';
#else
            std::cout << output << '\n';
#endif
        }
        printFlag = false;
    }
    this->detectUnclosedBlock();
}

std::string SymbolTable::processLine(const std::string &line) {
    static const std::regex INSERT_REGEX{ R"(^INSERT ([a-z]\w*) (string|number|\((?:|(?:number|string)(?:,(?:number|string))*)\)->(?:number|string)) (true|false)$)" };
    static const std::regex LOOKUP_REGEX{ R"(^LOOKUP ([a-z]\w*)$)" };
    static const std::regex ASSIGN_REGEX{
        R"(^ASSIGN ([a-z]\w*) (\d+|'[\dA-Za-z\s]*'|[a-z]\w*|[a-z]\w*\((?:|(?:\d+|'[\dA-Za-z\s]*'|[a-z]\w*)(?:,(?:\d+|'[\dA-Za-z\s]*'|[a-z]\w*))*)\))$)"
    };

    std::smatch tokens;
    if (std::regex_search(line, tokens, INSERT_REGEX)) {
        const auto &name = tokens[1];
        const auto &value = tokens[2];
        const bool isStatic = tokens[3] == "true";

        printFlag = true;
        auto result = insert(name, value, isStatic, line);
        return std::to_string(result.compNum) + ' ' + std::to_string(result.splayNum);
    }

    if (line == "BEGIN") {
        begin();
        return "";
    }
    if (line == "END") {
        end();
        return "";
    }
    if (line == "PRINT") {
        auto str = tree.toString(TraversalMethod::PREORDER);
        printFlag = !str.empty();
        return std::string{ str.begin(), str.end() - 1 };
    }

    if (std::regex_search(line, tokens, LOOKUP_REGEX)) {
        const auto &name = tokens[1];
        printFlag = true;
        return std::to_string(lookup(name, line));
    }
    if (std::regex_search(line, tokens, ASSIGN_REGEX)) {
        const auto &name = tokens[1];
        const auto &value = tokens[2];
        auto result = assign(name, value, line);
        printFlag = true;
        return std::to_string(result.compNum) + ' ' + std::to_string(result.splayNum);
    }
    throw InvalidInstruction(line);
}

SymbolTable::OpResult SymbolTable::assign(const std::string &name, const std::string &value, const std::string &line) {
    static const std::regex VARIABLE_SYMBOL_REGEX{ R"(^[a-z]\w*$)" };
    static const std::regex LITERAL_STRING_REGEX{ R"(^'[\dA-Za-z\s]*'$)" };
    static const std::regex LITERAL_NUMBER_REGEX{ R"(^\d+)" };
    OpResult result;

    if (std::regex_match(value, LITERAL_NUMBER_REGEX)) {
        auto typeOfAssignee = resolveType(name, result, line);
        if (typeOfAssignee != Symbol::DataType::NUMBER) {
            throw TypeMismatch(line);
        }
        return result;
    }
    if (std::regex_match(value, LITERAL_STRING_REGEX)) {
        auto typeOfAssignee = resolveType(name, result, line);
        if (typeOfAssignee != Symbol::DataType::STRING) {
            throw TypeMismatch(line);
        }
        return result;
    }
    if (std::regex_match(value, VARIABLE_SYMBOL_REGEX)) {
        auto typeOfValue = resolveType(value, result, line);
        auto typeOfAssignee = resolveType(name, result, line);

        if (typeOfAssignee != typeOfValue) {
            throw TypeMismatch(line);
        }

        return result;
    }
    // value is a function
    static const std::regex IN_PARAN_CAPTURE_REGEX{ R"(\((.*?)\))" };
    static const std::regex FUNC_NAME_CAPTURE_REGEX{ R"([a-z]\w*(?=\())" };

    std::smatch tokens;
    // first search for the function symbol
    std::regex_search(value, tokens, FUNC_NAME_CAPTURE_REGEX);

    OpResult findFuctionResult;
    auto *functionNode = findSymbolWithoutSplay(tokens[0], &findFuctionResult);
    if (functionNode == nullptr) {
        throw Undeclared(line);
    }
    if (functionNode->data->getSymbolType() != Symbol::SymbolType::FUNCTION) {
        throw TypeMismatch(line);
    }
    result += findFuctionResult;
    result += tree.splay(functionNode);

    const auto *functionSymbol = dynamic_cast<FunctionSymbol *>(tree.root->data.get());
    if (functionSymbol == nullptr) {
        throw TypeMismatch(line);
    }

    // capture everything inside paranthesis
    std::smatch inParan;
    std::regex_search(value, inParan, IN_PARAN_CAPTURE_REGEX);

    // split by comma
    TokenizeResult tokenizedParams = tokenizeParams(inParan[1].first, inParan[1].second);

    auto paramCount = tokenizedParams.size;
    std::unique_ptr<Symbol::DataType[]> paramsType = std::make_unique<Symbol::DataType[]>(paramCount);

    // resolve type of each param
    for (auto i = 0UL; i < paramCount; i++) {
        paramsType[i] = resolveType(tokenizedParams.data[i], result, line);
    }

    // then match the type of param to type of function
    if (!functionSymbol->matchParams(paramsType, paramCount)) {
        throw TypeMismatch(line);
    }

    // then find the assignee
    auto assigneeType = resolveType(name, result, line);

    // if data type is different
    if (assigneeType != functionNode->data->getDataType()) {
        throw TypeMismatch(line);
    }
    return result;
}

SymbolTable::FunctionDeclarationTokenizeResult SymbolTable::tokenizeFunctionDeclaration(const std::string &functionDeclaration) {
    FunctionDeclarationTokenizeResult result;

    auto firstBracket = functionDeclaration.cbegin();
    auto lastBracket = std::find(functionDeclaration.cbegin(), functionDeclaration.cend(), ')');

    auto paramsTokenizeResult = tokenizeParams(++firstBracket, lastBracket);

    result.params = std::move(paramsTokenizeResult.data);
    result.paramCount = paramsTokenizeResult.size;

    auto arrow = std::find(lastBracket, functionDeclaration.end(), '>');
    result.returnType = std::string(++arrow, functionDeclaration.end());
    return result;
}

SymbolTable::TokenizeResult SymbolTable::tokenizeParams(std::string::const_iterator start, std::string::const_iterator end) {
    if (start == end) {
        return {};
    }

    auto commaNum = std::count_if(start, end, [](char c) { return c == ','; });

    auto tokenNum = commaNum + 1;
    std::unique_ptr<std::string[]> tokens =
        std::make_unique<std::string[]>(static_cast<unsigned long>(tokenNum));
    unsigned long currentIndex = 0;
    auto currentEnd = start;

    for (; currentEnd != end; ++currentEnd) {
        if (*currentEnd == ',') {
            tokens[currentIndex++] = { start, currentEnd };
            ++currentEnd;
            start = currentEnd;
        }
    }

    if (start != end) {
        tokens[currentIndex] = { start, end };
    }

    return { std::move(tokens), static_cast<unsigned long>(tokenNum) };
}

Symbol::DataType SymbolTable::resolveType(const std::string &value, OpResult &result, const std::string &line) {
    static const std::regex NUMBER_REGEX{ R"(\d+)" };
    static const std::regex STRING_REGEX{ R"('[\dA-Za-z\s]*')" };

    if (std::regex_match(value, NUMBER_REGEX)) {
        return Symbol::DataType::NUMBER;
    }

    if (std::regex_match(value, STRING_REGEX)) {
        return Symbol::DataType::STRING;
    }

    auto *node = findSymbolWithoutSplay(value, &result);
    if (node == nullptr) {
        throw Undeclared(line);
    }
    if (node->data->getSymbolType() != Symbol::SymbolType::VARIABLE) {
        throw TypeMismatch(line);
    }
    result += tree.splay(node);
    return tree.root->data->getDataType();
}

void SymbolTable::end() {
    if (currentLevel == 0) {
        throw UnknownBlock();
    }
    tree.deleteAllNodeWithLevel(currentLevel);
    currentLevel--;
}

int SymbolTable::lookup(const std::string &name, const std::string &line) {    // NOLINT
    auto *node = findSymbolWithoutSplay(name, nullptr);

    if (node == nullptr) {
        throw Undeclared(line);
    }

    tree.splay(node);

    return node->data->getLevel();
}

SymbolTable::Tree::TreeNode *SymbolTable::Tree::findSymbolWithoutSplay(const std::string &name, int level, OpResult *result) const noexcept {
    VariableSymbol symbolToSearchFor(name, level, Symbol::DataType::STRING);
    auto *ptr = root;
    for (;;) {
        if (symbolToSearchFor == *static_cast<Symbol *>(ptr->data.get())) {
            if (result != nullptr) {
                result->compNum++;
            }
            return ptr;
        }
        if (symbolToSearchFor < *static_cast<Symbol *>(ptr->data.get())) {
            if (result != nullptr) {
                result->compNum++;
            }
            if (ptr->hasLeftChild()) {
                ptr = ptr->leftChild;
            } else {
                return nullptr;
            }
        } else {
            if (result != nullptr) {
                result->compNum++;
            }
            if (ptr->hasRightChild()) {
                ptr = ptr->rightChild;
            } else {
                return nullptr;
            }
        }
    }
}

SymbolTable::Tree::TreeNode *SymbolTable::findSymbolWithoutSplay(const std::string &name, OpResult *result) const {
    if (tree.root == nullptr) {
        return nullptr;
    }
    auto level = currentLevel;
    Tree::TreeNode *node = nullptr;
    OpResult tempResult;
    while (node == nullptr && level >= 0) {
        node = tree.findSymbolWithoutSplay(name, level, &tempResult);
        level--;
        if (node == nullptr) {    // if not found, reset op counter
            tempResult = {};
        }
    }
    if (result != nullptr) {
        *result += tempResult;
    }
    return node;
}

SymbolTable::OpResult SymbolTable::insert(const std::string &name, const std::string &value, const bool isStatic, const std::string &line) {    // NOLINT
    using TreeNode = Tree::TreeNode;

    const int targetLevel = isStatic ? 0 : currentLevel;
    OpResult result;
    std::unique_ptr<Symbol> newData;

    if (value == "string" || value == "number") {
        const Symbol::DataType type = value == "string" ? Symbol::DataType::STRING : Symbol::DataType::NUMBER;
        newData = std::make_unique<VariableSymbol>(name, targetLevel, type);

    } else {    // if function
        if (targetLevel != 0) {
            throw InvalidDeclaration(line);    // cannot declare a function in level other than 0
        }

        const auto tokenizedFunctionDeclaration = tokenizeFunctionDeclaration(value);

        const auto &paramCount = tokenizedFunctionDeclaration.paramCount;
        const auto &params = tokenizedFunctionDeclaration.params;
        const auto &returnTypeStr = tokenizedFunctionDeclaration.returnType;

        std::unique_ptr<Symbol::DataType[]> param = std::make_unique<Symbol::DataType[]>(paramCount);

        for (unsigned long i = 0; i < paramCount; i++) {
            param[i] = params[i] == "string" ? Symbol::DataType::STRING : Symbol::DataType::NUMBER;
        }

        Symbol::DataType returnType = returnTypeStr == "string" ? Symbol::DataType::STRING : Symbol::DataType::NUMBER;

        newData = std::make_unique<FunctionSymbol>(name, targetLevel, returnType, static_cast<int>(paramCount), std::move(param));
    }

    auto *ptr = tree.root;
    TreeNode *ptrParent = nullptr;

    while (ptr != nullptr) {
        ptrParent = ptr;
        result.compNum++;
        if (*static_cast<Symbol *>(newData.get()) > *static_cast<Symbol *>(ptr->data.get())) {
            ptr = ptr->rightChild;
        } else if (*static_cast<Symbol *>(newData.get()) < *static_cast<Symbol *>(ptr->data.get())) {
            ptr = ptr->leftChild;
        } else {
            throw Redeclared(line);
        }
    }

    ptr = new TreeNode(std::move(newData));
    ptr->parent = ptrParent;

    if (ptrParent == nullptr) {
        tree.root = ptr;
    } else if (*static_cast<Symbol *>(ptr->data.get()) > *static_cast<Symbol *>(ptrParent->data.get())) {
        ptrParent->rightChild = ptr;
    } else {
        ptrParent->leftChild = ptr;
    }

    result += tree.splay(ptr);

    return result;
}
void SymbolTable::detectUnclosedBlock() const {
    if (currentLevel != 0) {
        throw UnclosedBlock(currentLevel);
    }
}

void SymbolTable::begin() noexcept {
    currentLevel++;
}

void SymbolTable::Tree::deleteAllNodeWithLevel(int level) {
    deleteAllNodeWithLevel(root, level);
}

void SymbolTable::Tree::deleteAllNodeWithLevel(TreeNode *currentRoot, const int level) {
    if (currentRoot == nullptr) {
        return;
    }
    deleteAllNodeWithLevel(currentRoot->leftChild, level);
    deleteAllNodeWithLevel(currentRoot->rightChild, level);
    if (currentRoot->data->getLevel() == level) {
        deleteNode(currentRoot);
    }
}

void SymbolTable::Tree::deleteNode(TreeNode *node) {
    if (!node->hasLeftChild() && !node->hasRightChild()) {    // leaf node
        if (node->parent != nullptr) {
            if (node == node->parent->leftChild) {
                node->parent->leftChild = nullptr;
            } else {
                node->parent->rightChild = nullptr;
            }
        } else {
            root = nullptr;
        }
        delete node;
        return;
    }
    if (!node->hasLeftChild()) {    // node doesn't have left child, replace it by it's right child
        if (node->parent != nullptr) {
            if (node == node->parent->leftChild) {
                node->parent->leftChild = node->rightChild;
            } else {
                node->parent->rightChild = node->rightChild;
            }
            node->rightChild->parent = node->parent;
        } else {
            root = node->rightChild;
            node->rightChild->parent = nullptr;
        }
        delete node;
        return;
    }

    // node has left child
    // find greatest element in node's left subtree
    auto *ptr = node->leftChild;
    while (ptr->hasRightChild()) {
        ptr = ptr->rightChild;
    }
    // swap everything between node and greatest node's data
    std::unique_ptr<Symbol> nodeData = std::move(node->data);

    std::unique_ptr<Symbol> sucessorData = std::move(ptr->data);

    node->data = std::move(sucessorData);
    ptr->data = std::move(nodeData);

    if (ptr == ptr->parent->leftChild) {    // ptr is node's direct left child
        node->leftChild = nullptr;
        delete ptr;
        return;
    }
    ptr->parent->rightChild = ptr->leftChild;
    if (ptr->hasLeftChild()) {
        ptr->leftChild->parent = ptr->parent;
    }
    delete ptr;
}

SymbolTable::OpResult SymbolTable::Tree::splay(TreeNode *node) noexcept {    // NOLINT
    OpResult result;
    if (node == root || node == nullptr) {
        return result;
    }
    while (node != root) {
        if (node->parent == root) {    // ZIG case
            result.splayNum++;
            if (node == node->parent->leftChild) {    // ZIG left
                rotateWithLeftChild(node->parent);

            } else if (node == node->parent->rightChild) {
                rotateWithRightChild(node->parent);    // ZIG right
            }
        } else {
            result.splayNum++;
            if (node == node->parent->leftChild && node->parent == node->parent->parent->leftChild) {    // ZIG ZIG left
                rotateWithLeftChild(node->parent->parent);
                rotateWithLeftChild(node->parent);
            } else if (node == node->parent->rightChild && node->parent == node->parent->parent->rightChild) {    // ZIG ZIG right
                rotateWithRightChild(node->parent->parent);
                rotateWithRightChild(node->parent);
            } else if (node == node->parent->leftChild && node->parent == node->parent->parent->rightChild) {    // ZIG ZAG
                rotateWithLeftChild(node->parent);
                rotateWithRightChild(node->parent);
            } else if (node == node->parent->rightChild && node->parent == node->parent->parent->leftChild) {
                rotateWithRightChild(node->parent);
                rotateWithLeftChild(node->parent);
            }
        }
    }

    return result;
}

std::string SymbolTable::Tree::toString(TraversalMethod method) {
    std::string output;
    switch (method) {
    case TraversalMethod::INORDER:
        inOrderToString(root, output);
        break;
    case TraversalMethod::POSTORDER:
        postOrderToString(root, output);
        break;
    case TraversalMethod::PREORDER:
        preOrderToString(root, output);
    }
    return output;
}

void SymbolTable::Tree::preOrderToString(const TreeNode *currentRoot, std::string &output) {
    if (currentRoot == nullptr) {
        return;
    }
    output += currentRoot->data->toString();
    output += ' ';
    preOrderToString(currentRoot->leftChild, output);
    preOrderToString(currentRoot->rightChild, output);
}

void SymbolTable::Tree::inOrderToString(const TreeNode *currentRoot, std::string &output) {
    if (currentRoot == nullptr) {
        return;
    }
    inOrderToString(currentRoot->leftChild, output);
    output += currentRoot->data->toString();
    output += ' ';
    inOrderToString(currentRoot->rightChild, output);
}

void SymbolTable::Tree::postOrderToString(const TreeNode *currentRoot, std::string &output) {
    if (currentRoot == nullptr) {
        return;
    }
    preOrderToString(currentRoot->leftChild, output);
    preOrderToString(currentRoot->rightChild, output);
    output += currentRoot->data->toString();
    output += ' ';
}

void SymbolTable::Tree::rotateWithLeftChild(TreeNode *node) noexcept {
    if (node == nullptr || !node->hasLeftChild()) {
        return;
    }
    auto *oldLeftChild = node->leftChild;
    auto *parent = node->parent;

    node->leftChild = oldLeftChild->rightChild;
    if (node->hasLeftChild()) {
        node->leftChild->parent = node;
    }
    oldLeftChild->rightChild = node;

    if (node->parent == nullptr) {
        node->parent = oldLeftChild;
        oldLeftChild->parent = nullptr;
        root = oldLeftChild;
        return;
    }
    if (node == node->parent->leftChild) {
        node->parent->leftChild = oldLeftChild;
        node->parent = oldLeftChild;
        oldLeftChild->parent = parent;
        return;
    }
    if (node == node->parent->rightChild) {
        node->parent->rightChild = oldLeftChild;
        node->parent = oldLeftChild;
        oldLeftChild->parent = parent;
        return;
    }
}


void SymbolTable::Tree::rotateWithRightChild(TreeNode *node) noexcept {
    if (node == nullptr || !node->hasRightChild()) {
        return;
    }
    auto *oldRightChild = node->rightChild;
    auto *parent = node->parent;

    node->rightChild = oldRightChild->leftChild;
    if (node->hasRightChild()) {
        node->rightChild->parent = node;
    }

    oldRightChild->leftChild = node;

    if (node->parent == nullptr) {
        node->parent = oldRightChild;
        oldRightChild->parent = nullptr;
        root = oldRightChild;
        return;
    }
    if (node == node->parent->leftChild) {
        node->parent->leftChild = oldRightChild;
        node->parent = oldRightChild;
        oldRightChild->parent = parent;
        return;
    }
    if (node == node->parent->rightChild) {
        node->parent->rightChild = oldRightChild;
        node->parent = oldRightChild;
        oldRightChild->parent = parent;
        return;
    }
}

SymbolTable::Tree::~Tree() {
    delete root;
}

Symbol::Symbol(std::string name, int level, SymbolType symbolType, DataType dataType) : name(std::move(name)), level(level), symbolType(symbolType), dataType(dataType) {}
bool Symbol::operator==(const Symbol &rhs) const noexcept {
    return level == rhs.level && name == rhs.name;
}
bool Symbol::operator<(const Symbol &rhs) const noexcept {
    if (level == rhs.level) {
        return name < rhs.name;
    }
    return level < rhs.level;
}
bool Symbol::operator>(const Symbol &rhs) const noexcept {
    if (level == rhs.level) {
        return name > rhs.name;
    }
    return level > rhs.level;
}
std::string Symbol::toString() const {
    return name + "//" + std::to_string(level);
}

VariableSymbol::VariableSymbol(const std::string &name, int level, DataType dataType) : Symbol(name, level, SymbolType::VARIABLE, dataType) {}

VariableSymbol::VariableSymbol(const VariableSymbol &other) : Symbol(other.getName(), other.getLevel(), SymbolType::VARIABLE, other.getDataType()) {}

FunctionSymbol::FunctionSymbol(std::string name, int level, DataType returnType, int paramCount, std::unique_ptr<DataType[]> &&paramsType) : Symbol(std::move(name), level, SymbolType::FUNCTION, returnType), paramsType(std::move(paramsType)), paramCount(paramCount) {}

bool FunctionSymbol::matchParams(const std::unique_ptr<DataType[]> &paramsToMatch, unsigned long count) const {
    if (paramCount != static_cast<int>(count)) {
        return false;
    }

    for (int i = 0; i < static_cast<int>(count); i++) {
        if (paramsToMatch[static_cast<unsigned long>(i)] != paramsType[static_cast<unsigned long>(i)]) {
            return false;
        }
    }
    return true;
}

SymbolTable::Tree::TreeNode::TreeNode(std::unique_ptr<Symbol> &&data) : data(std::move(data)) {}

SymbolTable::Tree::TreeNode::~TreeNode() {
    if (hasLeftChild()) {
        delete leftChild;
    }
    if (hasRightChild()) {
        delete rightChild;
    }
}

SymbolTable::OpResult &SymbolTable::OpResult::operator+=(const OpResult &rhs) {
    compNum += rhs.compNum;
    splayNum += rhs.splayNum;
    return *this;
}
