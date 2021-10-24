#include "SymbolTable.h"
#define EXPERIMENTAL_PARSING 1
namespace match {
using TokenizedParam = FixedSizeVec<std::string>;
using StrCIter = std::string::const_iterator;

enum class InstructionType {
    INSERT,
    ASSIGN,
    LOOKUP,
    PRINT,
    BEGIN,
    END,
};

struct MatchResult {
    InstructionType type = InstructionType::PRINT;
    TokenizedParam params;
};

struct TokenizedFunctionDeclaration {
    Symbol::DataType returnType = Symbol::DataType::STRING;
    FixedSizeVec<Symbol::DataType> paramType;
};

enum class AssignValueType {
    LITERAL_NUMBER,
    LITERAL_STRING,
    IDENTIFIER,
    FUNC_CALL
};

struct ParsedAssignValue {
    AssignValueType type = AssignValueType::FUNC_CALL;
    FixedSizeVec<std::string> param;    // this will have size of 0 when type is LITERAL_STRING or LITERAL_NUMBER
                                        // size of 1 and store name of Symbol if type is IDENTIFIER
                                        // size of min 1 and store name of function at index 0 and param at index > 0
};

ParsedAssignValue parseAssignValue(const std::string &value, const std::string &line) {    // NOLINT
    if (value.empty() || ('A' <= *value.begin() && *value.begin() <= 'Z')) {
        throw InvalidInstruction(line);
    }

    if (std::all_of(value.begin(), value.end(), [](char c) { return '0' <= c && c <= '9'; })) {
        return { AssignValueType::LITERAL_NUMBER, FixedSizeVec<std::string>() };
    }

    if (value.length() >= 2 && *value.begin() == '\'' && *std::prev(value.end()) == '\'') {
        if (std::any_of(std::next(value.begin()), std::prev(value.end()), [](char c) {
                return (c < '0' || '9' < c) && (c < 'a' || 'z' < c) && (c < 'A' || 'Z' < c) && c != ' ';
            })) {
            throw InvalidInstruction(line);
        }
        return { AssignValueType::LITERAL_STRING, FixedSizeVec<std::string>() };
    }

    if (std::all_of(value.begin(), value.end(), [](char c) {
            return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || ('0' <= c && c <= '9') || c == '_';
        })) {
        ParsedAssignValue val{ AssignValueType::IDENTIFIER, FixedSizeVec<std::string>(1) };
        val.param[0] = value;
        return val;
    }

    // minimal function call: a()
    const size_t MIN_LENGTH = 3;
    if (value.length() < MIN_LENGTH) {
        throw InvalidInstruction(line);
    }
    auto openParan = std::find(value.begin(), value.end(), '(');
    if (openParan == value.end()) {
        throw InvalidInstruction(line);
    }
    auto closeParan = std::find(value.begin(), value.end(), ')');
    if (closeParan != std::prev(value.end())) {
        throw InvalidInstruction(line);
    }
    if (std::next(openParan) == closeParan) {
        ParsedAssignValue val{ AssignValueType::FUNC_CALL, FixedSizeVec<std::string>(1) };
        val.param[0] = { value.begin(), openParan };
        return val;
    }

    auto commaNum = std::count_if(std::next(openParan), closeParan, [](char c) { return c == ','; });
    auto tokenNum = commaNum + 1;

    unsigned long currentIndex = 1;
    auto currentEnd = std::next(openParan);
    auto start = currentEnd;

    ParsedAssignValue val{ AssignValueType::FUNC_CALL, FixedSizeVec<std::string>(static_cast<unsigned long>(tokenNum + 1)) };
    val.param[0] = { value.begin(), openParan };

    for (; currentEnd != closeParan; ++currentEnd) {
        if (*currentEnd == ',') {
            std::string token = { start, currentEnd };
            //            std::clog << token << " Line 97\n";
            if (token.empty() || ('A' <= *token.begin() && *token.begin() <= 'Z')) {
                throw InvalidInstruction(line);
            }
            if (std::all_of(token.begin(), token.end(), [](char c) { return '0' <= c && c <= '9'; })) {    // NOLINT
                val.param[currentIndex++] = std::move(token);
            } else if (token.length() >= 2 && *token.begin() == '\'' && *std::prev(token.end()) == '\'') {
                if (std::any_of(std::next(token.begin()), std::prev(token.end()), [](char c) {
                        return (c < '0' || '9' < c) && (c < 'a' || 'z' < c) && (c < 'A' || 'Z' < c) && c != ' ';
                    })) {
                    throw InvalidInstruction(line);
                }
                val.param[currentIndex++] = std::move(token);
            } else if (std::all_of(token.begin(), token.end(), [](char c) {
                           return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || ('0' <= c && c <= '9') || c == '_';
                       })) {
                val.param[currentIndex++] = std::move(token);
            } else {
                throw InvalidInstruction(line);
            }
            ++currentEnd;
            start = currentEnd;
        }
    }

    if (start != closeParan) {
        std::string token = { start, currentEnd };
        //       std::clog << token << " Line 124\n";
        if (token.empty() || ('A' <= *token.begin() && *token.begin() <= 'Z')) {
            throw InvalidInstruction(line);
        }
        if (std::all_of(token.begin(), token.end(), [](char c) { return '0' <= c && c <= '9'; })) {    // NOLINT
            val.param[currentIndex++] = std::move(token);
        } else if (token.length() >= 2 && *token.begin() == '\'' && *std::prev(token.end()) == '\'') {
            if (std::any_of(std::next(token.begin()), std::prev(token.end()), [](char c) {
                    return (c < '0' || '9' < c) && (c < 'a' || 'z' < c) && (c < 'A' || 'Z' < c) && c != ' ';
                })) {
                throw InvalidInstruction(line);
            }
            val.param[currentIndex++] = std::move(token);
        } else if (std::all_of(token.begin(), token.end(), [](char c) {
                       return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || ('0' <= c && c <= '9') || c == '_';
                   })) {
            val.param[currentIndex++] = std::move(token);
        } else {
            throw InvalidInstruction(line);
        }
    }
    return val;
}

TokenizedFunctionDeclaration tokenizeFunctionDeclaration(StrCIter begin, StrCIter end, const std::string &line) {
    // min length: ()->number
    TokenizedFunctionDeclaration decl;

    const long MIN_LENGTH = 10;
    if (end - begin < MIN_LENGTH || *begin != '(') {
        throw InvalidInstruction(line);
    }
    auto closingParan = std::find(begin, end, ')');
    if (closingParan == end) {
        throw InvalidInstruction(line);
    }
    auto dash = std::next(closingParan);
    if (dash == end || *dash != '-') {
        throw InvalidInstruction(line);
    }
    auto arrow = std::next(dash);
    if (arrow == end || *arrow != '>') {
        throw InvalidInstruction(line);
    }

    const char *number_WORD = "number";
    const char *string_WORD = "string";
    const size_t word_LEN = 6;

    if (std::equal(number_WORD, number_WORD + word_LEN, std::next(arrow))) {    // NOLINT
        decl.returnType = Symbol::DataType::NUMBER;
    } else if (std::equal(string_WORD, string_WORD + word_LEN, std::next(arrow))) {    // NOLINT
        decl.returnType = Symbol::DataType::STRING;
    } else {
        throw InvalidInstruction(line);
    }

    if (std::next(begin) == closingParan) {
        return decl;
    }

    auto commaNum = std::count_if(std::next(begin), closingParan, [](char c) { return c == ','; });
    auto tokenNum = commaNum + 1;

    // NOLINTNEXTLINE(hicpp-avoid-c-arrays, modernize-avoid-c-arrays, cppcoreguidelines-avoid-c-arrays)
    decl.paramType = FixedSizeVec<Symbol::DataType>(static_cast<unsigned long>(tokenNum));
    unsigned long currentIndex = 0;
    auto currentEnd = std::next(begin);
    auto start = currentEnd;

    for (; currentEnd != closingParan; ++currentEnd) {
        if (*currentEnd == ',') {
            std::string token = { start, currentEnd };
            if (token == "string") {
                decl.paramType[currentIndex++] = Symbol::DataType::STRING;
            } else if (token == "number") {
                decl.paramType[currentIndex++] = Symbol::DataType::NUMBER;
            } else {
                throw InvalidInstruction(line);
            }
            ++currentEnd;
            start = currentEnd;
        }
    }

    if (start != closingParan) {
        std::string token = { start, currentEnd };
        if (token == "string") {
            decl.paramType[currentIndex] = Symbol::DataType::STRING;
        } else if (token == "number") {
            decl.paramType[currentIndex] = Symbol::DataType::NUMBER;
        } else {
            throw InvalidInstruction(line);
        }
    }
    return decl;
}

TokenizedParam parseInsert(StrCIter begin, StrCIter end, const std::string &line) {
    // a string true
    //  ^      ^
    //  first  |
    //         second
    auto firstSpace = std::find(begin, end, ' ');
    if (firstSpace == end) {
        throw InvalidInstruction(line);
    }

    if (*begin < 'a' || 'z' < *begin) {
        throw InvalidInstruction(line);
    }

    if (std::any_of(begin, firstSpace, [](char c) {
            return (c < '0' || '9' < c) && (c < 'a' || 'z' < c) && (c < 'A' || 'Z' < c) && c != '_';
        })) {
        throw InvalidInstruction(line);
    }

    TokenizedParam param(3);
    param[0] = { begin, firstSpace };

    auto secondSpace = std::find(std::next(firstSpace), end, ' ');
    if (secondSpace == end) {
        throw InvalidInstruction(line);
    }
    param[1] = { std::next(firstSpace), secondSpace };

    const char *true_WORD = "true";
    const char *false_WORD = "false";
    const size_t true_LEN = 4;
    const size_t false_LEN = 5;

    if (!std::equal(true_WORD, true_WORD + true_LEN, std::next(secondSpace)) && !std::equal(false_WORD, false_WORD + false_LEN, std::next(secondSpace))) {    // NOLINT
        throw InvalidInstruction(line);
    }
    param[2] = { std::next(secondSpace), end };
    return param;
}
TokenizedParam parseAssign(StrCIter begin, StrCIter end, const std::string &line) {
    // a foo(12,'h e')
    //  ^
    //  firstspace
    auto firstSpace = std::find(begin, end, ' ');
    if (firstSpace == end) {
        throw InvalidInstruction(line);
    }

    if (*begin < 'a' || 'z' < *begin) {
        throw InvalidInstruction(line);
    }

    if (std::any_of(begin, firstSpace, [](char c) {
            return (c < '0' || '9' < c) && (c < 'a' || 'z' < c) && (c < 'A' || 'Z' < c) && c != '_';
        })) {
        throw InvalidInstruction(line);
    }
    TokenizedParam param(2);
    param[0] = { begin, firstSpace };
    param[1] = { std::next(firstSpace), end };
    return param;
}

MatchResult parseInstruction(const std::string &line) {    // NOLINT(readability-function-cognitive-complexity): Expected complicated function
    // minimum size instruction:
    // END
    if (line.size() < 3) {
        throw InvalidInstruction(line);
    }

    if (*line.begin() == 'I') {    // maybe INSERT, min INSERT instruction: INSERT a string true
        const size_t MIN_INSERT_LEN = 20;
        if (line.size() < MIN_INSERT_LEN) {
            throw InvalidInstruction(line);
        }

        const char *INSERT_WORD = "INSERT";
        const size_t INSERT_LEN = 6;
        if (!std::equal(INSERT_WORD, INSERT_WORD + INSERT_LEN, line.begin())) {    // NOLINT
            throw InvalidInstruction(line);
        }
        auto firstSpace = line.begin() + INSERT_LEN;

        if (*firstSpace != ' ') {
            throw InvalidInstruction(line);
        }

        MatchResult res{ InstructionType::INSERT, parseInsert(std::next(firstSpace), line.end(), line) };
        return res;
    }

    if (*line.begin() == 'A') {    // maybe ASSIGN, min ASSIGN instruction: ASSIGN a b
        const size_t MIN_ASSIGN_LEN = 10;
        if (line.size() < MIN_ASSIGN_LEN) {
            throw InvalidInstruction(line);
        }
        const char *ASSIGN_WORD = "ASSIGN";
        const size_t ASSIGN_LEN = 6;
        if (!std::equal(ASSIGN_WORD, ASSIGN_WORD + ASSIGN_LEN, line.begin())) {    // NOLINT
            throw InvalidInstruction(line);
        }
        auto firstSpace = line.begin() + ASSIGN_LEN;    // NOLINT
        if (*firstSpace != ' ') {
            throw InvalidInstruction(line);
        }
        MatchResult res{ InstructionType::ASSIGN, parseAssign(std::next(firstSpace), line.end(), line) };
        return res;
    }

    if (*line.begin() == 'L') {    // maybe LOOKUP, min LOOKUP instruction: LOOKUP a
        const size_t MIN_LOOKUP_LEN = 8;
        if (line.size() < MIN_LOOKUP_LEN) {
            throw InvalidInstruction(line);
        }
        const char *LOOKUP_WORD = "LOOKUP";
        const size_t LOOKUP_LEN = 6;
        if (!std::equal(LOOKUP_WORD, LOOKUP_WORD + LOOKUP_LEN, line.begin())) {    // NOLINT
            throw InvalidInstruction(line);
        }
        auto firstSpace = line.begin() + LOOKUP_LEN;
        if (*firstSpace != ' ') {
            throw InvalidInstruction(line);
        }
        if (*std::next(firstSpace) < 'a' || 'z' < *std::next(firstSpace)) {
            throw InvalidInstruction(line);
        }
        if (std::any_of(std::next(firstSpace), line.end(), [](char c) {
                return (c < 'a' || 'z' < c) && (c < 'A' || 'Z' < c) && (c < '0' || '9' < c) && c != '_';
            })) {
            throw InvalidInstruction(line);
        }
        MatchResult res{ InstructionType::LOOKUP, TokenizedParam(1) };
        res.params[0] = { std::next(firstSpace), line.end() };
        return res;
    }

    if (line == "BEGIN") {
        return { InstructionType::BEGIN, TokenizedParam() };
    }

    if (line == "END") {
        return { InstructionType::END, TokenizedParam() };
    }

    if (line == "PRINT") {
        return { InstructionType::PRINT, TokenizedParam() };
    }

    throw InvalidInstruction(line);
}

}    // namespace match

