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

    std::smatch tokens;
    if (std::regex_search(line, tokens, INSERT_REGEX)) {
        const auto &name = tokens[1];
        const auto &value = tokens[2];
        const bool isStatic = tokens[3] == "true";

        printFlag = true;
        auto result = insert(name, value, isStatic, line);
        return std::to_string(result.compNum) + ' ' + std::to_string(result.splayNum);
    }
    throw InvalidInstruction(line);
}

SymbolTable::OpResult SymbolTable::insert(const std::string &name, const std::string &value, const bool isStatic, const std::string &line) {
    const int targetLevel = isStatic ? 0 : currentLevel;
    OpResult result;
    std::unique_ptr<Symbol> newData;
    if (value == "string" || value == "number") {
        const Symbol::DataType type = value == "string" ? Symbol::DataType::STRING : Symbol::DataType::NUMBER;
        newData = std::make_unique<VariableSymbol>(name, targetLevel, type);
    } else {
        static const std::regex captureParam(R"((string|number)(?=,|\)))");
        static const std::regex captureType(R"(->(string|number))");

        std::smatch paramMatches;
        std::smatch typeMatches;

        std::regex_search(value, paramMatches, captureParam);
        std::regex_search(value, typeMatches, captureType);

        unsigned long paramNum = paramMatches.size();

        std::unique_ptr<Symbol::DataType[]> param = std::make_unique<Symbol::DataType[]>(paramNum);

        for (unsigned long i = 0; i < paramMatches.size(); i++) {
            param[i] = paramMatches[i] == "string" ? Symbol::DataType::STRING : Symbol::DataType::NUMBER;
        }

        Symbol::DataType returnType = typeMatches[1] == "string" ? Symbol::DataType::STRING : Symbol::DataType::NUMBER;

        newData = std::make_unique<FunctionSymbol>(name, targetLevel, returnType, static_cast<int>(paramNum), std::move(param));
    }
    if (tree.root == nullptr) {
        tree.root = std::make_unique<Tree::TreeNode>(std::move(newData));
        return { 0, 0 };
    }

    result = tree.splay(*newData);
    if (tree.root->data == newData) {
        throw Redeclared(line);
    }
    std::unique_ptr<Tree::TreeNode> newNode = std::make_unique<Tree::TreeNode>(std::move(newData));
    if (*(newNode->data) < *(tree.root->data.get())) {
        newNode->leftChild = std::move(tree.root->leftChild);
        newNode->rightChild = std::move(tree.root);
    } else if (*(newNode->data) > *(tree.root->data.get())) {
        newNode->rightChild = std::move(tree.root->rightChild);
        newNode->leftChild = std::move(tree.root);
    }
    tree.root = std::move(newNode);
    return result;
}
void SymbolTable::detectUnclosedBlock() const {
    if (currentLevel != 0) {
        throw UnclosedBlock(currentLevel);
    }
}
SymbolTable::OpResult SymbolTable::Tree::splay(const Symbol &data) {//NOLINT
    if (root == nullptr) {
        return { 0, 0 };
    }
    std::unique_ptr<TreeNode> leftHeader;
    std::unique_ptr<TreeNode> rightHeader;

    std::unique_ptr<TreeNode> *leftMostNode = &rightHeader;
    std::unique_ptr<TreeNode> *rightMostNode = &leftHeader;

    std::unique_ptr<TreeNode> currentRoot = std::move(root);

    OpResult result{ 0, 0 };

    for (;;) {
        const auto &rootData = *(currentRoot->data);

        if (data < rootData) {
            result.compNum++;
            if (currentRoot->hasLeftChild() && data < *(currentRoot->leftChild->data)) {
                currentRoot = TreeNode::rotateWithRightChild(std::move(currentRoot));
            }
            if (!currentRoot->hasLeftChild()) {
                break;
            }

            if (rightHeader == nullptr) {
                rightHeader = std::move(currentRoot);
            } else {
                (*leftMostNode)->leftChild = std::move(currentRoot);
                leftMostNode = &((*leftMostNode)->leftChild);
            }
            currentRoot = std::move((*leftMostNode)->leftChild);
            result.splayNum++;
            continue;
        }
        if (data > rootData) {
            result.compNum++;
            if (currentRoot->hasRightChild() && data > *(currentRoot->rightChild->data)) {
                currentRoot = TreeNode::rotateWithLeftChild(std::move(currentRoot));
            }
            if (!currentRoot->hasRightChild()) {
                break;
            }
            if (leftHeader == nullptr) {
                leftHeader = std::move(currentRoot);
            } else {
                (*rightMostNode)->rightChild = std::move(currentRoot);
                rightMostNode = &((*rightMostNode)->rightChild);
            }
            currentRoot = std::move((*rightMostNode)->rightChild);
            result.splayNum++;
            continue;
        }
        break;
    }

    if (leftHeader == nullptr) {
        leftHeader = std::move(currentRoot->leftChild);
    } else {
        (*rightMostNode)->rightChild = std::move(currentRoot->leftChild);
    }

    if (rightHeader == nullptr) {
        rightHeader = std::move(currentRoot->rightChild);
    } else {
        (*leftMostNode)->leftChild = std::move(currentRoot->rightChild);
    }


    currentRoot->rightChild = std::move(rightHeader);
    currentRoot->leftChild = std::move(leftHeader);
    root = std::move(currentRoot);
    return result;
}

std::unique_ptr<SymbolTable::Tree::TreeNode> SymbolTable::Tree::TreeNode::rotateWithLeftChild(std::unique_ptr<SymbolTable::Tree::TreeNode> node) {
    std::unique_ptr<SymbolTable::Tree::TreeNode> tempNode = std::move(node->rightChild);
    node->rightChild = std::move(tempNode->leftChild);
    tempNode->leftChild = std::move(node);
    node = std::move(tempNode);
    return node;
}

std::unique_ptr<SymbolTable::Tree::TreeNode> SymbolTable::Tree::TreeNode::rotateWithRightChild(std::unique_ptr<SymbolTable::Tree::TreeNode> node) {
    std::unique_ptr<SymbolTable::Tree::TreeNode> tempNode = std::move(node->leftChild);
    node->leftChild = std::move(tempNode->rightChild);
    tempNode->rightChild = std::move(node);
    node = std::move(tempNode);
    return node;
}

Symbol::Symbol(std::string name, int level, SymbolType symbolType, DataType dataType) : name(std::move(name)), level(level), symbolType(symbolType), dataType(dataType) {}

VariableSymbol::VariableSymbol(const std::string &name, int level, DataType dataType) : Symbol(name, level, SymbolType::VARIABLE, dataType) {}
VariableSymbol::VariableSymbol(const VariableSymbol &other) : Symbol(other.getName(), other.getLevel(), SymbolType::VARIABLE, other.getDataType()) {}

FunctionSymbol::FunctionSymbol(std::string name, int level, DataType returnType, int paramCount, std::unique_ptr<DataType[]> &&paramsType) : Symbol(std::move(name), level, SymbolType::FUNCTION, returnType), paramsType(std::move(paramsType)), paramCount(paramCount) {}

SymbolTable::Tree::TreeNode::TreeNode(std::unique_ptr<Symbol> &&data) : data(std::move(data)) {}
