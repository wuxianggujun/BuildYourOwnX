#include "Database.hpp"
#include "Statement.hpp"
#include "MetaCommand.hpp"
#include <iostream>

Database::Database(const std::string &filename): table(filename) {
}

Database::~Database() {
}

void print_prompt() {
    printf("db > ");
}

void Database::run() {
    InputBuffer input_buffer;
    while (true) {
        print_prompt();
        input_buffer.readInput();
        processInput(input_buffer);
    }
}

void Database::processInput(InputBuffer &input_buffer) {
    if (input_buffer.getBuffer()[0] == '.') {
        const MetaCommandResult result = MetaCommand::execute(input_buffer, table);
        if (result == MetaCommandResult::META_COMMAND_SUCCESS) {
            return;
        }
        if (result == MetaCommandResult::META_COMMAND_UNRECOGNIZED_COMMAND) {
            printf("Unrecognized command '%s'.\n", input_buffer.getBuffer());
            return;
        }
    }

    Statement statement{};
    switch (statement.prepare(input_buffer)) {
        case PrepareResult::PREPARE_SUCCESS:
            break;
        case PrepareResult::PREPARE_NEGATIVE_ID:
            printf("ID must be positive.\n");
            return;
        case PrepareResult::PREPARE_STRING_TOO_LONG:
            printf("String is too long.\n");
            return;
        case PrepareResult::PREPARE_SYNTAX_ERROR:
            printf("Syntax error. Could not parse statement.\n");
            return;
        case PrepareResult::PREPARE_UNRECOGNIZED_STATEMENT:
            printf("Unrecognized keyword at start of '%s'.\n", input_buffer.getBuffer());
            return;
    }
    switch (statement.execute(table)) {
        case ExecuteResult::EXECUTE_SUCCESS:
            printf("Executed.\n");
            break;
        case ExecuteResult::EXECUTE_TABLE_FULL:
            printf("Error: Table fail.\n");
            break;
    }
}
