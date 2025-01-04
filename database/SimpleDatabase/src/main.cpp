#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <ostream>
#include <string>
#include "InputBuffer.hpp"

void print_prompt() {
    printf("db > ");
}

enum class ExecuteResult {
    EXECUTE_SUCCESS,
    EXECUTE_TABLE_FULL
};

enum class MetaCommandResult {
    META_COMMAND_SUCCESS,
    META_COMMAND_UNRECOGNIZED_COMMAND
};

enum class PrepareResult {
    PREPARE_SUCCESS,
    PREPARE_NEGATIVE_ID,
    PREPARE_STRING_TOO_LONG,
    PREPARE_SYNTAX_ERROR,
    PREPARE_UNRECOGNIZED_STATEMENT
};

enum class StatementType {
    STATEMENT_INSERT,
    STATEMENT_SELECT
};

#define COLUMN_USERNAME_SIZE 32
#define COLUMN_EMAIL_SIZE 255

class Row {
public:
    uint32_t id;
    char username[COLUMN_USERNAME_SIZE + 1];
    char email[COLUMN_EMAIL_SIZE + 1];

    void print() const {
        printf("(%u, %s, %s)\n", id, username, email);
    }
};


class Statement {
public:
    StatementType type;
    Row row_to_insert;
};

#define size_of_attribute(Struct, Attribute) sizeof(((Struct *)0)->Attribute)

const uint32_t ID_SIZE = size_of_attribute(Row, id);
const uint32_t USERNAME_SIZE = size_of_attribute(Row, username);
const uint32_t EMAIL_SIZE = size_of_attribute(Row, email);
const uint32_t ID_OFFSET = 0;
const uint32_t USERNAME_OFFSET = ID_OFFSET + ID_SIZE;
const uint32_t EMAIL_OFFSET = USERNAME_OFFSET + USERNAME_SIZE;
const uint32_t ROW_SIZE = ID_SIZE + USERNAME_SIZE + EMAIL_SIZE;
const uint32_t PAGE_SIZE = 4096;
#define TABLE_MAX_PAGES 100
const uint32_t ROWS_PER_PAGE = PAGE_SIZE / ROW_SIZE;
const uint32_t TABLE_MAX_ROWS = ROWS_PER_PAGE * TABLE_MAX_PAGES;

class Table {
public:
    Table(): num_rows(0) {
        for (auto &page: pages) {
            page = nullptr;
        }
    }

    ~Table() {
        for (const auto &page: pages) {
            free(page);
        }
    }


    void serialize_row(const Row &source, void *destination) const {
        memcpy(static_cast<char *>(destination) + ID_OFFSET, &(source.id), ID_SIZE);
        memcpy(static_cast<char *>(destination) + USERNAME_OFFSET, &(source.username), USERNAME_SIZE);
        memcpy(static_cast<char *>(destination) + EMAIL_OFFSET, &(source.email), EMAIL_SIZE);
    }

    void deserialize_row(const void *source, Row &destination) const {
        memcpy(&(destination.id), static_cast<const char *>(source) + ID_OFFSET, ID_SIZE);
        memcpy(&(destination.username), static_cast<const char *>(source) + USERNAME_OFFSET, USERNAME_SIZE);
        memcpy(&(destination.email), static_cast<const char *>(source) + EMAIL_OFFSET, EMAIL_SIZE);
    }


    void *row_slot(const uint32_t row_num) {
        uint32_t page_num = row_num / ROWS_PER_PAGE;
        if (pages[page_num] == nullptr) {
            pages[page_num] = malloc(PAGE_SIZE);
        }

        uint32_t row_offset = row_num % ROWS_PER_PAGE;
        uint32_t byte_offset = row_offset * ROW_SIZE;
        return static_cast<char *>(pages[page_num]) + byte_offset;
    }

    uint32_t num_rows;

private:
    void *pages[TABLE_MAX_PAGES];
};


MetaCommandResult do_meta_command(const InputBuffer &input_buffer) {
    if (strcmp(input_buffer.getBuffer(), ".exit") == 0) {
        exit(EXIT_SUCCESS);
    }
    return MetaCommandResult::META_COMMAND_UNRECOGNIZED_COMMAND;
}

