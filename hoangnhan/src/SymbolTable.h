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

    Symbol(const Symbol &other);
    Symbol(Symbol &&other) noexcept;

    inline const std::string &getName() const noexcept {
        return m_name;
    }

    inline unsigned int getLevel() const noexcept {
        return m_level;
    }

    inline SymbolType getType() const noexcept {
        return m_type;
    }

public:
    virtual ~Symbol() = default;

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


class SymbolTree {

    void splay(const Symbol &data);

    class SymbolNode {
        std::unique_ptr<Symbol> m_data;
        std::unique_ptr<SymbolNode> leftChild;
        std::unique_ptr<SymbolNode> rightChild;

        inline bool hasLeftChild() const noexcept {
            return leftChild != nullptr;
        }

        inline bool hasRightChild() const noexcept {
            return rightChild != nullptr;
        }

        static std::unique_ptr<SymbolNode> rotateWithLeftChild(std::unique_ptr<SymbolNode> node);
        static std::unique_ptr<SymbolNode> rotateWithRightChild(std::unique_ptr<SymbolNode> node);

        friend void SymbolTree::splay(const Symbol &data);
    };

    std::unique_ptr<SymbolNode> m_rootNode;
    /**
     *  SymbolTree::insert
     *  This function insert a new symbol stored in unique_ptr<Symbol> and return true if insert successfully and false if not
     **/
    bool insert(std::unique_ptr<Symbol> *newElement);
    /**
     *  SymbolTree::insert
     *  This function search a symbol that have a name and level that have the same name and level as element,
     *  or the element that have closet match to element if not found
     **/
    std::unique_ptr<Symbol> &search(const Symbol &element);

    void removeAllElementWithScope(int scope);
};

class SymbolTable {
    struct opResult {
        unsigned int compareNum;
        unsigned int splayOpNum;
    };

    bool printFlag = false;

    unsigned int smallestScopeLevel = 0;

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

    opResult handleInsert(std::string &&name, std::string &&type, bool isStatic, const std::string &instruction);
    opResult handleAssign(std::string &&name, std::string &&value, const std::string &instruction);

    inline void handleBegin() {
        smallestScopeLevel++;
    }

    void handleEnd();

    unsigned int handleLookup(std::string &&symbolName, const std::string &instruction);

    std::string handlePrint() const;

public:
    void run(const std::string &filename);
};
