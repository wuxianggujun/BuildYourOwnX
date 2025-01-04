#pragma once

#include "Table.hpp"
#include "InputBuffer.hpp"

class Database {
public:
   explicit  Database(const std::string &filename);
   ~Database();

   void run();

private:
   void processInput(InputBuffer& input_buffer);
   Table table;
};