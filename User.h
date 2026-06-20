#pragma once

#include "Enums.h"

#include <string>
#include <vector>

class User {
public:
    User(std::string username, std::string password, Role role);
    virtual ~User() = default;

    const std::string& getUsername() const;
    bool checkPassword(const std::string& password) const;
    Role getRole() const;
    const std::string& getPassword() const;

    virtual std::string profileInfo() const;
    virtual std::vector<std::string> availableCommands() const = 0;

    virtual double getPassengerBalance() const { return 0.0; }
    virtual bool passengerWithdraw(double amount) { (void)amount; return false; }
    virtual void passengerDeposit(double amount) { (void)amount; }
    virtual void passengerAddFunds(double amount) { (void)amount; }

protected:
    std::string username;
    std::string password;
    Role role;
};
