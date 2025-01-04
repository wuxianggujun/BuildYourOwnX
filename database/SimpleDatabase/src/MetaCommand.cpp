#include "MetaCommand.hpp"
#include <cstdlib>
#include <iostream>
#include <cstring>
#include "BTree.hpp"
#include "Cursor.hpp"

MetaCommandResult MetaCommand::execute(InputBuffer &input_buffer, Table &table) {
    if (strcmp(input_buffer.getBuffer(), ".exit") == 0) {
        table.close();
        exit(EXIT_SUCCESS);
    }
    if (strcmp(input_buffer.getBuffer(), ".btree") == 0) {
        std::cout << "Tree:" << std::endl;
        ::print_leaf_node(table.getPager().getPage(0));
        return MetaCommandResult::META_COMMAND_SUCCESS;
    }
    if (strcmp(input_buffer.getBuffer(), ".constants") == 0) {
        std::cout << "Constants:" << std::endl;
        ::print_constants();
        return MetaCommandResult::META_COMMAND_SUCCESS;
    }
    return MetaCommandResult::META_COMMAND_UNRECOGNIZED_COMMAND;
}