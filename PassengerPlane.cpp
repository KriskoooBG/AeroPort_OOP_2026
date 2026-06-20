#include "PassengerPlane.h"

#include "Runway.h"

#include <utility>

PassengerPlane::PassengerPlane(int id, std::string model, std::string airlineName, int capacity, int health)
    : Aircraft(id, std::move(model), std::move(airlineName), health), capacity(capacity) {}

AircraftType PassengerPlane::getType() const {
    return AircraftType::PassengerPlane;
}

bool PassengerPlane::canUseRunway(const Runway& runway) const {
    return runway.isFree() && runway.getLength() >= 2000;
}

int PassengerPlane::getHealthLossPerFlight() const {
    return 35;
}

double PassengerPlane::calculateAirportFee(double ticketRevenue) const {
    return ticketRevenue * 0.10;
}

int PassengerPlane::getCapacityUnits() const {
    return capacity;
}

std::unique_ptr<Aircraft> PassengerPlane::cloneWithId(int newId) const {
    return std::make_unique<PassengerPlane>(newId, model, airlineName, capacity, health);
}
