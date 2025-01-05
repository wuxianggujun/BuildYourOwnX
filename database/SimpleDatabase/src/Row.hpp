#pragma once

class Row {
public:
    Row(): id(0) {
    }

    explicit Row(uint32_t id, const std::string &username, const std::string &email): id(id), username(username),
        email(email) {
        if (username.length() > COLUMN_USERNAME_SIZE) {
            throw std::runtime_error("Username too long.");
        }
        if (email.length() > COLUMN_EMAIL_SIZE) {
            throw std::runtime_error("Email too long.");
        }
    }

    std::string getUserName() const {
        return username;
    }
    std::string getEmail() const {
        return email;
    }
    uint32_t getId() const {
        return id;
    }

    void setId(uint32_t id) {
        this->id = id;
    }

    void setUsername(std::string username) {
        this->username = username;
    }

    void setEmail(std::string email) {
        this->email = email;
    }
private:
    uint32_t id;
    std::string username;
    std::string email;
};
