#include <fstream>
#include <iostream>
#include <string>

int main() {
    std::ifstream input("input.txt");
    std::ofstream output("output.txt");

    if (!input.is_open() || !output.is_open()) {
        std::cerr << "Error opening input or output file." << std::endl;
        return 1;
    }

    std::string line;
    output << "Processed Data:\n";
    while (std::getline(input, line)) {
        output << "-> " << line << std::endl;
    }

    input.close();
    output.close();

    std::cout << "C++ program executed successfully." << std::endl;
    return 0;
}
