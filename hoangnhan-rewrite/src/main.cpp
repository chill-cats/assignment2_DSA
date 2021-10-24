#include "SymbolTable.h"
#include <chrono>
using namespace std;

void test(const string &filename) {
    auto *st = new SymbolTable();
    auto start = std::chrono::high_resolution_clock::now();
    try {
        st->run(filename);
    } catch (exception &e) {
        cout << e.what();
    }
    delete st;
    auto end = std::chrono::high_resolution_clock::now();
    std::cout.flush();
    std::clog << "\n============\nTime: ";
    std::clog << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << "µs\n";
}

int main(int argc, char **argv) {
    if (argc < 2) {
        return 1;
    }
    test(argv[1]);    // NOLINT(*-avoid-c-arrays, cppcoreguidelines-pro-bounds-pointer-arithmetic)

    return 0;
}
