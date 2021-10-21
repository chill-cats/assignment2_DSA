#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H
#include "main.h"

class Symbol {    // NOLINT

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
    virtual bool operator==(const Symbol &rhs) const noexcept;
    virtual bool operator<(const Symbol &rhs) const noexcept;
    virtual bool operator>(const Symbol &rhs) const noexcept;

    std::string toString() const;
};

class FunctionSymbol : public Symbol {    // NOLINT

    const std::unique_ptr<DataType[]> paramsType;    // NOLINT
    const int paramCount = 0;

public:
    FunctionSymbol(std::string name, int level, DataType returnType, int paramCount, std::unique_ptr<DataType[]> &&paramsType);    // NOLINT
    FunctionSymbol(const FunctionSymbol &other);

    bool matchParams(const std::unique_ptr<DataType[]> &paramsToMatch, unsigned long count) const;    // NOLINT
};

class VariableSymbol : public Symbol {    // NOLINT

public:
    VariableSymbol(const std::string &name, int level, DataType dataType);
    VariableSymbol(const VariableSymbol &other);
};

class SymbolTable {
    struct OpResult {
        int compNum = 0;     // NOLINT
        int splayNum = 0;    // NOLINT
        OpResult &operator+=(const OpResult &rhs);
    };

    struct TokenizeResult {
        std::unique_ptr<std::string[]> data;    // NOLINT
        unsigned long size = 0;
    };

    struct FunctionDeclarationTokenizeResult {
        std::unique_ptr<std::string[]> params;    // NOLINT
        unsigned long paramCount = 0;
        std::string returnType;
    };

    struct FunctionCallTokenizeResult {
        std::string functionName;

        std::unique_ptr<std::string[]> paramsList;    // NOLINT
        unsigned long paramsCount = 0;
    };

    enum class TraversalMethod {
        PREORDER,
        INORDER,
        POSTORDER
    };

    enum class ValueType {
        STRING,
        NUMBER,
        SYMBOL,
        FUNCTION_CALL,
    };

    class Tree {            // NOLINT
        class TreeNode {    // NOLINT
            std::unique_ptr<Symbol> data;
            TreeNode *parent = nullptr;

            TreeNode *leftChild = nullptr;
            TreeNode *rightChild = nullptr;

            inline bool hasLeftChild() const noexcept {
                return leftChild != nullptr;
            }

            inline bool hasRightChild() const noexcept {
                return rightChild != nullptr;
            }
            inline bool isMyLeftChild(const TreeNode *const node) const noexcept {
                return hasLeftChild() && leftChild == node;
            }
            inline bool isMyRightChild(const TreeNode *const node) const noexcept {
                return hasRightChild() && rightChild == node;
            }

        public:
            explicit TreeNode(std::unique_ptr<Symbol> &&data);
            TreeNode(std::unique_ptr<Symbol> &&data, TreeNode *leftChild, TreeNode *rightChild, TreeNode *parent = nullptr);
            ~TreeNode();
            friend class Tree;
            friend class SymbolTable;
        };


        TreeNode *root = nullptr;

        OpResult splay(TreeNode *node) noexcept;

        /**
         * @brief   this function rotate a node with it's right child and return a pointer to new root
         * @param   node Node to rotate with it's right child
         * @return  node's right child now become new root of that subtree
         */
        void rotateWithRightChild(TreeNode *node) noexcept;

        /**
         * @brief   this function rotate a node with it's left child and return a pointer to new root
         * @param   node Node to rotate with it's left child
         * @return  node's left child now become new root of that subtree
         */
        void rotateWithLeftChild(TreeNode *node) noexcept;

        std::string toString(TraversalMethod method);

        static void preOrderToString(const TreeNode *currentRoot, std::string &output);
        static void inOrderToString(const TreeNode *currentRoot, std::string &output);
        static void postOrderToString(const TreeNode *currentRoot, std::string &output);


        TreeNode *findSymbolWithoutSplay(const std::string &name, int level, SymbolTable::OpResult *result) const noexcept;

        void deleteNode(TreeNode *node);
        ~Tree();
        friend class SymbolTable;
    };

    class SymbolList {
        class Scope {
            struct ScopeNode {
                Tree::TreeNode *ptr = nullptr;
                ScopeNode *next = nullptr;
            };
            ScopeNode *head = nullptr;

            Scope *nextScope = nullptr;

        public:
            void pushFront(Tree::TreeNode *node);
            Tree::TreeNode *popFront();

            Scope() = default;
            Scope(Scope &&other) = delete;
            Scope(const Scope &other) = delete;

            Scope &operator=(const Scope &other) = delete;
            Scope &operator=(Scope &&other) = delete;

            ~Scope();

            friend class SymbolList;
        };
        Scope *head = nullptr;
        Scope *tail = nullptr;

    public:
        void addMoreScope();
        void deleteScope();

        Scope *getHead() const noexcept;
        Scope *getTail() const noexcept;
    };

    int currentLevel = 0;
    bool printFlag = false;

    Tree tree;
    SymbolList symbols;


    std::string processLine(const std::string &line);

    void detectUnclosedBlock() const;

    OpResult insert(const std::string &name, const std::string &value, bool isStatic, const std::string &line);

    void begin() noexcept;
    void end();

    Symbol::DataType resolveType(const std::string &value, OpResult &result, const std::string &line);

    static SymbolTable::ValueType resolveValueType(const std::string &value);

    int lookup(const std::string &name, const std::string &line);

    Tree::TreeNode *findSymbolWithoutSplay(const std::string &name, OpResult *result) const;

    OpResult assign(const std::string &name, const std::string &value, const std::string &line);

    static TokenizeResult tokenizeParams(std::string::const_iterator start, std::string::const_iterator end);
    static FunctionDeclarationTokenizeResult tokenizeFunctionDeclaration(const std::string &functionDeclaration);
    static FunctionCallTokenizeResult tokenizeFunctionCall(const std::string &functionCall);

public:
    SymbolTable();
    void run(const string &filename);
};
#endif
