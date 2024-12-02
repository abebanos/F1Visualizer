#include "data_parser.h"
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <iostream>

std::vector<std::vector<std::string>> DataParser::parseCSV(const std::string& filepath) {
    std::vector<std::vector<std::string>> data;
    std::ifstream file(filepath);

    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + filepath);
    }

    std::string line;
    while (std::getline(file, line)) {
        std::vector<std::string> row;
        std::stringstream ss(line);
        std::string cell;

        while (std::getline(ss, cell, ',')) {
            row.push_back(cell);
        }
        data.push_back(row);
    }

    file.close();
    return data;
}
