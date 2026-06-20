#pragma once

#include "Enums.h"

#include <memory>
#include <string>

class Runway;

class Aircraft {
public:
    Aircraft(int id, std::string model, std::string airlineName, int health);
    virtual ~Aircraft() = default;

    int getId() const;
    const std::string& getModel() const;
    const std::string& getAirlineName() const;
    int getHealth() const;
    bool canFly() const;
    void repair();
    void decreaseHealthAfterFlight();
    void setHealth(int h);

    virtual AircraftType getType() const = 0;
    virtual bool canUseRunway(const Runway& runway) const = 0;
    virtual int getHealthLossPerFlight() const = 0;
    virtual double calculateAirportFee(double ticketRevenue) const = 0;
    virtual int getCapacityUnits() const = 0;
    virtual std::unique_ptr<Aircraft> cloneWithId(int newId) const = 0;

protected:
    int id;
    std::string model;
    std::string airlineName;
    int health;
};
