#pragma once

#include <string>

enum class Role {
    Passenger,
    Dispatcher,
    Administrator
};

enum class TicketType {
    Standard,
    LastMinute,
    VIP
};

enum class FlightStatus {
    Scheduled,
    Boarding,
    Delayed,
    Departed,
    Cancelled
};

enum class RunwayStatus {
    Free,
    Occupied,
    Maintenance
};

enum class Weather {
    Sunny,
    Storm
};

enum class AircraftType {
    PassengerPlane,
    CargoPlane,
    PrivateJet
};

std::string toString(Role role);
std::string toString(TicketType type);
std::string toString(FlightStatus status);
std::string toString(RunwayStatus status);
std::string toString(Weather weather);
std::string toString(AircraftType type);

bool parseRole(const std::string& value, Role& role);
bool parseTicketType(const std::string& value, TicketType& type);
bool parseWeather(const std::string& value, Weather& weather);
bool parseAircraftType(const std::string& value, AircraftType& type);
