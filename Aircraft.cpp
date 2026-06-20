#include "Aircraft.h"

#include <algorithm>
#include <utility>

Aircraft::Aircraft(int id, std::string model, std::string airlineName, int health)
    : id(id), model(std::move(model)), airlineName(std::move(airlineName)), health(health) {}

int Aircraft::getId() const {
    return id;
}

const std::string& Aircraft::getModel() const {
    return model;
}

const std::string& Aircraft::getAirlineName() const {
    return airlineName;
}

int Aircraft::getHealth() const {
    return health;
}

bool Aircraft::canFly() const {
    return health >= 20;
}

void Aircraft::repair() {
    health = 100;
}

void Aircraft::decreaseHealthAfterFlight() {
    health = std::max(0, health - getHealthLossPerFlight());
}

void Aircraft::setHealth(int h) {
    health = std::clamp(h, 0, 100);
}
