#pragma once

#include "Airline.h"
#include "Flight.h"
#include "Hangar.h"
#include "Runway.h"
#include "Ticket.h"
#include "User.h"
#include "Aircraft.h"

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

class AeroPortSystem {
public:
    static AeroPortSystem& instance();

    AeroPortSystem(const AeroPortSystem&) = delete;
    AeroPortSystem& operator=(const AeroPortSystem&) = delete;

    std::string registerUser(const std::string& username, const std::string& password, Role role);
    std::string login(const std::string& username, const std::string& password);
    std::string logout();
    std::string help() const;
    std::string viewProfile() const;
    bool hasCurrentUser() const;
    std::string buildRunway(const std::string& id, int length, bool hasIls, bool hasVipTerminal, bool hasHeavyDuty);
    std::string buildHangar(const std::string& id, int capacity, double repairFee);
    std::string closeRunway(const std::string& runwayId);
    std::string registerAirline(const std::string& name, double initialCapital);
    std::string buyAircraft(const std::string& airlineName, AircraftType type, const std::string& model, int capacityOrTonnage);
    std::string cloneAircraft(int aircraftId, int count);
    std::string scheduleFlight(const std::string& flightId, int aircraftId, const std::string& destination, double basePrice);
    std::string listFleet(const std::string& airlineName) const;
    std::string listAirspace() const;
    std::string listRunways() const;
    std::string assignRunway(const std::string& flightId, const std::string& runwayId);
    std::string delayFlight(const std::string& flightId, const std::string& reason);
    std::string freeRunway(const std::string& runwayId);
    std::string undo();

    std::string addFunds(double amount);
    std::string listFlights(const std::string& destination) const;
    std::string filterFlights(double maxPrice) const;
    std::string bookTicket(const std::string& flightId, TicketType type);
    std::string upgradeTicket(const std::string& flightId, TicketType newType);
    std::string addBaggage(const std::string& flightId, double weightKg);
    std::string cancelTicket(const std::string& flightId);
    std::string myTickets() const;

private:
    AeroPortSystem();

    std::shared_ptr<const User> currentUser() const;
    std::shared_ptr<User> currentUser();
    bool userExists(const std::string& username) const;
    bool currentUserHasRole(Role role) const;
    std::shared_ptr<Airline> findAirlineByAircraftId(int aircraftId);
    std::shared_ptr<const Airline> findAirlineByAircraftId(int aircraftId) const;

    std::unordered_map<std::string, std::shared_ptr<User>> users;
    std::string currentUsername;

    std::unordered_map<std::string, std::shared_ptr<Airline>> airlines;
    std::unordered_map<std::string, Flight> flights;
    std::unordered_map<std::string, Runway> runways;
    std::unordered_map<std::string, Hangar> hangars;
    std::unordered_map<int, std::shared_ptr<Aircraft>> aircraft;
    std::vector<Ticket> tickets;
    int nextAircraftId = 1;

    double airportBalance = 0.0;

    struct DispatcherAction {
        enum class Type { Assign, Free, HangarAdmit, HangarRetrieve } type;
        std::string flightId;
        std::string runwayId;
        double ticketRevenue = 0.0;
        double airportFee = 0.0;
        double transferredToAirline = 0.0;
        int aircraftId = 0;
        int prevAircraftHealth = 0;
        double prevAirlineBalance = 0.0;
        double prevAirportBalance = 0.0;
    };
    std::vector<DispatcherAction> actionHistory;

public:
    std::string save(const std::string& filename = "aeroport_data.bin") const;
    std::string load(const std::string& filename = "aeroport_data.bin");

    std::string sendToHangar(int aircraftId, const std::string& hangarId);
    std::string retrieveFromHangar(int aircraftId);
    std::string cancelFlight(const std::string& flightId);
    std::string flightRevenue(const std::string& flightId) const;
    std::string airportReport() const;
    std::string auditAirline(const std::string& airlineName) const;
    std::string setWeather(Weather weather);
};
