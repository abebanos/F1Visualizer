#pragma once

#include <string>
#include <vector>

class DataParser {
public:
    static std::vector<std::string> parseCSV(const std::string& filepath);
};