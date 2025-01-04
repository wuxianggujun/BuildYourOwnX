#include "Compiler.hpp"
#include <cstring>

CompileResult Compiler::compile(const InputBuffer& inputBuffer, Statement& statement) {
    if (strncmp(inputBuffer.getBuffer(), "insert", 6) == 0) {
        statement.type = StatementType::STATEMENT_INSERT;
        int args_assigned = sscanf(
                inputBuffer.getBuffer(), "insert %u %s %s",
                &(statement.row_to_insert.id),
                statement.row_to_insert.username,
                statement.row_to_insert.email
        );
        if (args_assigned < 3) {
            return CompileResult::COMPILE_ERROR;
        }
        return CompileResult::COMPILE_SUCCESS;
    } else if (strcmp(inputBuffer.getBuffer(), "select") == 0) {
        statement.type = StatementType::STATEMENT_SELECT;
        return CompileResult::COMPILE_SUCCESS;
    } else {
        return CompileResult::COMPILE_ERROR;
    }
}