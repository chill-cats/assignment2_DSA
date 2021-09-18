#include "main.h"

enum class DataType {
    STRING,
    NUMBER
};

enum class SymbolType {
    VARIABLE,
    FUNCTION
};

class Symbol {//NOLINT
    const std::string m_name;
    const unsigned int m_level;
    const SymbolType m_type;

protected:
    Symbol(std::string name, unsigned int level, SymbolType type);
    inline const std::string &getName() const noexcept {
        return m_name;
    }
    inline unsigned int getLevel() const noexcept {
        return m_level;
    }
    inline SymbolType getType() const noexcept {
        return m_type;
    }

    virtual ~Symbol() = default;

public:
    friend bool operator==(const Symbol &lhs, const Symbol &rhs);
    friend bool operator!=(const Symbol &lhs, const Symbol &rhs);
    friend bool operator<=(const Symbol &lhs, const Symbol &rhs);
    friend bool operator>=(const Symbol &lhs, const Symbol &rhs);
    friend bool operator<(const Symbol &lhs, const Symbol &rhs);
    friend bool operator>(const Symbol &lhs, const Symbol &rhs);
};

class FunctionSymbol : public Symbol {
    const DataType m_returnType;
    const std::unique_ptr<DataType[]> m_paramTypes;
    const unsigned int m_paramNum;

public:
    FunctionSymbol(std::string name, unsigned int level, DataType returnType, std::unique_ptr<DataType[]> &paramTypes, unsigned int paramNum);

    inline DataType getReturnType() const noexcept {
        return m_returnType;
    }

    bool isParamMatch(const std::unique_ptr<DataType[]> &paramsToMatch, unsigned int paramNum) const noexcept;
};

class VariableSymbol : public Symbol {
    const DataType m_dataType;

public:
    VariableSymbol(std::string name, unsigned int level, DataType dataType);

    inline DataType getDataType() const noexcept {
        return m_dataType;
    }
};

class SymbolTable {
    bool printFlag{ false };
    int smallestScopeLevel{ 0 };

    inline bool getPrintFlag() const noexcept {
        return printFlag;
    }
    inline void setPrintFlag() noexcept {
        printFlag = true;
    }
    inline void resetPrintFlag() noexcept {
        printFlag = false;
    }

    std::string processLine(const std::string &line);

    void detectUnclosedBlock() const;

public:
    void run(const std::string &filename);
};
