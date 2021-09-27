#include "SymbolTable.h"

void SymbolTable::run(const std::string &filename) {
    std::ifstream file(filename);
    std::string line;
    while (std::getline(file, line)) {
        auto output = this->processLine(line);
        if (this->getPrintFlag()) {
#ifdef __EMSCRIPTEN__
            cout << output << '\n';
#else
            std::cout << output << '\n';
#endif
        }
        this->resetPrintFlag();
    }
    this->detectUnclosedBlock();
}

void SymbolTable::detectUnclosedBlock() const {
    if (this->smallestScopeLevel == 0) {
        return;
    }
    throw UnclosedBlock(static_cast<int>(this->smallestScopeLevel));
}

std::string SymbolTable::processLine(const std::string &line) {
    static const std::regex INSERT_REGEX{ R"(^INSERT ([a-z]\w*) (string|number|\((?:|(?:number|string)(?:,(?:number|string))*)\)->(?:number|string)) (true|false)$)" };
    static const std::regex ASSIGN_REGEX{
        R"(^ASSIGN ([a-z]\w*) (\d+|'[\dA-Za-z\s]*'|[a-z]\w*|[a-z]\w*\((?:|(?:\d+|'[\dA-Za-z\s]*'|[a-z]\w*)(?:,(?:\d+|'[\dA-Za-z\s]*'|[a-z]\w*))*)\))$)"
    };
    static const char *BEGIN_COMMAND{ "BEGIN" };
    static const char *END_COMMAND{ "END" };
    static const std::regex LOOKUP_REGEX{ R"()" };
    static const char *PRINT_COMMAND{ "PRINT" };

    std::smatch matches;
    if (std::regex_search(line, matches, INSERT_REGEX)) {
        auto symbolName = matches[1];
        auto symbolType = matches[2];
        auto isStatic = matches[2] == "true";

        auto result = handleInsert(symbolName, symbolType, isStatic, line);

        setPrintFlag();
        return std::to_string(result.compareNum) + ' ' + std::to_string(result.splayOpNum);
    }
    if (std::regex_search(line, matches, ASSIGN_REGEX)) {
        auto symbolName = matches[1];
        auto value = matches[2];

        auto result = handleAssign(symbolName, value, line);

        setPrintFlag();
        return std::to_string(result.compareNum) + ' ' + std::to_string(result.splayOpNum);
    }
    if (line == BEGIN_COMMAND) {
        handleBegin();

        resetPrintFlag();
        return {};
    }
    if (line == END_COMMAND) {
        handleEnd();

        resetPrintFlag();
        return {};
    }
    if (std::regex_search(line, matches, LOOKUP_REGEX)) {
        auto symbolName = matches[1];
        auto result = handleLookup(symbolName, line);

        setPrintFlag();
        return std::to_string(result);
    }
    if (line == PRINT_COMMAND) {
        auto result = handlePrint();

        setPrintFlag();
        return result;
    }
    throw InvalidInstruction(line);
}

SymbolTable::opResult SymbolTable::handleInsert(std::string &&name, std::string &&type, bool isStatic, const std::string &instruction) {
    return {};
}

SymbolTable::opResult SymbolTable::handleAssign(std::string &&name, std::string &&value, const std::string &instruction) {
    return {};
}

void SymbolTable::handleEnd() {
}

unsigned int SymbolTable::handleLookup(std::string &&symbolName, const std::string &instruction) {
    return 0;
}

std::string SymbolTable::handlePrint() const {
    return {};
}

Symbol::Symbol(std::string name, unsigned int level, SymbolType type) : m_name{ std::move(name) }, m_level{ level }, m_type{ type } {}
Symbol::Symbol(Symbol &&other) noexcept : m_name(other.m_name), m_level(other.m_level), m_type{ other.m_type } {}

bool operator==(const Symbol &lhs, const Symbol &rhs) {
    return lhs.m_level == rhs.m_level && lhs.m_name == rhs.m_name;
}

bool operator!=(const Symbol &lhs, const Symbol &rhs) {
    return !(lhs == rhs);
}

bool operator<(const Symbol &lhs, const Symbol &rhs) {
    if (lhs.m_level != rhs.m_level) {
        return lhs.m_level < rhs.m_level;
    }
    return lhs.m_name < rhs.m_name;
}

bool operator>(const Symbol &lhs, const Symbol &rhs) {
    return rhs < lhs;
}

bool operator<=(const Symbol &lhs, const Symbol &rhs) {
    return !(lhs > rhs);
}

bool operator>=(const Symbol &lhs, const Symbol &rhs) {
    return !(lhs < rhs);
}

VariableSymbol::VariableSymbol(std::string name, unsigned int level, DataType dataType) : Symbol{ std::move(name), level, SymbolType::VARIABLE }, m_dataType{ dataType } {}

FunctionSymbol::FunctionSymbol(
    std::string name,
    unsigned int level,
    DataType returnType,
    std::unique_ptr<DataType[]> &paramTypes,
    unsigned int paramNum) : Symbol{ std::move(name), level, SymbolType::FUNCTION },
                             m_returnType{ returnType },
                             m_paramTypes{ std::move(paramTypes) },
                             m_paramNum{ paramNum } {}

bool FunctionSymbol::isParamMatch(const std::unique_ptr<DataType[]> &paramsToMatch, unsigned int paramNum) const noexcept {
    if (paramNum != m_paramNum) {
        return false;
    }
    for (unsigned int i = 0; i < paramNum; i++) {
        if (m_paramTypes[i] != paramsToMatch[i]) {
            return false;
        }
    }
    return true;
}


void SymbolTree::splay(const Symbol &data) {//NOLINT
    if (m_rootNode == nullptr) {
        return;
    }
    std::unique_ptr<SymbolNode> leftHeader;
    std::unique_ptr<SymbolNode> rightHeader;

    std::unique_ptr<SymbolNode> *leftMostNode = &rightHeader;
    std::unique_ptr<SymbolNode> *rightMostNode = &leftHeader;

    std::unique_ptr<SymbolNode> currentRoot = std::move(m_rootNode);

    for (;;) {
        const auto &rootData = *(currentRoot->m_data);

        if (data < rootData) {
            if (currentRoot->hasLeftChild() && data < *(currentRoot->leftChild->m_data)) {
                currentRoot = SymbolNode::rotateWithRightChild(std::move(currentRoot));
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
            continue;
        }
        if (data > rootData) {
            if (currentRoot->hasRightChild() && data > *(currentRoot->rightChild->m_data)) {
                currentRoot = SymbolNode::rotateWithLeftChild(std::move(currentRoot));
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
    m_rootNode = std::move(currentRoot);
}

std::unique_ptr<SymbolTree::SymbolNode> SymbolTree::SymbolNode::rotateWithLeftChild(std::unique_ptr<SymbolNode> node) {
    std::unique_ptr<SymbolTree::SymbolNode> tempNode = std::move(node->rightChild);
    node->rightChild = std::move(tempNode->leftChild);
    tempNode->leftChild = std::move(node);
    node = std::move(tempNode);
    return node;
}

std::unique_ptr<SymbolTree::SymbolNode> SymbolTree::SymbolNode::rotateWithRightChild(std::unique_ptr<SymbolNode> node) {
    std::unique_ptr<SymbolTree::SymbolNode> tempNode = std::move(node->leftChild);
    node->leftChild = std::move(tempNode->rightChild);
    tempNode->rightChild = std::move(node);
    node = std::move(tempNode);
    return node;
}
