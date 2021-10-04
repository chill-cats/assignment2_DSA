#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H
#include "main.h"

class Symbol {//NOLINT

public:
    enum class SymbolType {
        FUNCTION,
        VARIABLE,
    };
    enum class DataType {
        STRING,
        NUMBER,
    };

private:
    const std::string name;
    const int level = 0;
    const SymbolType symbolType = SymbolType::VARIABLE;
    const DataType dataType;

protected:
    Symbol(std::string name, int level, SymbolType symbolType, DataType dataType);
    Symbol(const Symbol &other);

public:
    virtual ~Symbol() = default;

    inline const std::string &getName() const noexcept {
        return name;
    }

    inline int getLevel() const noexcept {
        return level;
    }

    inline SymbolType getSymbolType() const noexcept {
        return symbolType;
    }

    inline DataType getDataType() const noexcept {
        return dataType;
    }
    inline bool operator==(const Symbol &rhs) const noexcept {
        return level == rhs.level && name == rhs.name;
    }
    inline bool operator<(const Symbol &rhs) const noexcept {
        if (level == rhs.level) {
            return name < rhs.name;
        }
        return level < rhs.level;
    }
    inline bool operator>(const Symbol &rhs) const noexcept {
        if (level == rhs.level) {
            return name > rhs.name;
        }
        return level > rhs.level;
    }
};

class FunctionSymbol : public Symbol {//NOLINT

    const std::unique_ptr<DataType[]> paramsType;
    const int paramCount = 0;

public:
    FunctionSymbol(std::string name, int level, DataType returnType, int paramCount, std::unique_ptr<DataType[]> &&paramsType);
    FunctionSymbol(const FunctionSymbol &other);
};

class VariableSymbol : public Symbol {//NOLINT

public:
    VariableSymbol(const std::string &name, int level, DataType dataType);
    VariableSymbol(const VariableSymbol &other);
};

class SymbolTable {
    struct OpResult {
        int compNum = 0;
        int splayNum = 0;
    };

    class Tree {
        class TreeNode {
            std::unique_ptr<Symbol> data;
            std::unique_ptr<TreeNode> leftChild;
            std::unique_ptr<TreeNode> rightChild;

            TreeNode *parent = nullptr;
            inline bool hasLeftChild() const {
                return leftChild != nullptr;
            }

            inline bool hasRightChild() const {
                return rightChild != nullptr;
            }

            static std::unique_ptr<TreeNode> rotateWithRightChild(std::unique_ptr<TreeNode> node);
            static std::unique_ptr<TreeNode> rotateWithLeftChild(std::unique_ptr<TreeNode> node);

        public:
            explicit TreeNode(std::unique_ptr<Symbol> &&data);
            TreeNode(std::unique_ptr<Symbol> &&data, std::unique_ptr<TreeNode> &&leftChild, std::unique_ptr<TreeNode> &&rightChild, TreeNode *parent = nullptr);
            friend class Tree;
            friend class SymbolTable;
        };

        std::unique_ptr<TreeNode> root;

        OpResult splay(const Symbol &data);

        friend class SymbolTable;
    };

    int currentLevel = 0;
    bool printFlag = false;

    Tree tree;

public:
    void run(const string &filename);
    std::string processLine(const std::string &line);
    void detectUnclosedBlock() const;

    OpResult insert(const std::string &name, const std::string &value, bool isStatic, const std::string &line);
};
#endif
