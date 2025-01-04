#pragma once

#include "InputBuffer.hpp"
#include "Table.hpp"
#include "Common.hpp"

class MetaCommand {
public:
    static MetaCommandResult execute(InputBuffer& input_buffer,Table& table);
};
