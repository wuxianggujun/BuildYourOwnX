#pragma once

const uint32_t PAGE_SIZE = 4096;
const uint32_t TABLE_MAX_PAGES = 400;
const uint32_t COLUMN_USERNAME_SIZE = 32;
const uint32_t COLUMN_EMAIL_SIZE = 255;
const uint32_t INVALID_PAGE_NUM = UINT32_MAX;

enum class ExecuteResult {
    SUCCESS,
    DUPLICATE_KEY,
};

enum class MetaCommandResult {
    SUCCESS,
    UNRECOGNIZED_COMMAND,
};

enum class PrepareResult {
    SUCCESS,
    NEGATIVE_ID,
    STRING_TOO_LONG,
    SYNTAX_ERROR,
    UNRECOGNIZED_COMMAND,
};

enum class StatementType {
    INSERT,
    SELECT,
};

enum class NodeType {
    INTERNAL,
    LEAF,
};
