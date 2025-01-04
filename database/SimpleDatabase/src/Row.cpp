#include "Row.hpp"

#define size_of_attribute(Struct, Attribute) sizeof(((Struct *)0)->Attribute)

// Define the constants that were declared as extern in Common.hpp
const uint32_t ID_SIZE = size_of_attribute(Row, id);
const uint32_t USERNAME_SIZE = size_of_attribute(Row, username);
const uint32_t EMAIL_SIZE = size_of_attribute(Row, email);
const uint32_t ID_OFFSET = 0;
const uint32_t USERNAME_OFFSET = ID_OFFSET + ID_SIZE;
const uint32_t EMAIL_OFFSET = USERNAME_OFFSET + USERNAME_SIZE;
const uint32_t ROW_SIZE = ID_SIZE + USERNAME_SIZE + EMAIL_SIZE;

// Definitions for size and offset calculation functions
uint32_t calculateIdSize() { return ID_SIZE; }
uint32_t calculateUsernameSize() { return USERNAME_SIZE; }
uint32_t calculateEmailSize() { return EMAIL_SIZE; }
uint32_t calculateIdOffset() { return ID_OFFSET; }
uint32_t calculateUsernameOffset() { return USERNAME_OFFSET; }
uint32_t calculateEmailOffset() { return EMAIL_OFFSET; }
uint32_t calculateRowSize() { return ROW_SIZE; }


void Row::print() const {
    printf("(%u, %s, %s)\n", id, username, email);
}


void Row::serialize(void *destination) const {
    memcpy(static_cast<char *>(destination) + ID_OFFSET, &id, ID_SIZE);
    memcpy(static_cast<char *>(destination) + USERNAME_OFFSET, username, USERNAME_SIZE);
    memcpy(static_cast<char *>(destination) + EMAIL_OFFSET, email, EMAIL_SIZE);
}

void Row::deserialize(void *source){
    memcpy(&id, static_cast<char *>(source) + ID_OFFSET, ID_SIZE);
    memcpy(&username, static_cast<char *>(source) + USERNAME_OFFSET, USERNAME_SIZE);
    memcpy(&email, static_cast<char *>(source) + EMAIL_OFFSET, EMAIL_SIZE);
}