SymbolTable::SymbolTable() {
    symbols.addMoreScope();
}

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
#ifdef EXPERIMENTAL_PARSING
    auto instr = match::parseInstruction(line);
    switch (instr.type) {
    case match::InstructionType::INSERT: {
        bool isStatic = instr.params[2] == "true";
        printFlag = true;

        auto result = insert(instr.params[0], instr.params[1], isStatic, line);
        return std::to_string(result.compNum) + ' ' + std::to_string(result.splayNum);
    }
    case match::InstructionType::ASSIGN: {
        const auto &name = instr.params[0];
        const auto &value = instr.params[1];
        printFlag = true;

        auto result = assign(name, value, line);
        return std::to_string(result.compNum) + ' ' + std::to_string(result.splayNum);
    }
    case match::InstructionType::LOOKUP: {
        printFlag = true;
        return std::to_string(lookup(instr.params[0], line));
    }
    case match::InstructionType::BEGIN:
        begin();
        return "";
    case match::InstructionType::END:
        end();
        return "";
    case match::InstructionType::PRINT: {
        auto str = tree.toString(TraversalMethod::PREORDER);
        printFlag = true;
        return str;
    }
    }

#else
    static const std::regex INSERT_REGEX{
        R"(^INSERT ([a-z]\w*) (string|number|\((?:|(?:number|string)(?:,(?:number|string))*)\)->(?:number|string)) (true|false)$)"
    };
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
        printFlag = true;
        return str;
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
#endif
}

