#include <catch2/catch.hpp>
#include <memory>
#include <algorithm>
#include <array>
#include <cstring>
#include <functional>
#include <random>
#include <string>

#include "SymbolTable.h"

/*
unsigned int Factorial(unsigned int number)// NOLINT(misc-no-recursion)
{
  return number <= 1 ? number : Factorial(number - 1) * number;
}

TEST_CASE("Factorials are computed", "[factorial]")
{
  REQUIRE(Factorial(1) == 1);
  REQUIRE(Factorial(2) == 2);
  REQUIRE(Factorial(3) == 6);
  REQUIRE(Factorial(10) == 3628800);
}
*/
/*
TEST_CASE("Test param matching function match true", "[FunctionSymbol::paramMatching]") {
    StaticVector<Symbol::DataType> vec(100);    // NOLINT
    bool isNumber = false;
    for (auto a = 0UL; a < 100; a++) {                                              // NOLINT
        vec[a] = isNumber ? Symbol::DataType::NUMBER : Symbol::DataType::STRING;    // NOLINT
        isNumber = !isNumber;
    }

    std::unique_ptr<FunctionSymbol> symbolA = std::make_unique<FunctionSymbol>("a", 0, Symbol::DataType::NUMBER, std::move(vec));

    StaticVector<Symbol::DataType> vec2(100);    // NOLINT
    isNumber = false;
    for (auto a = 0UL; a < 100; a++) {                                               // NOLINT
        vec2[a] = isNumber ? Symbol::DataType::NUMBER : Symbol::DataType::STRING;    // NOLINT
        isNumber = !isNumber;
    }
    REQUIRE(symbolA->matchParams(vec2) == true);
}

TEST_CASE("Test param matching function match true, size = 1", "[FunctionSymbol::paramMatching]") {
    StaticVector<Symbol::DataType> vec(1);
    vec[0UL] = Symbol::DataType::NUMBER;

    std::unique_ptr<FunctionSymbol> symbolA = std::make_unique<FunctionSymbol>("a", 0, Symbol::DataType::NUMBER, std::move(vec));

    StaticVector<Symbol::DataType> vec2(1);
    vec2[0UL] = Symbol::DataType::NUMBER;

    REQUIRE(symbolA->matchParams(vec2) == true);
}

TEST_CASE("Test param matching function match false with unmatch symbol, size = 1", "[FunctionSymbol::paramMatching]") {
    StaticVector<Symbol::DataType> vec(1);
    vec[0UL] = Symbol::DataType::NUMBER;

    std::unique_ptr<FunctionSymbol> symbolA = std::make_unique<FunctionSymbol>("a", 0, Symbol::DataType::NUMBER, std::move(vec));

    StaticVector<Symbol::DataType> vec2(1);
    vec2[0UL] = Symbol::DataType::STRING;

    REQUIRE(symbolA->matchParams(vec2) == false);
}

TEST_CASE("Test param matching function match false with unmatch symbol", "[FunctionSymbol::paramMatching]") {
    StaticVector<Symbol::DataType> vec(100);    // NOLINT
    bool isNumber = false;
    for (auto a = 0UL; a < 100; a++) {                                              // NOLINT
        vec[a] = isNumber ? Symbol::DataType::NUMBER : Symbol::DataType::STRING;    // NOLINT
        isNumber = !isNumber;
    }

    std::unique_ptr<FunctionSymbol> symbolA = std::make_unique<FunctionSymbol>("a", 0, Symbol::DataType::NUMBER, std::move(vec));

    StaticVector<Symbol::DataType> vec2(100);    // NOLINT
    isNumber = true;
    for (auto a = 0UL; a < 100; a++) {                                               // NOLINT
        vec2[a] = isNumber ? Symbol::DataType::NUMBER : Symbol::DataType::STRING;    // NOLINT
        isNumber = !isNumber;
    }
    REQUIRE(symbolA->matchParams(vec2) == false);
}

TEST_CASE("Test param matching function match false unmatch size", "[FunctionSymbol::paramMatching]") {
    StaticVector<Symbol::DataType> vec(182);    // NOLINT
    bool isNumber = false;
    for (auto a = 0UL; a < vec.size(); a++) {                                       // NOLINT
        vec[a] = isNumber ? Symbol::DataType::NUMBER : Symbol::DataType::STRING;    // NOLINT
        isNumber = !isNumber;
    }

    std::unique_ptr<FunctionSymbol> symbolA = std::make_unique<FunctionSymbol>("a", 0, Symbol::DataType::NUMBER, std::move(vec));

    StaticVector<Symbol::DataType> vec2(100);    // NOLINT
    isNumber = true;
    for (auto a = 0UL; a < vec2.size(); a++) {                                       // NOLINT
        vec2[a] = isNumber ? Symbol::DataType::NUMBER : Symbol::DataType::STRING;    // NOLINT
        isNumber = !isNumber;
    }
    REQUIRE(symbolA->matchParams(vec2) == false);
}

TEST_CASE("Test param matching function match false zero size and non zero size", "[FunctionSymbol::paramMatching]") {
    StaticVector<Symbol::DataType> vec(0);

    std::unique_ptr<FunctionSymbol> symbolA = std::make_unique<FunctionSymbol>("a", 0, Symbol::DataType::NUMBER, std::move(vec));

    StaticVector<Symbol::DataType> vec2(100);    // NOLINT
    bool isNumber = true;
    for (auto a = 0UL; a < vec2.size(); a++) {                                       // NOLINT
        vec2[a] = isNumber ? Symbol::DataType::NUMBER : Symbol::DataType::STRING;    // NOLINT
        isNumber = !isNumber;
    }
    REQUIRE(symbolA->matchParams(vec2) == false);
}

TEST_CASE("Test param matching function match true two zero size", "[FunctionSymbol::paramMatching]") {
    StaticVector<Symbol::DataType> vec(0);

    std::unique_ptr<FunctionSymbol> symbolA = std::make_unique<FunctionSymbol>("a", 0, Symbol::DataType::NUMBER, std::move(vec));

    StaticVector<Symbol::DataType> vec2(0);
    REQUIRE(symbolA->matchParams(vec2) == true);
}
*/
