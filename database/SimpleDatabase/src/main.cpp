#include <iostream>
#include <sstream>

#include "Common.hpp"
#include "Cursor.hpp"
#include "InputBuffer.hpp"
#include "InternalNode.hpp"
#include "LeafNode.hpp"
#include "Node.hpp"
#include "Pager.hpp"
#include "Row.hpp"
#include "Statement.hpp"
#include "Table.hpp"

// Helper functions
void print_row(const Row& row) {
  std::cout << "(" << row.getId() << ", " << row.getUserName() << ", " << row.getEmail() << ")" << std::endl;
}

void print_constants() {
  std::cout << "ROW_SIZE: " << sizeof(uint32_t) + COLUMN_USERNAME_SIZE + 1 + COLUMN_EMAIL_SIZE + 1<< std::endl;
  std::cout << "COMMON_NODE_HEADER_SIZE: " << sizeof(uint8_t) + sizeof(bool) + sizeof(uint32_t) << std::endl;
  std::cout << "LEAF_NODE_HEADER_SIZE: " << sizeof(uint8_t) + sizeof(bool) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t) << std::endl;
  std::cout << "LEAF_NODE_CELL_SIZE: " << sizeof(uint32_t) + sizeof(uint32_t) + COLUMN_USERNAME_SIZE + 1 + COLUMN_EMAIL_SIZE + 1 << std::endl;
  std::cout << "LEAF_NODE_SPACE_FOR_CELLS: " << PAGE_SIZE - (sizeof(uint8_t) + sizeof(bool) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t)) << std::endl;
  std::cout << "LEAF_NODE_MAX_CELLS: " << (PAGE_SIZE - (sizeof(uint8_t) + sizeof(bool) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t))) / (sizeof(uint32_t) + sizeof(uint32_t) + COLUMN_USERNAME_SIZE + 1 + COLUMN_EMAIL_SIZE + 1) << std::endl;
}

void indent(uint32_t level) {
  for (uint32_t i = 0; i < level; i++) {
    std::cout << "  ";
  }
}

void print_tree(Pager* pager, uint32_t page_num, uint32_t indentation_level) {
    Node* node = pager->getPage(page_num);

    if (node->getHeader()->getType() == NodeType::LEAF) {
        auto* leaf_node = dynamic_cast<LeafNode*>(node);
        uint32_t num_keys = leaf_node->getHeader()->getNumCells();
        indent(indentation_level);
        std::cout << "- leaf (size " << num_keys << ")" << std::endl;
        for (uint32_t i = 0; i < num_keys; i++) {
            indent(indentation_level + 1);
            std::cout << "- " << leaf_node->getKey(i) << std::endl;
        }
    } else {
        InternalNode* internal_node = dynamic_cast<InternalNode*>(node);
        uint32_t num_keys = internal_node->getHeader()->getNumKeys();
        indent(indentation_level);
        std::cout << "- internal (size " << num_keys << ")" << std::endl;
        for (uint32_t i = 0; i < num_keys; i++) {
            uint32_t child_page_num = internal_node->getChildPageNum(pager, i);
            print_tree(pager, child_page_num, indentation_level + 1);

            indent(indentation_level + 1);
            std::cout << "- key " << internal_node->getKey(i) << std::endl;
        }
        uint32_t right_child_page_num = internal_node->getHeader()->getRightChildPageNum();
        print_tree(pager, right_child_page_num, indentation_level + 1);
    }
}

// Meta commands
MetaCommandResult do_meta_command(const std::string& command, Table* table) {
  if (command == ".exit") {
    table->close();
    exit(EXIT_SUCCESS);
  } else if (command == ".btree") {
      std::cout << "Tree:" << std::endl;
      print_tree(table->getPager(), 0, 0);
      return MetaCommandResult::SUCCESS;
  } else if (command == ".constants") {
      std::cout << "Constants:" << std::endl;
      print_constants();
      return MetaCommandResult::SUCCESS;
  } else {
    return MetaCommandResult::UNRECOGNIZED_COMMAND;
  }
}

// Prepare statement
PrepareResult prepare_insert(const std::string& buffer, Statement& statement) {
  statement.type = StatementType::INSERT;

  std::istringstream iss(buffer);
  std::string keyword, id_string, username, email;
  iss >> keyword >> id_string >> username >> email;

  if (id_string.empty() || username.empty() || email.empty()) {
    return PrepareResult::SYNTAX_ERROR;
  }

  try {
    int id = std::stoi(id_string);
    if (id < 0) {
      return PrepareResult::NEGATIVE_ID;
    }
    statement.rowToInsert.setId(id);
    statement.rowToInsert.setUsername(username);
    statement.rowToInsert.setEmail(email);
  } catch (const std::invalid_argument& e) {
    return PrepareResult::SYNTAX_ERROR;
  } catch (const std::out_of_range& e) {
    return PrepareResult::SYNTAX_ERROR;
  }

  return PrepareResult::SUCCESS;
}

