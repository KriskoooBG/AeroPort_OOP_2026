#pragma once

#include <string>
#include <vector>

class Hangar {
public:
    Hangar(std::string id, int capacity, double repairFee);

    const std::string& getId() const;
    int getCapacity() const;
    double getRepairFee() const;
    bool hasFreeSlot() const;
    bool containsAircraft(int aircraftId) const;
    bool admitAircraft(int aircraftId);
    bool releaseAircraft(int aircraftId);
    const std::vector<int>& getAircraftIds() const;

private:
    std::string id;
    int capacity;
    double repairFee;
    std::vector<int> aircraftIds;
};
