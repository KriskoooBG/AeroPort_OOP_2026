#include "PrivateJet.h"

#include "Runway.h"

#include <utility>

PrivateJet::PrivateJet(int id, std::string model, std::string airlineName, int health)
    : Aircraft(id, std::move(model), std::move(airlineName), health) {}

AircraftType PrivateJet::getType() const {
    return AircraftType::PrivateJet;
}

bool PrivateJet::canUseRunway(const Runway& runway) const {
    return runway.isFree() && runway.getLength() >= 1000 && runway.supportsVipTerminal();
}

int PrivateJet::getHealthLossPerFlight() const {
    return 25;
}

double PrivateJet::calculateAirportFee(double) const {
    return 5000.0;
}

int PrivateJet::getCapacityUnits() const {
    return 12;
}

std::unique_ptr<Aircraft> PrivateJet::cloneWithId(int newId) const {
    return std::make_unique<PrivateJet>(newId, model, airlineName, health);
}