PrepareResult prepare_statement(const std::string& buffer, Statement& statement) {
  if (buffer.rfind("insert", 0) == 0) {
    return prepare_insert(buffer, statement);
  } else if (buffer == "select") {
    statement.type = StatementType::SELECT;
    return PrepareResult::SUCCESS;
  }

  return PrepareResult::UNRECOGNIZED_COMMAND;
}

// Execute statement
ExecuteResult execute_insert(Statement& statement, Table* table) {
  Row* row_to_insert = &statement.rowToInsert;
  Cursor* cursor = new Cursor(table, row_to_insert->getId());

  Node* node = table->getPager()->getPage(cursor->getPageNUm());
  
  if (node->getHeader()->getType() == NodeType::LEAF) {
      LeafNode* leaf_node = dynamic_cast<LeafNode*>(node);
      uint32_t num_cells = leaf_node->getHeader()->getNumCells();
      
      if (cursor->getCellNum() < num_cells) {
        uint32_t key_at_index = leaf_node->getKey(cursor->getCellNum());
        if (key_at_index == row_to_insert->getId()) {
          delete cursor;
          return ExecuteResult::DUPLICATE_KEY;
        }
      }
  
      if (num_cells >= LeafNode::LEAF_NODE_MAX_CELLS) {
          table->getPager()->leafNodeSplitAndInsert(cursor, row_to_insert->getId(), row_to_insert);
      } else {
          leaf_node->insertAt(cursor->getCellNum(), row_to_insert->getId(), *row_to_insert);
      }
  }
  
  delete cursor;
  return ExecuteResult::SUCCESS;
}

ExecuteResult execute_select(Statement& statement, Table* table) {
  Cursor* cursor = table->tableStart();

  while (!cursor->isEndOfTable()) {
    Row* row = cursor->getValue();
    if (row != nullptr)
    {
      print_row(*row);
      delete row;
    }
    cursor->advance();
  }

  delete cursor;
  return ExecuteResult::SUCCESS;
}

ExecuteResult execute_statement(Statement& statement, Table* table) {
  switch (statement.type) {
    case StatementType::INSERT:
      return execute_insert(statement, table);
    case StatementType::SELECT:
      return execute_select(statement, table);
  }
  return ExecuteResult::SUCCESS; 
}


int main(int argc, char *argv[]) {
    /*
    if (argc < 2) {
        std::cerr << "Must supply a database filename." << std::endl;
        exit(EXIT_FAILURE);
    }
    */

    // std::string filename = argv[1];
    std::string filename = "Default.db";
    Table table(filename);

    InputBuffer inputBuffer;
    while (true) {
        std::cout << "db > ";
        inputBuffer.readInput();
        std::string buffer = inputBuffer.getBuffer();

        if (buffer[0] == '.') {
            switch (do_meta_command(buffer, &table)) {
                case MetaCommandResult::SUCCESS:
                    continue;
                case MetaCommandResult::UNRECOGNIZED_COMMAND:
                    std::cout << "Unrecognized command '" << buffer << "'" << std::endl;
                    continue;
            }
        }

        Statement statement(StatementType::INSERT);

        switch (prepare_statement(buffer, statement)) {
            case PrepareResult::SUCCESS:
                break;
            case PrepareResult::NEGATIVE_ID:
                std::cout << "ID must be positive." << std::endl;
                continue;
            case PrepareResult::STRING_TOO_LONG:
                std::cout << "String is too long." << std::endl;
                continue;
            case PrepareResult::SYNTAX_ERROR:
                std::cout << "Syntax error. Could not parse statement." << std::endl;
                continue;
            case PrepareResult::UNRECOGNIZED_COMMAND:
                std::cout << "Unrecognized keyword at start of '" << buffer << "'." << std::endl;
                continue;
        }
        switch (execute_statement(statement, &table)) {
            case ExecuteResult::SUCCESS:
                std::cout << "Executed." << std::endl;
                break;
            case ExecuteResult::DUPLICATE_KEY:
                std::cout << "Error: Duplicate key." << std::endl;
                break;
        }
    }
    return 0;
}
