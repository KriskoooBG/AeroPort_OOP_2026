#include "CommandProcessor.h"

#include "AeroPortSystem.h"
#include "Enums.h"

#include <sstream>
#include <stdexcept>

namespace {
bool parseInt(const std::string& value, int& result) {
    try {
        std::size_t parsedChars = 0;
        result = std::stoi(value, &parsedChars);
        return parsedChars == value.size();
    } catch (const std::exception&) {
        return false;
    }
}

bool parseDouble(const std::string& value, double& result) {
    try {
        std::size_t parsedChars = 0;
        result = std::stod(value, &parsedChars);
        return parsedChars == value.size();
    } catch (const std::exception&) {
        return false;
    }
}

bool isFlag(const std::string& value, const std::string& shortName, const std::string& longName) {
    return value == shortName || value == longName;
}
}

std::string CommandProcessor::process(const std::string& line) {
    const std::vector<std::string> args = tokenize(line);
    if (args.empty()) {
        return "";
    }

    AeroPortSystem& system = AeroPortSystem::instance();
    const std::string& command = args[0];

    if (command == "help") {
        return system.help();
    }
    if (command == "login") {
        if (args.size() != 3) {
            return "[Error] Usage: login [username] [password]";
        }
        return system.login(args[1], args[2]);
    }
    if (command == "logout") {
        return system.logout();
    }
    if (command == "view-profile") {
        return system.viewProfile();
    }
    if (command == "register") {
        if (args.size() != 4) {
            return "[Error] Usage: register [username] [password] [Passenger|Dispatcher]";
        }

        Role role;
        if (!parseRole(args[3], role)) {
            return "[Error] Invalid role. Allowed roles: Passenger, Dispatcher.";
        }
        return system.registerUser(args[1], args[2], role);
    }
    if (command == "build-runway") {
        if (args.size() < 3) {
            return "[Error] Usage: build-runway [id] [length] [-ils] [-vip] [-heavy]";
        }

        int length = 0;
        if (!parseInt(args[2], length)) {
            return "[Error] Runway length must be a valid integer.";
        }

        bool hasIls = false;
        bool hasVip = false;
        bool hasHeavy = false;

        for (std::size_t i = 3; i < args.size(); ++i) {
            if (isFlag(args[i], "-ils", "ILS")) {
                hasIls = true;
            } else if (isFlag(args[i], "-vip", "VIP")) {
                hasVip = true;
            } else if (isFlag(args[i], "-heavy", "HeavyDuty")) {
                hasHeavy = true;
            } else {
                return "[Error] Unknown runway flag '" + args[i] + "'.";
            }
        }

        return system.buildRunway(args[1], length, hasIls, hasVip, hasHeavy);
    }
    if (command == "build-hangar") {
        if (args.size() != 4) {
            return "[Error] Usage: build-hangar [id] [capacity] [repair_fee]";
        }

        int capacity = 0;
        double repairFee = 0.0;
        if (!parseInt(args[2], capacity)) {
            return "[Error] Hangar capacity must be a valid integer.";
        }
        if (!parseDouble(args[3], repairFee)) {
            return "[Error] Repair fee must be a valid number.";
        }

        return system.buildHangar(args[1], capacity, repairFee);
    }
    if (command == "close-runway") {
        if (args.size() != 2) {
            return "[Error] Usage: close-runway [runway_id]";
        }

        return system.closeRunway(args[1]);
    }
    if (command == "register-airline") {
        if (args.size() != 3) {
            return "[Error] Usage: register-airline [name] [initial_capital]";
        }

        double initialCapital = 0.0;
        if (!parseDouble(args[2], initialCapital)) {
            return "[Error] Initial capital must be a valid number.";
        }

        return system.registerAirline(args[1], initialCapital);
    }
    if (command == "buy-aircraft") {
        if (args.size() != 5) {
            return "[Error] Usage: buy-aircraft [airline_name] [type] [model] [capacity/tonnage]";
        }

        AircraftType type;
        int capacityOrTonnage = 0;
        if (!parseAircraftType(args[2], type)) {
            return "[Error] Invalid aircraft type. Use PassengerPlane, CargoPlane or PrivateJet.";
        }
        if (!parseInt(args[4], capacityOrTonnage)) {
            return "[Error] Capacity/tonnage must be a valid integer.";
        }

        return system.buyAircraft(args[1], type, args[3], capacityOrTonnage);
    }
    if (command == "clone-aircraft") {
        if (args.size() != 3) {
            return "[Error] Usage: clone-aircraft [id] [count]";
        }

        int aircraftId = 0;
        int count = 0;
        if (!parseInt(args[1], aircraftId)) {
            return "[Error] Aircraft ID must be a valid integer.";
        }
        if (!parseInt(args[2], count)) {
            return "[Error] Clone count must be a valid integer.";
        }

        return system.cloneAircraft(aircraftId, count);
    }
    if (command == "schedule-flight") {
        if (args.size() != 5) {
            return "[Error] Usage: schedule-flight [flight_id] [aircraft_id] [destination] [base_price]";
        }

        int aircraftId = 0;
        double basePrice = 0.0;
        if (!parseInt(args[2], aircraftId)) {
            return "[Error] Aircraft ID must be a valid integer.";
        }
        if (!parseDouble(args[4], basePrice)) {
            return "[Error] Base price must be a valid number.";
        }

        return system.scheduleFlight(args[1], aircraftId, args[3], basePrice);
    }
    if (command == "list-fleet") {
        if (args.size() != 2) {
            return "[Error] Usage: list-fleet [airline_name]";
        }

        return system.listFleet(args[1]);
    }

    if (command == "add-funds") {
        if (args.size() != 2) return "[Error] Usage: add-funds [amount]";
        double amount = 0.0;
        if (!parseDouble(args[1], amount)) return "[Error] Amount must be a valid number.";
        return system.addFunds(amount);
    }
    if (command == "list-flights") {
        if (args.size() != 2) return "[Error] Usage: list-flights [destination]";
        return system.listFlights(args[1]);
    }
    if (command == "filter-flights") {
        if (args.size() != 2) return "[Error] Usage: filter-flights [max_price]";
        double maxPrice = 0.0;
        if (!parseDouble(args[1], maxPrice)) return "[Error] Price must be a valid number.";
        return system.filterFlights(maxPrice);
    }
    if (command == "book-ticket") {
        if (args.size() != 3) return "[Error] Usage: book-ticket [flight_id] [ticket_type]";
        TicketType type;
        if (!parseTicketType(args[2], type)) return "[Error] Invalid ticket type. Use Standard, LastMinute or VIP.";
        return system.bookTicket(args[1], type);
    }
    if (command == "upgrade-ticket") {
        if (args.size() != 3) return "[Error] Usage: upgrade-ticket [flight_id] [new_ticket_type]";
        TicketType type;
        if (!parseTicketType(args[2], type)) return "[Error] Invalid ticket type. Use Standard, LastMinute or VIP.";
        return system.upgradeTicket(args[1], type);
    }
    if (command == "add-baggage") {
        if (args.size() != 3) return "[Error] Usage: add-baggage [flight_id] [weight]";
        double w = 0.0;
        if (!parseDouble(args[2], w)) return "[Error] Weight must be a valid number.";
        return system.addBaggage(args[1], w);
    }
    if (command == "cancel-ticket") {
        if (args.size() != 2) return "[Error] Usage: cancel-ticket [flight_id]";
        return system.cancelTicket(args[1]);
    }
    if (command == "my-tickets") {
        if (args.size() != 1) return "[Error] Usage: my-tickets";
        return system.myTickets();
    }
    if (command == "list-airspace") {
        if (args.size() != 1) return "[Error] Usage: list-airspace";
        return system.listAirspace();
    }
    if (command == "list-runways") {
        if (args.size() != 1) return "[Error] Usage: list-runways";
        return system.listRunways();
    }
    if (command == "assign-runway") {
        if (args.size() != 3) return "[Error] Usage: assign-runway [flight_id] [runway_id]";
        return system.assignRunway(args[1], args[2]);
    }
    if (command == "delay-flight") {
        if (args.size() < 2) return "[Error] Usage: delay-flight [flight_id] [reason]";
        std::string reason;
        if (args.size() > 2) {
            for (size_t i = 2; i < args.size(); ++i) {
                if (i > 2) reason += " ";
                reason += args[i];
            }
        }
        return system.delayFlight(args[1], reason);
    }
    if (command == "free-runway") {
        if (args.size() != 2) return "[Error] Usage: free-runway [runway_id]";
        return system.freeRunway(args[1]);
    }
    if (command == "undo") {
        if (args.size() != 1) return "[Error] Usage: undo";
        return system.undo();
    }
    if (command == "send-to-hangar") {
        if (args.size() != 3) return "[Error] Usage: send-to-hangar [aircraft_id] [hangar_id]";
        int aid = 0;
        if (!parseInt(args[1], aid)) return "[Error] Aircraft ID must be a valid integer.";
        return system.sendToHangar(aid, args[2]);
    }
    if (command == "retrieve-from-hangar") {
        if (args.size() != 2) return "[Error] Usage: retrieve-from-hangar [aircraft_id]";
        int aid = 0;
        if (!parseInt(args[1], aid)) return "[Error] Aircraft ID must be a valid integer.";
        return system.retrieveFromHangar(aid);
    }
    if (command == "cancel-flight") {
        if (args.size() != 2) return "[Error] Usage: cancel-flight [flight_id]";
        return system.cancelFlight(args[1]);
    }
    if (command == "flight-revenue") {
        if (args.size() != 2) return "[Error] Usage: flight-revenue [flight_id]";
        return system.flightRevenue(args[1]);
    }
    if (command == "airport-report") {
        if (args.size() != 1) return "[Error] Usage: airport-report";
        return system.airportReport();
    }
    if (command == "audit-airline") {
        if (args.size() != 2) return "[Error] Usage: audit-airline [airline_name]";
        return system.auditAirline(args[1]);
    }
    if (command == "set-weather") {
        if (args.size() != 2) return "[Error] Usage: set-weather [SUNNY|STORM]";
        Weather w;
        if (!parseWeather(args[1], w)) return "[Error] Invalid weather. Use SUNNY or STORM.";
        return system.setWeather(w);
    }
    if (command == "save") {
        return system.save();
    }
    if (command == "load") {
        return system.load();
    }

    return "[Error] Unknown command. Type help for available commands.";
}

std::vector<std::string> CommandProcessor::tokenize(const std::string& line) const {
    std::istringstream input(line);
    std::vector<std::string> tokens;
    std::string token;

    while (input >> token) {
        tokens.push_back(token);
    }
    return tokens;
}
