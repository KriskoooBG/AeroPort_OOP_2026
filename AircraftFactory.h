#pragma once

#include "Aircraft.h"

#include <memory>
#include <string>

class AircraftFactory {
public:
    static std::unique_ptr<Aircraft> create(
        int id,
        AircraftType type,
        const std::string& model,
        const std::string& airlineName,
        int capacityOrTonnage
    );
};
