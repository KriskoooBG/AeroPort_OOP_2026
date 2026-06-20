#include "Hangar.h"

#include <algorithm>
#include <utility>

Hangar::Hangar(std::string id, int capacity, double repairFee)
    : id(std::move(id)), capacity(capacity), repairFee(repairFee) {}

const std::string& Hangar::getId() const {
    return id;
}

int Hangar::getCapacity() const {
    return capacity;
}

double Hangar::getRepairFee() const {
    return repairFee;
}

bool Hangar::hasFreeSlot() const {
    return static_cast<int>(aircraftIds.size()) < capacity;
}

bool Hangar::containsAircraft(int aircraftId) const {
    return std::find(aircraftIds.begin(), aircraftIds.end(), aircraftId) != aircraftIds.end();
}

bool Hangar::admitAircraft(int aircraftId) {
    if (!hasFreeSlot() || containsAircraft(aircraftId)) {
        return false;
    }
    aircraftIds.push_back(aircraftId);
    return true;
}

bool Hangar::releaseAircraft(int aircraftId) {
    const auto it = std::find(aircraftIds.begin(), aircraftIds.end(), aircraftId);
    if (it == aircraftIds.end()) {
        return false;
    }
    aircraftIds.erase(it);
    return true;
}

const std::vector<int>& Hangar::getAircraftIds() const {
    return aircraftIds;
}
