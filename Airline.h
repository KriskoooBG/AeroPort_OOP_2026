#pragma once

#include <string>
#include <vector>

class Airline {
public:
    Airline(std::string name, double balance);

    const std::string& getName() const;
    double getBalance() const;
    bool withdraw(double amount);
    void deposit(double amount);

    void addAircraft(int aircraftId);
    void addFlight(const std::string& flightId);
    const std::vector<int>& getAircraftIds() const;
    const std::vector<std::string>& getFlightIds() const;

private:
    std::string name;
    double balance;
    std::vector<int> aircraftIds;
    std::vector<std::string> flightIds;
};
