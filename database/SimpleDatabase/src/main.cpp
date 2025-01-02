#include <cstdio>
#include <cstdlib>
#include <string>
#include "InputBuffer.hpp"

void print_prompt() {
    printf("db > ");
}

int main(int argc, char *argv[]) {
    auto *inputBuffer = new InputBuffer();
    while (true) {
        print_prompt();
        inputBuffer->readInput();

        if (strcmp(inputBuffer->getBuffer(), ".exit") == 0) {
            delete inputBuffer;
            exit(EXIT_SUCCESS);
        }
        printf("Unrecognized command '%s'.\n", inputBuffer->getBuffer());
    }
}
