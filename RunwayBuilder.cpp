#include "RunwayBuilder.h"

RunwayBuilder& RunwayBuilder::withId(const std::string& id) {
    this->id = id;
    return *this;
}

RunwayBuilder& RunwayBuilder::withLength(int length) {
    this->length = length;
    return *this;
}

RunwayBuilder& RunwayBuilder::withIls(bool value) {
    hasIls = value;
    return *this;
}

RunwayBuilder& RunwayBuilder::withVipTerminal(bool value) {
    hasVipTerminal = value;
    return *this;
}

RunwayBuilder& RunwayBuilder::withHeavyDuty(bool value) {
    hasHeavyDuty = value;
    return *this;
}

Runway RunwayBuilder::build() const {
    return Runway(id, length, hasIls, hasVipTerminal, hasHeavyDuty);
}