PrepareResult prepare_insert(InputBuffer *input_buffer, Statement *statement) {
    statement->type = StatementType::STATEMENT_INSERT;

    char *keyword = strtok(const_cast<char *>(input_buffer->getBuffer()), " ");
    char *id_string = strtok(nullptr, " ");
    char *username = strtok(nullptr, " ");
    char *email = strtok(nullptr, " ");

    if (id_string == nullptr || username == nullptr || email == nullptr) {
        return PrepareResult::PREPARE_SYNTAX_ERROR;
    }

    int id = atoi(id_string);
    if (id < 0) {
        return PrepareResult::PREPARE_NEGATIVE_ID;
    }

    if (strlen(username) > COLUMN_USERNAME_SIZE) {
        return PrepareResult::PREPARE_STRING_TOO_LONG;
    }

    if (strlen(email) > COLUMN_EMAIL_SIZE) {
        return PrepareResult::PREPARE_STRING_TOO_LONG;
    }

    statement->row_to_insert.id = id;
    strcpy(statement->row_to_insert.username, username);
    strcpy(statement->row_to_insert.email, email);
    return PrepareResult::PREPARE_SUCCESS;
}


PrepareResult prepare_statement(const InputBuffer &input_buffer, Statement &statement) {
    if (strncmp(input_buffer.getBuffer(), "insert", 6) == 0) {
        return prepare_insert(const_cast<InputBuffer *>(&input_buffer), &statement);
    }
    if (strncmp(input_buffer.getBuffer(), "select", 6) == 0) {
        statement.type = StatementType::STATEMENT_SELECT;
        return PrepareResult::PREPARE_SUCCESS;
    }
    return PrepareResult::PREPARE_UNRECOGNIZED_STATEMENT;
}

ExecuteResult execute_insert(Statement &statement, Table &table) {
    if (table.num_rows >= TABLE_MAX_ROWS) {
        return ExecuteResult::EXECUTE_TABLE_FULL;
    }

    Row &row_to_insert = statement.row_to_insert;

    table.serialize_row(row_to_insert, table.row_slot(table.num_rows));
    table.num_rows += 1;

    return ExecuteResult::EXECUTE_SUCCESS;
}

ExecuteResult execute_select(Statement &statement, Table &table) {
    Row row{};
    for (uint32_t i = 0; i < table.num_rows; i++) {
        table.deserialize_row(table.row_slot(i), row);
        row.print();
    }
    return ExecuteResult::EXECUTE_SUCCESS;
}

ExecuteResult execute_statement(Statement &statement, Table &table) {
    switch (statement.type) {
        case StatementType::STATEMENT_INSERT:
            return execute_insert(statement, table);
        case StatementType::STATEMENT_SELECT:
            return execute_select(statement, table);
    }
    return ExecuteResult::EXECUTE_SUCCESS;
}

void insert_many_rows(Table &table) {
    for (uint32_t i = 1; i <= 1400; ++i) {
        Row row;
        row.id = i;
        sprintf(row.username, "user%u", i);
        sprintf(row.email, "user%u@example.com", i);

        Statement statement;
        statement.type = StatementType::STATEMENT_INSERT;
        statement.row_to_insert = row;

        ExecuteResult result = execute_insert(statement, table);
        if (result == ExecuteResult::EXECUTE_TABLE_FULL) {
            std::cerr << "Error: Table full." << std::endl;
            return;
        }
    }
    std::cout << "Inserted 1400 rows successfully." << std::endl;
}

[[noreturn]] int main(int argc, char *argv[]) {
    Table table;
    InputBuffer input_buffer;

    /*
    // Call the function to insert 1400 rows
    insert_many_rows(table);
*/

    while (true) {
        print_prompt();
        input_buffer.readInput();

        if (input_buffer.getBuffer()[0] == '.') {
            switch (do_meta_command(input_buffer)) {
                case MetaCommandResult::META_COMMAND_SUCCESS:
                    continue;
                case MetaCommandResult::META_COMMAND_UNRECOGNIZED_COMMAND:
                    printf("Unrecognized command '%s'.\n", input_buffer.getBuffer());
                    continue;
            }
        }

        Statement statement{};
        switch (prepare_statement(input_buffer, statement)) {
            case PrepareResult::PREPARE_SUCCESS:
                break;
            case PrepareResult::PREPARE_NEGATIVE_ID:
                printf("ID must be positive.\n");
                continue;
            case PrepareResult::PREPARE_STRING_TOO_LONG:
                printf("String is too long.\n");
                continue;
            case PrepareResult::PREPARE_SYNTAX_ERROR:
                printf("Syntax error. Could not parse statement.\n");
                continue;
            case PrepareResult::PREPARE_UNRECOGNIZED_STATEMENT:
                printf("Unrecognized keyword at start of '%s'.\n", input_buffer.getBuffer());
                continue;
        }
        switch (execute_statement(statement, table)) {
            case ExecuteResult::EXECUTE_SUCCESS:
                printf("Executed.\n");
                break;
            case ExecuteResult::EXECUTE_TABLE_FULL:
                printf("Error: Table fail.\n");
                break;
        }
    }
}
