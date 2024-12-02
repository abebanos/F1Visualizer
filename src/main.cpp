#include <iostream>
#include "data_parser.h"

void testParseCSV() {
    try {
        std::string filepath = "data/drivers.csv";
        std::vector<std::vector<std::string>> data = DataParser::parseCSV(filepath);

        std::cout << "Parsed CSV Data:" << std::endl;
        for (const auto& row : data) {
            for (const auto& cell : row) {
                std::cout << cell << " ";
            }
            std::cout << std::endl;
        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

int main() {
    std::cout << "Running CSV Parser Test..." << std::endl;
    testParseCSV();
    return 0;
}
