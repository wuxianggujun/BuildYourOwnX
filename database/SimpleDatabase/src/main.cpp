#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <ostream>
#include <string>

#include "Database.hpp"

[[noreturn]] int main(int argc, char *argv[]) {
    std::string filename = "default.db"; // 默认文件名
    if (argc >= 2) {
        filename = argv[1];
    }
    Database database(filename);
    database.run();
}
