#include "Ticket.h"

#include <utility>

Ticket::Ticket(std::string ownerName, std::string flightId, TicketType type, double paidAmount, double baggageKg)
    : ownerName(std::move(ownerName)),
      flightId(std::move(flightId)),
      type(type),
      paidAmount(paidAmount),
      baggageKg(baggageKg) {}

const std::string& Ticket::getOwnerName() const {
    return ownerName;
}

const std::string& Ticket::getFlightId() const {
    return flightId;
}

TicketType Ticket::getType() const {
    return type;
}

double Ticket::getPaidAmount() const {
    return paidAmount;
}

double Ticket::getBaggageKg() const {
    return baggageKg;
}

bool Ticket::isRefundable() const {
    return type != TicketType::LastMinute;
}

void Ticket::addBaggage(double weightKg, double fee) {
    baggageKg += weightKg;
    (void)fee;
}

void Ticket::upgrade(TicketType newType, double paidDifference, double includedBaggageKg) {
    type = newType;
    paidAmount += paidDifference;
    if (includedBaggageKg > baggageKg) {
        baggageKg = includedBaggageKg;
    }
}

bool Ticket::isActive() const {
    return active;
}

void Ticket::cancel() {
    active = false;
}
