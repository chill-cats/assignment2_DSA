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
    if (this->smallestScopeLevel <= 0) {
        return;
    }
    throw UnclosedBlock(this->smallestScopeLevel);
}

Symbol::Symbol(std::string name, unsigned int level, SymbolType type) : m_level{ level }, m_name{ std::move(name) }, m_type{ type } {}

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
    for (int i = 0; i < paramNum; i++) {
        if (m_paramTypes[i] != paramsToMatch[i]) {
            return false;
        }
    }
    return true;
}
