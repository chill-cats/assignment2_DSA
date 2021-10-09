#ifndef DSA_EXCEPTION_H
#define DSA_EXCEPTION_H
#include <string>
#include <iostream>

using namespace std;
class InvalidInstruction : public exception {
    string mess;

public:
    explicit InvalidInstruction(const string &instruction) : mess("Invalid: " + instruction) {}
    const char *what() const noexcept override {
        return mess.c_str();
    }
};
class TypeMismatch : public exception {
    string mess;

public:
    explicit TypeMismatch(const string &instruction) : mess("TypeMismatch: " + instruction) {}
    const char *what() const noexcept override {
        return mess.c_str();
    }
};
class Undeclared : public exception {
    string mess;

public:
    explicit Undeclared(const string &instruction) : mess("Undeclared: " + instruction) {}
    const char *what() const noexcept override {
        return mess.c_str();
    }
};
class Redeclared : public exception {
    string mess;

public:
    explicit Redeclared(const string &instruction) : mess("Redeclared: " + instruction) {}
    const char *what() const noexcept override {
        return mess.c_str();
    }
};
class InvalidDeclaration : public exception {
    string mess;

public:
    explicit InvalidDeclaration(const string &instruction) : mess("InvalidDeclaration: " + instruction) {}
    const char *what() const noexcept override {
        return mess.c_str();
    }
};
class UnclosedBlock : public exception {
    string mess;

public:
    explicit UnclosedBlock(int level) : mess("UnclosedBlock: " + to_string(level)) {}
    const char *what() const noexcept override {
        return mess.c_str();
    }
};
class UnknownBlock : public exception {
    string mess;

public:
    UnknownBlock() : mess("UnknownBlock") {}
    const char *what() const noexcept override {
        return mess.c_str();
    }
};
#endif
