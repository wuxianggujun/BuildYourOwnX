#include "MetaCommand.hpp"
#include <cstdlib>
#include <cstring>

MetaCommandResult MetaCommand::execute(InputBuffer &input_buffer, Table &table) {
    if (strcmp(input_buffer.getBuffer(), ".exit") == 0) {
        table.close();
        exit(EXIT_SUCCESS);
    }
    return MetaCommandResult::META_COMMAND_UNRECOGNIZED_COMMAND;
}
