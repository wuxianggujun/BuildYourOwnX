#pragma once

#include "Table.hpp"

class Cursor {
public:
	explicit Cursor(Table* table, uint32_t page_num,uint32_t cell_num);
	~Cursor();


	void* value();
	void advance();
	bool isEndOfTable() const;

	void setCellNum(uint32_t cell_num);
	
	uint32_t getPageNum() const;
	uint32_t getCellNum() const;

	friend void leaf_node_insert(Cursor* cursor,uint32_t key,Row* value);

private:
	Table* table;
	uint32_t page_num;
	uint32_t cell_num;
	bool end_of_table;
};