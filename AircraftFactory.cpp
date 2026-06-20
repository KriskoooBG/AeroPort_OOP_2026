#include "AircraftFactory.h"

#include "CargoPlane.h"
#include "PassengerPlane.h"
#include "PrivateJet.h"

std::unique_ptr<Aircraft> AircraftFactory::create(
    int id,
    AircraftType type,
    const std::string& model,
    const std::string& airlineName,
    int capacityOrTonnage
) {
    switch (type) {
    case AircraftType::PassengerPlane:
        return std::make_unique<PassengerPlane>(id, model, airlineName, capacityOrTonnage);
    case AircraftType::CargoPlane:
        return std::make_unique<CargoPlane>(id, model, airlineName, capacityOrTonnage);
    case AircraftType::PrivateJet:
        return std::make_unique<PrivateJet>(id, model, airlineName);
    }

    return nullptr;
}
