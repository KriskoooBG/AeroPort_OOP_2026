#include "Dispatcher.h"

#include <utility>

Dispatcher::Dispatcher(std::string username, std::string password)
    : User(std::move(username), std::move(password), Role::Dispatcher) {}

std::vector<std::string> Dispatcher::availableCommands() const {
    return {
        "list-airspace", "list-runways", "assign-runway", "delay-flight",
        "free-runway", "undo", "view-profile", "logout", "help"
    };
}
