#pragma once

#include "Aircraft.h"

class CargoPlane : public Aircraft {
public:
    CargoPlane(int id, std::string model, std::string airlineName, int tonnage, int health = 100);

    AircraftType getType() const override;
    bool canUseRunway(const Runway& runway) const override;
    int getHealthLossPerFlight() const override;
    double calculateAirportFee(double ticketRevenue) const override;
    int getCapacityUnits() const override;
    std::unique_ptr<Aircraft> cloneWithId(int newId) const override;

private:
    int tonnage;
};
