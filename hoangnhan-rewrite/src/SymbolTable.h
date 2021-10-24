#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H
#include "main.h"
#include <iterator>

template<typename T>
class FixedSizeVec {
public:
    using value_type = T;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using pointer = T *;
    using const_pointer = const T *;
    using reference = T &;
    using const_reference = const T &;
    using iterator = pointer;
    using const_iterator = const_pointer;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

private:
    std::unique_ptr<value_type[]> m_data;    // NOLINT(cppcoreguidelines-avoid-c-arrays,modernize-avoid-c-arrays,hicpp-avoid-c-arrays): We are not allow to use vector
    size_type m_size = 0;

public:
    FixedSizeVec();
    explicit FixedSizeVec(size_type size);
    FixedSizeVec(const FixedSizeVec &other);
    FixedSizeVec(FixedSizeVec &&other) noexcept;
    template<typename Iter>
    FixedSizeVec(Iter begin, Iter end);
    ~FixedSizeVec() = default;

    FixedSizeVec &operator=(const FixedSizeVec &other);
    FixedSizeVec &operator=(FixedSizeVec &&other) noexcept;

    reference operator[](size_type index);
    const_reference operator[](size_type index) const;

    iterator begin() noexcept;
    const_iterator begin() const noexcept;
    const_iterator cbegin() const noexcept;

    iterator end() noexcept;
    const_iterator end() const noexcept;
    const_iterator cend() const noexcept;

    reverse_iterator rbegin() noexcept;
    const_reverse_iterator rbegin() const noexcept;
    const_reverse_iterator crbegin() const noexcept;

    reverse_iterator rend() noexcept;
    const_reverse_iterator rend() const noexcept;
    const_reverse_iterator crend() const noexcept;

    size_type size() const noexcept;
    bool empty() const noexcept;
};
template<typename T>
FixedSizeVec<T>::FixedSizeVec() : m_data(std::make_unique<value_type[]>(0)) {}    // NOLINT

template<typename T>
FixedSizeVec<T>::FixedSizeVec(size_type size) : m_data(std::make_unique<value_type[]>(size)), m_size(size) {}    // NOLINT

template<typename T>
FixedSizeVec<T>::FixedSizeVec(const FixedSizeVec &other) : m_data(std::make_unique<value_type[]>(other.m_size)), m_size(other.m_size) {    // NOLINT
    for (size_type i = 0; i < m_size; ++i) {
        m_data[i] = other.m_data[i];
    }
}

template<typename T>
FixedSizeVec<T>::FixedSizeVec(FixedSizeVec &&other) noexcept : m_data(std::move(other.m_data)), m_size(other.m_size) {    // NOLINT
    other.m_size = 0;
}

template<typename T>
template<typename Iter>
FixedSizeVec<T>::FixedSizeVec(Iter begin, Iter end) : m_data(std::make_unique<value_type[]>(end - begin)), m_size(end - begin) {    // NOLINT
    size_type index = 0;
    for (const auto &it = begin; it != end; ++it) {
        m_data[index++] = it;
    }
}

template<typename T>
FixedSizeVec<T> &FixedSizeVec<T>::operator=(const FixedSizeVec &other) {
    if (&other == this) {
        return *this;
    }
    m_data = std::make_unique<value_type[]>(other.m_size);    // NOLINT
    m_size = other.m_size;
    for (size_type i = 0; i < m_size; ++i) {
        m_data[i] = other.m_data[i];
    }
    return *this;
}

template<typename T>
FixedSizeVec<T> &FixedSizeVec<T>::operator=(FixedSizeVec &&other) noexcept {
    m_data = std::move(other.m_data);
    m_size = other.m_size;
    other.m_size = 0;
    return *this;
}

template<typename T>
typename FixedSizeVec<T>::reference FixedSizeVec<T>::operator[](size_type index) {
    return m_data[index];
}

template<typename T>
typename FixedSizeVec<T>::const_reference FixedSizeVec<T>::operator[](size_type index) const {
    return m_data[index];
}
template<typename T>
typename FixedSizeVec<T>::iterator FixedSizeVec<T>::begin() noexcept {
    return m_data.get();
}

template<typename T>
typename FixedSizeVec<T>::const_iterator FixedSizeVec<T>::begin() const noexcept {
    return m_data.get();
}

template<typename T>
typename FixedSizeVec<T>::const_iterator FixedSizeVec<T>::cbegin() const noexcept {
    return m_data.get();
}

template<typename T>
typename FixedSizeVec<T>::iterator FixedSizeVec<T>::end() noexcept {
    return m_data.get() + m_size;
}

template<typename T>
typename FixedSizeVec<T>::const_iterator FixedSizeVec<T>::end() const noexcept {
    return m_data.get() + m_size;
}

template<typename T>
typename FixedSizeVec<T>::const_iterator FixedSizeVec<T>::cend() const noexcept {
    return m_data.get() + m_size;
}

template<typename T>
typename FixedSizeVec<T>::reverse_iterator FixedSizeVec<T>::rbegin() noexcept {
    return std::reverse_iterator<iterator>(m_data.get() + m_size);
}

template<typename T>
typename FixedSizeVec<T>::const_reverse_iterator FixedSizeVec<T>::rbegin() const noexcept {
    return std::reverse_iterator<const_iterator>(m_data.get() + m_size);
}

template<typename T>
typename FixedSizeVec<T>::const_reverse_iterator FixedSizeVec<T>::crbegin() const noexcept {
    return std::reverse_iterator<const_iterator>(m_data.get() + m_size);
}

template<typename T>
typename FixedSizeVec<T>::reverse_iterator FixedSizeVec<T>::rend() noexcept {
    return std::reverse_iterator<const_iterator>(m_data.get());
}

template<typename T>
typename FixedSizeVec<T>::const_reverse_iterator FixedSizeVec<T>::rend() const noexcept {
    return std::reverse_iterator<const_iterator>(m_data.get());
}

template<typename T>
typename FixedSizeVec<T>::const_reverse_iterator FixedSizeVec<T>::crend() const noexcept {
    return std::reverse_iterator<const_iterator>(m_data.get());
}
template<typename T>
typename FixedSizeVec<T>::size_type FixedSizeVec<T>::size() const noexcept {
    return m_size;
}

template<typename T>
bool FixedSizeVec<T>::empty() const noexcept {
    return size() == 0;
}

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
    bool operator==(const Symbol &rhs) const noexcept;
    bool operator<(const Symbol &rhs) const noexcept;
    bool operator>(const Symbol &rhs) const noexcept;

    bool equal(const std::string &nameToComp, int levelToComp) const noexcept;
    bool lessThan(const std::string &nameToComp, int levelToComp) const noexcept;
    bool greaterThan(const std::string &nameToComp, int levelToComp) const noexcept;

    std::string toString() const;
};

class FunctionSymbol : public Symbol {    // NOLINT
    FixedSizeVec<DataType> paramsType;    // NOLINT

public:
    FunctionSymbol(std::string name, int level, DataType returnType, FixedSizeVec<DataType> &&paramsType);    // NOLINT
    FunctionSymbol(const FunctionSymbol &other);

    bool matchParams(FixedSizeVec<DataType>) const;    // NOLINT
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
        std::string returnType;
        FixedSizeVec<std::string> params;
    };

    struct FunctionCallTokenizeResult {
        std::string functionName;
        FixedSizeVec<std::string> params;
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
            ScopeNode *front = nullptr;
            ScopeNode *rear = nullptr;

            Scope *nextScope = nullptr;

        public:
            void pushRear(Tree::TreeNode *node);
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

        ~SymbolList();
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
