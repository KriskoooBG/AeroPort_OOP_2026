#include "Passenger.h"

#include <iomanip>
#include <sstream>
#include <utility>

Passenger::Passenger(std::string username, std::string password, double balance)
    : User(std::move(username), std::move(password), Role::Passenger), balance(balance) {}

double Passenger::getBalance() const {
    return balance;
}

void Passenger::addFunds(double amount) {
    balance += amount;
}

bool Passenger::withdraw(double amount) {
    if (amount > balance) {
        return false;
    }
    balance -= amount;
    return true;
}

void Passenger::deposit(double amount) {
    balance += amount;
}

std::string Passenger::profileInfo() const {
    std::ostringstream out;
    out << "User: " << username << " | Role: Passenger | Balance: "
        << std::fixed << std::setprecision(2) << balance << " EUR";
    return out.str();
}

std::vector<std::string> Passenger::availableCommands() const {
    return {
        "add-funds", "list-flights", "filter-flights", "book-ticket",
        "upgrade-ticket", "add-baggage", "cancel-ticket", "my-tickets",
        "view-profile", "logout", "help"
    };
}

double Passenger::getPassengerBalance() const {
    return balance;
}

bool Passenger::passengerWithdraw(double amount) {
    return withdraw(amount);
}

void Passenger::passengerDeposit(double amount) {
    deposit(amount);
}

void Passenger::passengerAddFunds(double amount) {
    addFunds(amount);
}
