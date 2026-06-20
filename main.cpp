#include "CommandProcessor.h"

#include <iostream>
#include <string>
#include "AeroPortSystem.h"

int main() {
    CommandProcessor processor;
    std::string line;

    std::cout << "[System] AeroPort started. Type help for available commands." << std::endl;
    std::cout << AeroPortSystem::instance().load() << std::endl;

    while (true) {
        std::cout << "> ";
        if (!std::getline(std::cin, line)) {
            std::cout << AeroPortSystem::instance().save() << std::endl;
            break;
        }
        if (line.empty()) continue;
        if (line == "exit") {
            std::cout << AeroPortSystem::instance().save() << std::endl;
            std::cout << "[System] Goodbye." << std::endl;
            break;
        }
        const std::string result = processor.process(line);
        if (!result.empty()) std::cout << result << std::endl;
    }

    return 0;
}
