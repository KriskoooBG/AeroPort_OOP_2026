#pragma once

#include "User.h"

class Administrator : public User {
public:
    Administrator();
    Administrator(std::string username, std::string password);

    std::string profileInfo() const override;
    std::vector<std::string> availableCommands() const override;
};
