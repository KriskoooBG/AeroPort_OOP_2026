#include "Validation.h"

#include <cctype>

bool isPositiveNumber(double value) {
    return value > 0.0;
}

bool isPositiveInteger(int value) {
    return value > 0;
}

bool isValidIdentifier(const std::string& value) {
    if (value.empty()) {
        return false;
    }

    for (const char ch : value) {
        const unsigned char symbol = static_cast<unsigned char>(ch);
        if (!std::isalnum(symbol) && ch != '-' && ch != '_') {
            return false;
        }
    }
    return true;
}
