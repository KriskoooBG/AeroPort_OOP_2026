#pragma once

#include "Enums.h"

#include <optional>
#include <string>

class Runway {
public:
    Runway(std::string id, int length, bool hasIls, bool hasVipTerminal, bool hasHeavyDuty);

    const std::string& getId() const;
    int getLength() const;
    RunwayStatus getStatus() const;
    bool supportsIls() const;
    bool supportsVipTerminal() const;
    bool supportsHeavyDuty() const;
    std::optional<int> getOccupiedAircraftId() const;

    bool isFree() const;
    void occupy(int aircraftId);
    void free();
    void closeForMaintenance();

    std::string info() const;

private:
    std::string id;
    int length;
    RunwayStatus status;
    bool hasIls;
    bool hasVipTerminal;
    bool hasHeavyDuty;
    std::optional<int> occupiedAircraftId;
};
