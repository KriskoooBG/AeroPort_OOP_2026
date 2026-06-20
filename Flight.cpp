#include "Flight.h"

#include <utility>

Flight::Flight(std::string id, int aircraftId, std::string airlineName, std::string destination, double basePrice)
    : id(std::move(id)),
      aircraftId(aircraftId),
      airlineName(std::move(airlineName)),
      destination(std::move(destination)),
      basePrice(basePrice),
      status(FlightStatus::Scheduled) {}

const std::string& Flight::getId() const {
    return id;
}

int Flight::getAircraftId() const {
    return aircraftId;
}

const std::string& Flight::getAirlineName() const {
    return airlineName;
}

const std::string& Flight::getDestination() const {
    return destination;
}

double Flight::getBasePrice() const {
    return basePrice;
}

FlightStatus Flight::getStatus() const {
    return status;
}

std::optional<std::string> Flight::getAssignedRunwayId() const {
    return assignedRunwayId;
}

void Flight::setStatus(FlightStatus status) {
    this->status = status;
}

void Flight::assignRunway(const std::string& runwayId) {
    assignedRunwayId = runwayId;
    status = FlightStatus::Boarding;
}

void Flight::clearRunway() {
    assignedRunwayId.reset();
}

void Flight::addTicketIndex(int ticketIndex) {
    ticketIndexes.push_back(ticketIndex);
}

const std::vector<int>& Flight::getTicketIndexes() const {
    return ticketIndexes;
}
