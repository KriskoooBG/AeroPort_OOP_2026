#pragma once

#include "Runway.h"

#include <string>

class RunwayBuilder {
public:
    RunwayBuilder& withId(const std::string& id);
    RunwayBuilder& withLength(int length);
    RunwayBuilder& withIls(bool value = true);
    RunwayBuilder& withVipTerminal(bool value = true);
    RunwayBuilder& withHeavyDuty(bool value = true);

    Runway build() const;

private:
    std::string id;
    int length = 0;
    bool hasIls = false;
    bool hasVipTerminal = false;
    bool hasHeavyDuty = false;
};
