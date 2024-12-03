#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <chrono>
#include <iomanip>
using namespace std;

// Store driver information
struct Driver {
    int driverId;
    string forename;
    string surname;
};

// Store lap time information
struct LapTime {
    int driverId;
    string time; // In seconds format
    int milliseconds;
};

// Custom Min-Heap implementation
class MinHeap {
    vector<LapTime> heap;

    void heapifyUp(int index) {
        while (index > 0) {
            int parent = (index - 1) / 2;
            if (heap[index].milliseconds >= heap[parent].milliseconds)
                break;
            swap(heap[index], heap[parent]);
            index = parent;
        }
    }

    void heapifyDown(int index) {
        int size = heap.size();
        while (true) {
            int left = 2 * index + 1, right = 2 * index + 2, smallest = index;

            if (left < size && heap[left].milliseconds < heap[smallest].milliseconds)
                smallest = left;
            if (right < size && heap[right].milliseconds < heap[smallest].milliseconds)
                smallest = right;

            if (smallest == index)
                break;

            swap(heap[index], heap[smallest]);
            index = smallest;
        }
    }

public:
    void insert(const LapTime& lapTime) {
        heap.push_back(lapTime);
        heapifyUp(heap.size() - 1);
    }

    LapTime extractMin() {
        LapTime min = heap[0];
        heap[0] = heap.back();
        heap.pop_back();
        heapifyDown(0);
        return min;
    }

    bool isEmpty() const {
        return heap.empty();
    }
};

// Helper functions
vector<string> split(const string& line, char delimiter) {
    vector<string> tokens;
    stringstream ss(line);
    string token;
    while (getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

string stripQuotes(const string& str) {
    if (str.size() >= 2 && str.front() == '"' && str.back() == '"') {
        return str.substr(1, str.size() - 2);
    }
    return str;
}


void loadDrivers(const string& filename, unordered_map<int, Driver>& drivers) {
    ifstream file(filename);
    string line;

    if (!file.is_open()) {
        cerr << "File not open: " << filename << "\n";
        return;
    }

    bool isHeader = true; // Flag to identify the header row
    while (getline(file, line)) {
        if (line == "driverId,driverRef,number,code,forename,surname,dob,nationality,url") {
            continue; // Skip header row
        }

        auto tokens = split(line, ',');
        if (tokens.size() >= 6) { // Ensure sufficient tokens
            try {
                int driverId = stoi(tokens[0]);
                string forename = stripQuotes(tokens[4]);
                string surname = stripQuotes(tokens[5]);
                drivers[driverId] = {driverId, forename, surname};
            } catch (...) {
                cerr << "Error parsing line: " << line << "\n";
            }
        } else {
            cerr << "Malformed line: " << line << "\n";
        }
    }
}



void displayTop5(const string& title, const vector<LapTime>& top5, const unordered_map<int, Driver>& drivers) {
    cout << "\n" << title << ":\n";
    cout << setw(20) << "Driver" << setw(10) << "Time\n";
    cout << "----------------------------------------\n";
    for (const auto& lapTime : top5) {
        if (drivers.find(lapTime.driverId) != drivers.end()) {
            const auto& driver = drivers.at(lapTime.driverId);
            cout << setw(20) << (driver.forename + " " + driver.surname)
                 << setw(10) << lapTime.time << "\n";
        } else {
            cerr << "Driver ID " << lapTime.driverId << " not found in drivers map!\n";
        }
    }
}



// Main function
int main() {
    // File paths
    const string driversFile = "csvFiles/drivers.csv";
    const string lapTimesFile = "csvFiles/lap_times.csv";

    // Load drivers
    unordered_map<int, Driver> drivers;
    loadDrivers(driversFile, drivers);

    // User input
    int selectedRaceId, selectedLap;
    cout << "Enter Race ID: ";
    cin >> selectedRaceId;
    cout << "Enter Lap Number: ";
    cin >> selectedLap;

    // Min-Heap approach
    MinHeap minHeap;
    auto startMinHeap = chrono::high_resolution_clock::now(); // Start timer for min heap
    ifstream file(lapTimesFile);
    string line;

    while (getline(file, line)) {
        if (line.empty()) {
            cerr << "Skipping empty line.\n";
            continue; // Skip empty lines
        }

        if (line == "raceId,driverId,lap,position,time,milliseconds") {
            continue; // Skip header row
        }

        auto tokens = split(line, ',');
        if (tokens.size() != 6) {
            cerr << "Unexpected number of tokens in line: " << line << "\n";
            continue; // Skip malformed lines
        }

        try {
            // Validate and convert each token
            int raceId = stoi(tokens[0]);
            int driverId = stoi(tokens[1]);
            int lap = stoi(tokens[2]);
            int milliseconds = stoi(tokens[5]);

            if (raceId == selectedRaceId && lap == selectedLap) {
                // cout << "Inserting driverId " << driverId << " with time " << tokens[4] << "\n"; Insertion debugging
                string time = stripQuotes(tokens[4]);
                minHeap.insert({driverId, time, milliseconds});
            }
        } catch (const exception& e) {
            cerr << "Error processing line: " << line << " - " << e.what() << "\n";
            continue;
        }
    }

    vector<LapTime> top5MinHeap;
    for (int i = 0; i < 5 && !minHeap.isEmpty(); ++i) {
        top5MinHeap.push_back(minHeap.extractMin());
    }
    auto endMinHeap = chrono::high_resolution_clock::now();
    chrono::duration<double> durationMinHeap = endMinHeap - startMinHeap;


    // Display results
    displayTop5("Min-Heap Results", top5MinHeap, drivers);
    cout << "\nMin-Heap Build & Query Runtime: " << durationMinHeap.count() << " seconds\n";

    return 0;
}
