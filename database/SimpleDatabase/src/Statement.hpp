#pragma once

#include "InputBuffer.hpp"
#include "Table.hpp"
#include "Row.hpp"
#include "Common.hpp"

class Statement {
public:
    Statement();
    PrepareResult prepare(InputBuffer& input_buffer);
    ExecuteResult execute(Table& table);

private:
    StatementType type;
    Row rowToInsert;

    PrepareResult prepareInsert(InputBuffer& input_buffer);
    ExecuteResult executeInsert(Table& table);
    ExecuteResult executeSelect(Table& table);
};