SymbolTable::OpResult SymbolTable::assign(const std::string &name, const std::string &value, const std::string &line) {    // NOLINT
    OpResult result;

#ifdef EXPERIMENTAL_PARSING
    auto val = match::parseAssignValue(value, line);
    switch (val.type) {
    case match::AssignValueType::LITERAL_NUMBER: {
        auto typeOfAssignee = resolveType(name, result, line);
        if (typeOfAssignee != Symbol::DataType::NUMBER) {
            throw TypeMismatch(line);
        }
        return result;
    }
    case match::AssignValueType::LITERAL_STRING: {
        auto typeOfAssignee = resolveType(name, result, line);
        if (typeOfAssignee != Symbol::DataType::STRING) {
            throw TypeMismatch(line);
        }
        return result;
    }
    case match::AssignValueType::IDENTIFIER: {
        auto typeOfValue = resolveType(val.param[0], result, line);
        auto typeOfAssignee = resolveType(name, result, line);

        if (typeOfAssignee != typeOfValue) {
            throw TypeMismatch(line);
        }

        return result;
    }
    case match::AssignValueType::FUNC_CALL: {
        auto *functionNode = findSymbolWithoutSplay(val.param[0], &result);
        if (functionNode == nullptr) {
            throw Undeclared(line);
        }
        if (functionNode->data->getSymbolType() != Symbol::SymbolType::FUNCTION) {
            throw TypeMismatch(line);
        }
        result += tree.splay(functionNode);
        const auto *functionNodeReal = static_cast<FunctionSymbol *>(functionNode->data.get());    // NOLINT(cppcoreguidelines-pro-type-static-cast-downcast): functionNode is guranteed to be FunctionSymbol

        FixedSizeVec<Symbol::DataType> paramsType(val.param.size() - 1);

        // resolve type of each param
        for (auto i = 1UL; i < val.param.size(); i++) {
            paramsType[i - 1] =
                resolveType(val.param[i], result, line);
        }

        // then match the type of param to type of function
        if (!functionNodeReal->matchParams(paramsType)) {
            throw TypeMismatch(line);
        }

        auto assigneeType{ resolveType(name, result, line) };

        if (assigneeType != functionNode->data->getDataType()) {
            throw TypeMismatch(line);
        }
        return result;
    }
    }
    throw InvalidInstruction(line);
#else
    auto valueType = resolveValueType(value);
    if (valueType == ValueType::NUMBER) {
        auto typeOfAssignee = resolveType(name, result, line);
        if (typeOfAssignee != Symbol::DataType::NUMBER) {
            throw TypeMismatch(line);
        }
        return result;
    }
    if (valueType == ValueType::STRING) {
        auto typeOfAssignee = resolveType(name, result, line);
        if (typeOfAssignee != Symbol::DataType::STRING) {
            throw TypeMismatch(line);
        }
        return result;
    }
    if (valueType == ValueType::SYMBOL) {
        auto typeOfValue = resolveType(value, result, line);
        auto typeOfAssignee = resolveType(name, result, line);

        if (typeOfAssignee != typeOfValue) {
            throw TypeMismatch(line);
        }

        return result;
    }
    if (valueType == ValueType::FUNCTION_CALL) {
        const auto tokenizedFunctionCall{ tokenizeFunctionCall(value) };

        auto *functionNode =
            findSymbolWithoutSplay(tokenizedFunctionCall.functionName, &result);
        if (functionNode == nullptr) {
            throw Undeclared(line);
        }
        if (functionNode->data->getSymbolType() != Symbol::SymbolType::FUNCTION) {
            throw TypeMismatch(line);
        }
        result += tree.splay(functionNode);

        const auto *functionSymbol =
            static_cast<FunctionSymbol *>(tree.root->data.get());    // NOLINT(cppcoreguidelines-pro-type-static-cast-downcast): functionNode is guranteed to be FunctionSymbol

        // NOLINTNEXTLINE(hicpp-avoid-c-arrays, modernize-avoid-c-arrays, cppcoreguidelines-avoid-c-arrays)
        std::unique_ptr<Symbol::DataType[]> paramsType = std::make_unique<Symbol::DataType[]>(tokenizedFunctionCall.paramsCount);

        // resolve type of each param
        for (auto i = 0UL; i < tokenizedFunctionCall.paramsCount; i++) {
            paramsType[i] =
                resolveType(tokenizedFunctionCall.paramsList[i], result, line);
        }

        // then match the type of param to type of function
        if (!functionSymbol->matchParams(paramsType, tokenizedFunctionCall.paramsCount)) {
            throw TypeMismatch(line);
        }

        auto assigneeType{ resolveType(name, result, line) };

        if (assigneeType != functionNode->data->getDataType()) {
            throw TypeMismatch(line);
        }
        return result;
    }
    throw std::logic_error("Cannot reach here!");
#endif
}
/*
SymbolTable::FunctionCallTokenizeResult
    SymbolTable::tokenizeFunctionCall(const std::string &functionCall) {

    auto startOfFunctionName{ functionCall.begin() };
    auto endOfFunctionName{
        std::find(functionCall.begin(), functionCall.end(), '(')
    };

    auto tokenizedParams{
        tokenizeParams(endOfFunctionName + 1, std::prev(functionCall.end()))
    };
    return { { startOfFunctionName, endOfFunctionName },
        std::move(tokenizedParams.data),
        tokenizedParams.size };
}

SymbolTable::FunctionDeclarationTokenizeResult SymbolTable::tokenizeFunctionDeclaration(const std::string &functionDeclaration) {
    auto firstBracket{ functionDeclaration.begin() };
    auto lastBracket{
        std::find(functionDeclaration.begin(), functionDeclaration.end(), ')')
    };

    auto paramsTokenizeResult{ tokenizeParams(firstBracket + 1, lastBracket) };

    auto arrow = std::find(lastBracket, functionDeclaration.end(), '>');

    return { std::string{ arrow + 1, functionDeclaration.end() }, paramsTokenizeResult };
}
*/

