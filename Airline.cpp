#include "Airline.h"

#include <utility>

Airline::Airline(std::string name, double balance)
    : name(std::move(name)), balance(balance) {}

const std::string& Airline::getName() const {
    return name;
}

double Airline::getBalance() const {
    return balance;
}

bool Airline::withdraw(double amount) {
    if (amount > balance) {
        return false;
    }
    balance -= amount;
    return true;
}

void Airline::deposit(double amount) {
    balance += amount;
}

void Airline::addAircraft(int aircraftId) {
    aircraftIds.push_back(aircraftId);
}

void Airline::addFlight(const std::string& flightId) {
    flightIds.push_back(flightId);
}

const std::vector<int>& Airline::getAircraftIds() const {
    return aircraftIds;
}

const std::vector<std::string>& Airline::getFlightIds() const {
    return flightIds;
}
