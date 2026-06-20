#include "Enums.h"

std::string toString(Role role) {
    switch (role) {
    case Role::Passenger:
        return "Passenger";
    case Role::Dispatcher:
        return "Dispatcher";
    case Role::Administrator:
        return "Administrator";
    }
    return "Unknown";
}

std::string toString(TicketType type) {
    switch (type) {
    case TicketType::Standard:
        return "Standard";
    case TicketType::LastMinute:
        return "LastMinute";
    case TicketType::VIP:
        return "VIP";
    }
    return "Unknown";
}

std::string toString(FlightStatus status) {
    switch (status) {
    case FlightStatus::Scheduled:
        return "Scheduled";
    case FlightStatus::Boarding:
        return "Boarding";
    case FlightStatus::Delayed:
        return "Delayed";
    case FlightStatus::Departed:
        return "Departed";
    case FlightStatus::Cancelled:
        return "Cancelled";
    }
    return "Unknown";
}

std::string toString(RunwayStatus status) {
    switch (status) {
    case RunwayStatus::Free:
        return "Free";
    case RunwayStatus::Occupied:
        return "Occupied";
    case RunwayStatus::Maintenance:
        return "Maintenance";
    }
    return "Unknown";
}

std::string toString(Weather weather) {
    switch (weather) {
    case Weather::Sunny:
        return "SUNNY";
    case Weather::Storm:
        return "STORM";
    }
    return "Unknown";
}

std::string toString(AircraftType type) {
    switch (type) {
    case AircraftType::PassengerPlane:
        return "PassengerPlane";
    case AircraftType::CargoPlane:
        return "CargoPlane";
    case AircraftType::PrivateJet:
        return "PrivateJet";
    }
    return "Unknown";
}

bool parseRole(const std::string& value, Role& role) {
    if (value == "Passenger") {
        role = Role::Passenger;
        return true;
    }
    if (value == "Dispatcher" || value == "TrafficControl") {
        role = Role::Dispatcher;
        return true;
    }
    if (value == "Administrator" || value == "Admin") {
        role = Role::Administrator;
        return true;
    }
    return false;
}

bool parseTicketType(const std::string& value, TicketType& type) {
    if (value == "Standard") {
        type = TicketType::Standard;
        return true;
    }
    if (value == "LastMinute") {
        type = TicketType::LastMinute;
        return true;
    }
    if (value == "VIP") {
        type = TicketType::VIP;
        return true;
    }
    return false;
}

bool parseWeather(const std::string& value, Weather& weather) {
    if (value == "SUNNY" || value == "Sunny") {
        weather = Weather::Sunny;
        return true;
    }
    if (value == "STORM" || value == "Storm") {
        weather = Weather::Storm;
        return true;
    }
    return false;
}

bool parseAircraftType(const std::string& value, AircraftType& type) {
    if (value == "PassengerPlane") {
        type = AircraftType::PassengerPlane;
        return true;
    }
    if (value == "CargoPlane") {
        type = AircraftType::CargoPlane;
        return true;
    }
    if (value == "PrivateJet") {
        type = AircraftType::PrivateJet;
        return true;
    }
    return false;
}
