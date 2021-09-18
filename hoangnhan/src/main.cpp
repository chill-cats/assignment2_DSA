#include "SymbolTable.h"

void test(const char *filename) {
    auto *table = new SymbolTable();
    try {
        table->run(filename);
    } catch (std::exception &e) {
        std::cout << e.what();
    }
    delete table;
}

int main(const int argc, const char **argv) {
    if (argc < 2) {
        return 1;
    }
    test(argv[1]);
    return 0;
}
