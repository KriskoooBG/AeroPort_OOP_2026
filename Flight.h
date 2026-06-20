#pragma once

#include "Enums.h"

#include <optional>
#include <string>
#include <vector>

class Flight {
public:
    Flight(std::string id, int aircraftId, std::string airlineName, std::string destination, double basePrice);

    const std::string& getId() const;
    int getAircraftId() const;
    const std::string& getAirlineName() const;
    const std::string& getDestination() const;
    double getBasePrice() const;
    FlightStatus getStatus() const;
    std::optional<std::string> getAssignedRunwayId() const;

    void setStatus(FlightStatus status);
    void assignRunway(const std::string& runwayId);
    void clearRunway();
    void addTicketIndex(int ticketIndex);
    const std::vector<int>& getTicketIndexes() const;

private:
    std::string id;
    int aircraftId;
    std::string airlineName;
    std::string destination;
    double basePrice;
    FlightStatus status;
    std::optional<std::string> assignedRunwayId;
    std::vector<int> ticketIndexes;
};
