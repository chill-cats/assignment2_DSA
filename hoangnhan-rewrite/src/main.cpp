#include "SymbolTable.h"
using namespace std;

void test(const string &filename) {
    auto *st = new SymbolTable();
    try {
        st->run(filename);
    } catch (exception &e) {
        cout << e.what();
    }
    delete st;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        return 1;
    }
    test(argv[1]);//NOLINT

    return 0;
}