SymbolTable::TokenizeResult SymbolTable::tokenizeParams(std::string::const_iterator start,
    std::string::const_iterator end) {
    if (start == end) {
        return {};
    }

    auto commaNum = std::count_if(start, end, [](char c) { return c == ','; });

    auto tokenNum = commaNum + 1;

    // NOLINTNEXTLINE(hicpp-avoid-c-arrays, modernize-avoid-c-arrays, cppcoreguidelines-avoid-c-arrays)
    std::unique_ptr<std::string[]> tokens = std::make_unique<std::string[]>(static_cast<unsigned long>(tokenNum));

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

SymbolTable::ValueType SymbolTable::resolveValueType(const std::string &value) {
    auto firstChar = *value.begin();
    auto lastChar = *value.rbegin();
    if (firstChar == '\'') {
        return ValueType::STRING;
    }
    if ('0' <= firstChar && firstChar <= '9') {
        return ValueType::NUMBER;
    }
    if (lastChar == ')') {
        return ValueType::FUNCTION_CALL;
    }
    return ValueType::SYMBOL;
}

Symbol::DataType SymbolTable::resolveType(const std::string &value,
    OpResult &result,
    const std::string &line) {
    auto valueType = resolveValueType(value);

    if (valueType == ValueType::NUMBER) {
        return Symbol::DataType::NUMBER;
    }

    if (valueType == ValueType::STRING) {
        return Symbol::DataType::STRING;
    }
    if (valueType == ValueType::SYMBOL) {
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
    throw TypeMismatch(line);
}

void SymbolTable::end() {
    if (currentLevel == 0) {
        throw UnknownBlock();
    }
    for (auto *node = symbols.getHead()->popFront(); node != nullptr; node = symbols.getHead()->popFront()) {
        tree.deleteNode(node);
    }
    symbols.deleteScope();
    currentLevel--;
}

int SymbolTable::lookup(const std::string &name, const std::string &line) {    // NOLINT(bugprone-easily-swappable-parameters)
    auto *node = findSymbolWithoutSplay(name, nullptr);

    if (node == nullptr) {
        throw Undeclared(line);
    }

    tree.splay(node);

    return node->data->getLevel();
}

SymbolTable::Tree::TreeNode *
    SymbolTable::Tree::findSymbolWithoutSplay(const std::string &name, int level, OpResult *result) const noexcept {
    auto *ptr = root;
    for (;;) {
        if (ptr->data->equal(name, level)) {
            if (result != nullptr) {
                result->compNum++;
            }
            return ptr;
        }
        if (ptr->data->greaterThan(name, level)) {
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

SymbolTable::OpResult SymbolTable::insert(const std::string &name, const std::string &value,    // NOLINT(bugprone-easily-swappable-parameters)
    const bool isStatic,
    const std::string &line) {
    using TreeNode = Tree::TreeNode;

    const int targetLevel = isStatic ? 0 : currentLevel;
    OpResult result;
    std::unique_ptr<Symbol> newData;

#ifdef EXPERIMENTAL_PARSING
    if (value == "string") {
        const auto type = Symbol::DataType::STRING;
        newData = std::make_unique<VariableSymbol>(name, targetLevel, type);

    } else if (value == "number") {
        const auto type = Symbol::DataType::NUMBER;
        newData = std::make_unique<VariableSymbol>(name, targetLevel, type);
    } else {
        if (targetLevel != 0) {
            throw InvalidDeclaration(line);
        }
        auto tokenizedDeclaration = match::tokenizeFunctionDeclaration(value.begin(), value.end(), line);
        newData = std::make_unique<FunctionSymbol>(name, targetLevel, tokenizedDeclaration.returnType, std::move(tokenizedDeclaration.paramType));
    }

#else

    if (value == "string" || value == "number") {
        const Symbol::DataType type =
            value == "string" ? Symbol::DataType::STRING : Symbol::DataType::NUMBER;
        newData = std::make_unique<VariableSymbol>(name, targetLevel, type);

    } else {    // if function
        if (targetLevel != 0) {
            throw InvalidDeclaration(line);    // cannot declare a function in level other than 0
        }

        const auto tokenizedFunctionDeclaration = tokenizeFunctionDeclaration(value);

        const auto &paramCount = tokenizedFunctionDeclaration.paramCount;
        const auto &params = tokenizedFunctionDeclaration.params;
        const auto &returnTypeStr = tokenizedFunctionDeclaration.returnType;

        // NOLINTNEXTLINE(hicpp-avoid-c-arrays, modernize-avoid-c-arrays, cppcoreguidelines-avoid-c-arrays)
        std::unique_ptr<Symbol::DataType[]> param = std::make_unique<Symbol::DataType[]>(paramCount);

        for (unsigned long i = 0; i < paramCount; i++) {
            param[i] = params[i] == "string" ? Symbol::DataType::STRING
                                             : Symbol::DataType::NUMBER;
        }

        Symbol::DataType returnType = returnTypeStr == "string"
                                          ? Symbol::DataType::STRING
                                          : Symbol::DataType::NUMBER;

        newData = std::make_unique<FunctionSymbol>(name, targetLevel, returnType, static_cast<int>(paramCount), std::move(param));
    }
#endif

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

    if (targetLevel == 0) {
        symbols.getTail()->pushRear(tree.root);
    } else {
        symbols.getHead()->pushRear(tree.root);
    }

    return result;
}
void SymbolTable::detectUnclosedBlock() const {
    if (currentLevel != 0) {
        throw UnclosedBlock(currentLevel);
    }
}

void SymbolTable::begin() noexcept {
    currentLevel++;
    symbols.addMoreScope();
}

SymbolTable::OpResult SymbolTable::Tree::splay(TreeNode *node) noexcept {
    if (node == root || node == nullptr || root == nullptr) {
        return { 0, 0 };
    }

    while (node != root) {
        if (node->parent == root) {                     // ZIG case
            if (node->parent->isMyLeftChild(node)) {    // ZIG left
                rotateWithLeftChild(node->parent);

            } else if (node->parent->isMyRightChild(node)) {
                rotateWithRightChild(node->parent);    // ZIG right
            }
        } else {
            if (node->parent->isMyLeftChild(node) && node->parent->parent->isMyLeftChild(node->parent)) {    // ZIG ZIG left
                rotateWithLeftChild(node->parent->parent);
                rotateWithLeftChild(node->parent);

            } else if (node->parent->isMyRightChild(node) && node->parent->parent->isMyRightChild(node->parent)) {    // ZIG ZIG right
                rotateWithRightChild(node->parent->parent);
                rotateWithRightChild(node->parent);

            } else if (node->parent->isMyLeftChild(node) && node->parent->parent->isMyRightChild(node->parent)) {    // ZIG ZAG
                rotateWithLeftChild(node->parent);
                rotateWithRightChild(node->parent);

            } else if (node->parent->isMyRightChild(node) && node->parent->parent->isMyLeftChild(node->parent)) {
                rotateWithRightChild(node->parent);
                rotateWithLeftChild(node->parent);
            }
        }
    }

    return { 0, 1 };
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
    auto endIter = output.rbegin();
    if (output.length() >= 2) {
        if (*endIter == ' ') {
            output.erase(std::next(endIter).base());
        }
    }

    return output;
}

void SymbolTable::Tree::preOrderToString(const TreeNode *currentRoot,
    std::string &output) {
    if (currentRoot == nullptr) {
        return;
    }
    output += currentRoot->data->toString();
    output += ' ';
    preOrderToString(currentRoot->leftChild, output);
    preOrderToString(currentRoot->rightChild, output);
}

void SymbolTable::Tree::inOrderToString(const TreeNode *currentRoot,
    std::string &output) {
    if (currentRoot == nullptr) {
        return;
    }
    inOrderToString(currentRoot->leftChild, output);
    output += currentRoot->data->toString();
    output += ' ';
    inOrderToString(currentRoot->rightChild, output);
}

void SymbolTable::Tree::postOrderToString(const TreeNode *currentRoot,
    std::string &output) {
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
    if (node->parent->isMyLeftChild(node)) {
        node->parent->leftChild = oldLeftChild;
        node->parent = oldLeftChild;
        oldLeftChild->parent = parent;
        return;
    }
    if (node->parent->isMyRightChild(node)) {
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
    if (node->parent->isMyLeftChild(node)) {
        node->parent->leftChild = oldRightChild;
        node->parent = oldRightChild;
        oldRightChild->parent = parent;
        return;
    }
    if (node->parent->isMyRightChild(node)) {
        node->parent->rightChild = oldRightChild;
        node->parent = oldRightChild;
        oldRightChild->parent = parent;
        return;
    }
}
void SymbolTable::Tree::deleteNode(TreeNode *node) {
    splay(node);

    Tree leftSubTree;
    Tree rightSubTree;
    leftSubTree.root = node->leftChild;
    rightSubTree.root = node->rightChild;
    node->leftChild = nullptr;
    node->rightChild = nullptr;

    if (leftSubTree.root != nullptr) {
        leftSubTree.root->parent = nullptr;
    }

    if (rightSubTree.root != nullptr) {
        rightSubTree.root->parent = nullptr;
    }
    delete node;
    if (leftSubTree.root == nullptr) {
        root = rightSubTree.root;
        rightSubTree.root = nullptr;
        return;
    }
    // left sub tree is not null
    if (rightSubTree.root != nullptr) {
        auto *maxPtr = leftSubTree.root;
        for (; maxPtr->hasRightChild(); maxPtr = maxPtr->rightChild) {}
        leftSubTree.splay(maxPtr);
    }
    root = leftSubTree.root;
    leftSubTree.root = nullptr;
    if (rightSubTree.root != nullptr) {
        root->rightChild = rightSubTree.root;
        rightSubTree.root = nullptr;
    }
    if (root->hasRightChild()) {
        root->rightChild->parent = root;
    }
}

SymbolTable::Tree::~Tree() { delete root; }

Symbol::Symbol(std::string name, int level, SymbolType symbolType, DataType dataType)
    : name(std::move(name)), level(level), symbolType(symbolType),
      dataType(dataType) {}

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

bool Symbol::equal(const std::string &nameToComp, int levelToComp) const noexcept {
    return level == levelToComp && name == nameToComp;
}

bool Symbol::lessThan(const std::string &nameToComp, int levelToComp) const noexcept {
    if (level == levelToComp) {
        return name < nameToComp;
    }
    return level < levelToComp;
}

bool Symbol::greaterThan(const std::string &nameToComp, int levelToComp) const noexcept {
    if (level == levelToComp) {
        return name > nameToComp;
    }
    return level > levelToComp;
}

std::string Symbol::toString() const {
    return name + "//" + std::to_string(level);
}

VariableSymbol::VariableSymbol(const std::string &name, int level, DataType dataType)
    : Symbol(name, level, SymbolType::VARIABLE, dataType) {}

VariableSymbol::VariableSymbol(const VariableSymbol &other)
    : Symbol(other.getName(), other.getLevel(), SymbolType::VARIABLE, other.getDataType()) {}

// NOLINTNEXTLINE(hicpp-avoid-c-arrays, modernize-avoid-c-arrays, cppcoreguidelines-avoid-c-arrays)
FunctionSymbol::FunctionSymbol(std::string name, int level, DataType returnType, FixedSizeVec<DataType> &&paramsType)
    : Symbol(std::move(name), level, SymbolType::FUNCTION, returnType),
      paramsType(paramsType) {}

// NOLINTNEXTLINE(hicpp-avoid-c-arrays, modernize-avoid-c-arrays, cppcoreguidelines-avoid-c-arrays)
bool FunctionSymbol::matchParams(FixedSizeVec<DataType> paramsToMatch) const {
    return std::equal(paramsToMatch.begin(), paramsToMatch.end(), paramsType.begin(), paramsType.end());
}

SymbolTable::Tree::TreeNode::TreeNode(std::unique_ptr<Symbol> &&data)
    : data(std::move(data)) {}

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

SymbolTable::SymbolList::Scope *SymbolTable::SymbolList::getHead() const noexcept {
    return head;
}
SymbolTable::SymbolList::Scope *SymbolTable::SymbolList::getTail() const noexcept {
    return tail;
}

SymbolTable::SymbolList::Scope::~Scope() {
    auto *symbol = front;
    while (symbol != nullptr) {
        auto *ptr = symbol;
        symbol = symbol->next;
        delete ptr;
    }
}

SymbolTable::Tree::TreeNode *SymbolTable::SymbolList::Scope::popFront() {
    if (front == nullptr) {
        return nullptr;
    }
    auto *oldFront = front;
    auto *oldFrontPtr = front->ptr;

    front = front->next;

    if (front == nullptr) {
        rear = nullptr;
    }
    delete oldFront;
    return oldFrontPtr;
}

void SymbolTable::SymbolList::Scope::pushRear(Tree::TreeNode *node) {
    auto *newNode = new ScopeNode{ node };
    if (rear == nullptr) {
        front = newNode;
        rear = front;
        return;
    }
    rear->next = newNode;
    rear = rear->next;
}

void SymbolTable::SymbolList::addMoreScope() {
    if (tail == nullptr) {
        head = new Scope();
        tail = head;
        return;
    }
    auto *newNode = new Scope();
    newNode->nextScope = head;
    head = newNode;
}

void SymbolTable::SymbolList::deleteScope() {
    if (tail == nullptr) {
        return;
    }
    if (head == tail) {
        delete head;
        tail = nullptr;
        head = nullptr;
        return;
    }
    auto *oldHead = head;
    head = head->nextScope;
    delete oldHead;
}

SymbolTable::SymbolList::~SymbolList() {
    auto *list = head;
    while (list != nullptr) {
        auto *ptr = list;
        list = list->nextScope;
        delete ptr;
    }
}
