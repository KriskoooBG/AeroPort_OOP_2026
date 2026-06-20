#include "Administrator.h"

Administrator::Administrator()
    : User("admin", "admin", Role::Administrator) {}

Administrator::Administrator(std::string username, std::string password)
    : User(std::move(username), std::move(password), Role::Administrator) {}

std::string Administrator::profileInfo() const {
    return "User: admin | Role: System Administrator | Privilege: ALL_ACCESS";
}

std::vector<std::string> Administrator::availableCommands() const {
    return {
        "build-runway", "build-hangar", "close-runway", "set-weather",
        "register-airline", "buy-aircraft", "clone-aircraft", "send-to-hangar",
        "schedule-flight", "retrieve-from-hangar", "cancel-flight",
        "flight-revenue", "list-fleet", "airport-report", "audit-airline",
        "view-profile", "logout", "help", "save", "load"
    };
}
