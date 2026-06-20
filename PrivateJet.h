#pragma once

#include "Aircraft.h"

class PrivateJet : public Aircraft {
public:
    PrivateJet(int id, std::string model, std::string airlineName, int health = 100);

    AircraftType getType() const override;
    bool canUseRunway(const Runway& runway) const override;
    int getHealthLossPerFlight() const override;
    double calculateAirportFee(double ticketRevenue) const override;
    int getCapacityUnits() const override;
    std::unique_ptr<Aircraft> cloneWithId(int newId) const override;
};
