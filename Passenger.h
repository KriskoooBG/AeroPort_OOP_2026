#pragma once

#include "User.h"

class Passenger : public User {
public:
    Passenger(std::string username, std::string password, double balance = 0.0);

    double getBalance() const;
    void addFunds(double amount);
    bool withdraw(double amount);
    void deposit(double amount);

    std::string profileInfo() const override;
    std::vector<std::string> availableCommands() const override;

    double getPassengerBalance() const override;
    bool passengerWithdraw(double amount) override;
    void passengerDeposit(double amount) override;
    void passengerAddFunds(double amount) override;

private:
    double balance;
};
