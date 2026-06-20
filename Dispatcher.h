#pragma once

#include "User.h"

class Dispatcher : public User {
public:
    Dispatcher(std::string username, std::string password);

    std::vector<std::string> availableCommands() const override;
};
