#pragma once

#include "Enums.h"

#include <string>

class Ticket {
public:
    Ticket(std::string ownerName, std::string flightId, TicketType type, double paidAmount, double baggageKg);

    const std::string& getOwnerName() const;
    const std::string& getFlightId() const;
    TicketType getType() const;
    double getPaidAmount() const;
    double getBaggageKg() const;
    bool isRefundable() const;
    void addBaggage(double weightKg, double fee);
    void upgrade(TicketType newType, double paidDifference, double includedBaggageKg);
    bool isActive() const;
    void cancel();

private:
    std::string ownerName;
    std::string flightId;
    TicketType type;
    double paidAmount;
    double baggageKg;
    bool active = true;
};
