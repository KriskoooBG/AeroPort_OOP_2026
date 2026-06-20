#include "CargoPlane.h"

#include "Runway.h"

#include <utility>

CargoPlane::CargoPlane(int id, std::string model, std::string airlineName, int tonnage, int health)
    : Aircraft(id, std::move(model), std::move(airlineName), health), tonnage(tonnage) {}

AircraftType CargoPlane::getType() const {
    return AircraftType::CargoPlane;
}

bool CargoPlane::canUseRunway(const Runway& runway) const {
    return runway.isFree() && runway.getLength() >= 3000 && runway.supportsHeavyDuty();
}

int CargoPlane::getHealthLossPerFlight() const {
    return 45;
}

double CargoPlane::calculateAirportFee(double) const {
    return tonnage * 15.0;
}

int CargoPlane::getCapacityUnits() const {
    return tonnage;
}

std::unique_ptr<Aircraft> CargoPlane::cloneWithId(int newId) const {
    return std::make_unique<CargoPlane>(newId, model, airlineName, tonnage, health);
}
