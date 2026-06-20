#pragma once

#include <string>
#include <vector>

class CommandProcessor {
public:
    std::string process(const std::string& line);

private:
    std::vector<std::string> tokenize(const std::string& line) const;
};
