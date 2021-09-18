#ifndef ERROR_H
#define ERROR_H

#include <exception>
#include <string>

class Undeclared : public std::exception {
private:
    const std::string m_value;

public:
    explicit Undeclared(std::string &&instr) : m_value{ "Undeclared: " + instr } {}

    const char *what() const noexcept override { return m_value.c_str(); }
};

class Redeclared : public std::exception {
private:
    const std::string m_value;

public:
    explicit Redeclared(std::string &&instr) : m_value{ "Redeclared: " + instr } {}

    const char *what() const noexcept override {
        return m_value.c_str();
    }
};

class InvalidDeclaration : public std::exception {
private:
    const std::string m_value;

public:
    explicit InvalidDeclaration(std::string &&instr) : m_value{ "InvalidDeclaration: " + instr } {}

    const char *what() const noexcept override {
        return m_value.c_str();
    }
};

class TypeMismatch : public std::exception {
private:
    const std::string m_value;

public:
    explicit TypeMismatch(std::string &&instr) : m_value{ "TypeMismatch: " + instr } {}

    const char *what() const noexcept override {
        return m_value.c_str();
    }
};

class UnclosedBlock : public std::exception {
private:
    const std::string m_blockLevel;

public:
    explicit UnclosedBlock(int blockLevel) : m_blockLevel("UnclosedBlock: " + std::to_string(blockLevel)) {}
    const char *what() const noexcept override {
        return m_blockLevel.c_str();
    }
};

class UnknownBlock : public std::exception {
public:
    const char *what() const noexcept override {
        return "UnknownBlock";
    }
};
class InvalidInstruction : public std::exception {
private:
    const std::string m_value;

public:
    explicit InvalidInstruction(std::string &&instr) : m_value{ "InvalidInstruction: " + instr } {}
    const char *what() const noexcept override {
        return m_value.c_str();
    }
};
#endif
