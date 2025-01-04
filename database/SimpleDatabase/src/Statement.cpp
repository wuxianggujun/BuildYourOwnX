#include "Statement.hpp"
#include <cstring>
#include <iostream>

Statement::Statement(): type(StatementType::STATEMENT_SELECT), rowToInsert() {
}

PrepareResult Statement::prepare(InputBuffer &input_buffer) {
    if (strncmp(input_buffer.getBuffer(), "insert", 6) == 0) {
        return prepareInsert(input_buffer);
    }
    if (strncmp(input_buffer.getBuffer(), "select", 6) == 0) {
        type = StatementType::STATEMENT_SELECT;
        return PrepareResult::PREPARE_SUCCESS;
    }
    return PrepareResult::PREPARE_UNRECOGNIZED_STATEMENT;
}

ExecuteResult Statement::execute(Table &table) {
    switch (type) {
        case StatementType::STATEMENT_INSERT:
            return executeInsert(table);
        case StatementType::STATEMENT_SELECT:
            return executeSelect(table);
    }
    return ExecuteResult::EXECUTE_SUCCESS;
}

PrepareResult Statement::prepareInsert(InputBuffer &input_buffer) {
    type = StatementType::STATEMENT_INSERT;

    char *keyword = strtok(const_cast<char *>(input_buffer.getBuffer()), " ");
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

    rowToInsert.id = id;
    strcpy(rowToInsert.username, username);
    strcpy(rowToInsert.email, email);
    return PrepareResult::PREPARE_SUCCESS;
}

ExecuteResult Statement::executeInsert(Table &table) {
    if (table.isFull()) {
        return ExecuteResult::EXECUTE_TABLE_FULL;
    }
    table.insert(rowToInsert);
    return ExecuteResult::EXECUTE_SUCCESS;
}

ExecuteResult Statement::executeSelect(Table &table) {
    table.select();
    return ExecuteResult::EXECUTE_SUCCESS;
}


