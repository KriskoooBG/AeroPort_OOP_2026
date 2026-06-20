#pragma once

#include "Aircraft.h"

class PassengerPlane : public Aircraft {
public:
    PassengerPlane(int id, std::string model, std::string airlineName, int capacity, int health = 100);

    AircraftType getType() const override;
    bool canUseRunway(const Runway& runway) const override;
    int getHealthLossPerFlight() const override;
    double calculateAirportFee(double ticketRevenue) const override;
    int getCapacityUnits() const override;
    std::unique_ptr<Aircraft> cloneWithId(int newId) const override;

private:
    int capacity;
};
