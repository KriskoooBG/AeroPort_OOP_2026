#include "Runway.h"

#include <sstream>
#include <utility>

Runway::Runway(std::string id, int length, bool hasIls, bool hasVipTerminal, bool hasHeavyDuty)
    : id(std::move(id)),
      length(length),
      status(RunwayStatus::Free),
      hasIls(hasIls),
      hasVipTerminal(hasVipTerminal),
      hasHeavyDuty(hasHeavyDuty) {}

const std::string& Runway::getId() const {
    return id;
}

int Runway::getLength() const {
    return length;
}

RunwayStatus Runway::getStatus() const {
    return status;
}

bool Runway::supportsIls() const {
    return hasIls;
}

bool Runway::supportsVipTerminal() const {
    return hasVipTerminal;
}

bool Runway::supportsHeavyDuty() const {
    return hasHeavyDuty;
}

std::optional<int> Runway::getOccupiedAircraftId() const {
    return occupiedAircraftId;
}

bool Runway::isFree() const {
    return status == RunwayStatus::Free;
}

void Runway::occupy(int aircraftId) {
    status = RunwayStatus::Occupied;
    occupiedAircraftId = aircraftId;
}

void Runway::free() {
    status = RunwayStatus::Free;
    occupiedAircraftId.reset();
}

void Runway::closeForMaintenance() {
    status = RunwayStatus::Maintenance;
    occupiedAircraftId.reset();
}

std::string Runway::info() const {
    std::ostringstream out;
    out << id << " (" << length << "m";
    if (hasIls) {
        out << ", ILS";
    }
    if (hasVipTerminal) {
        out << ", VIP";
    }
    if (hasHeavyDuty) {
        out << ", HeavyDuty";
    }
    if (!hasIls && !hasVipTerminal && !hasHeavyDuty) {
        out << ", Basic";
    }
    out << ") -> Status: " << toString(status);
    if (occupiedAircraftId.has_value()) {
        out << " by aircraft " << occupiedAircraftId.value();
    }
    return out.str();
}
