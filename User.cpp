#include "User.h"

#include <utility>

User::User(std::string username, std::string password, Role role)
    : username(std::move(username)), password(std::move(password)), role(role) {}

const std::string& User::getUsername() const {
    return username;
}

const std::string& User::getPassword() const {
    return password;
}

bool User::checkPassword(const std::string& password) const {
    return this->password == password;
}

Role User::getRole() const {
    return role;
}

std::string User::profileInfo() const {
    return "User: " + username + " | Role: " + toString(role);
}
