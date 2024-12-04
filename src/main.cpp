#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <chrono>
#include <iomanip>
#include <map>
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

// Custom B+ Tree implementation using multimap (simplified)
class BPlusTree {
    multimap<int, LapTime> tree;

public:
    void insert(const LapTime& lapTime) {
        tree.insert({lapTime.milliseconds, lapTime});
    }

    vector<LapTime> getTopK(int k) {
        vector<LapTime> topK;
        for (auto it = tree.begin(); it != tree.end() && k > 0; ++it, --k) {
            topK.push_back(it->second);
        }
        return topK;
    }

    bool isEmpty() const {
        return tree.empty();
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

// Load Results as a JSON for front end
void writeResultsToJson(const string& filename, const vector<LapTime>& minHeapTop5, const vector<LapTime>& bPlusTreeTop5, const unordered_map<int, Driver>& drivers, double minHeapRuntime, double bPlusTreeRuntime) {
    ofstream outFile(filename);
    if (!outFile.is_open()) {
        cerr << "Error: Could not open file " << filename << "\n";
        return;
    }

    outFile << "{\n";
    outFile << "  \"minHeapResults\": [\n";
    for (size_t i = 0; i < minHeapTop5.size(); ++i) {
        const auto& lapTime = minHeapTop5[i];
        if (drivers.find(lapTime.driverId) != drivers.end()) {
            const auto& driver = drivers.at(lapTime.driverId);
            outFile << "    {\n";
            outFile << "      \"driver\": \"" << driver.forename + " " + driver.surname << "\",\n";
            outFile << "      \"time\": \"" << lapTime.time << "\"\n";
            outFile << "    }";
            if (i != minHeapTop5.size() - 1) outFile << ",";
            outFile << "\n";
        }
    }
    outFile << "  ],\n";
    outFile << "  \"minHeapRuntime\": " << minHeapRuntime << ",\n";

    outFile << "  \"bPlusTreeResults\": [\n";
    for (size_t i = 0; i < bPlusTreeTop5.size(); ++i) {
        const auto& lapTime = bPlusTreeTop5[i];
        if (drivers.find(lapTime.driverId) != drivers.end()) {
            const auto& driver = drivers.at(lapTime.driverId);
            outFile << "    {\n";
            outFile << "      \"driver\": \"" << driver.forename + " " + driver.surname << "\",\n";
            outFile << "      \"time\": \"" << lapTime.time << "\"\n";
            outFile << "    }";
            if (i != bPlusTreeTop5.size() - 1) outFile << ",";
            outFile << "\n";
        }
    }
    outFile << "  ],\n";
    outFile << "  \"bPlusTreeRuntime\": " << bPlusTreeRuntime << "\n";
    outFile << "}\n";

    outFile.close();
}

// Main function
int main(int argc, char* argv[]) {
    if (argc != 3) {
        cerr << "Usage: main.exe <Race ID> <Lap Number>\n";
        return 1;
    }

    int selectedRaceId = stoi(argv[1]);
    int selectedLap = stoi(argv[2]);

    // File paths
    const string driversFile = "data/drivers.csv";
    const string lapTimesFile = "data/lap_times.csv";

    // Load drivers
    unordered_map<int, Driver> drivers;
    loadDrivers(driversFile, drivers);

    // Min-Heap approach
    MinHeap minHeap;
    auto startMinHeap = chrono::high_resolution_clock::now(); // Start timer for min heap
    ifstream file(lapTimesFile);
    string line;

    while (getline(file, line)) {
        if (line.empty() || line == "raceId,driverId,lap,position,time,milliseconds") {
            continue;
        }

        auto tokens = split(line, ',');
        if (tokens.size() != 6) {
            continue;
        }

        try {
            int raceId = stoi(tokens[0]);
            int driverId = stoi(tokens[1]);
            int lap = stoi(tokens[2]);
            int milliseconds = stoi(tokens[5]);

            if (raceId == selectedRaceId && lap == selectedLap) {
                string time = stripQuotes(tokens[4]);
                minHeap.insert({driverId, time, milliseconds});
            }
        } catch (...) {
            continue;
        }
    }

    vector<LapTime> top5MinHeap;
    for (int i = 0; i < 5 && !minHeap.isEmpty(); ++i) {
        top5MinHeap.push_back(minHeap.extractMin());
    }
    auto endMinHeap = chrono::high_resolution_clock::now();
    chrono::duration<double> durationMinHeap = endMinHeap - startMinHeap;

    // B+ Tree Approach
    BPlusTree bPlusTree;
    file.clear();
    file.seekg(0);

    auto startBPlusTree = chrono::high_resolution_clock::now();
    while (getline(file, line)) {
        if (line.empty() || line == "raceId,driverId,lap,position,time,milliseconds") {
            continue;
        }

        auto tokens = split(line, ',');
        if (tokens.size() != 6) {
            continue;
        }

        try {
            int raceId = stoi(tokens[0]);
            int driverId = stoi(tokens[1]);
            int lap = stoi(tokens[2]);
            int milliseconds = stoi(tokens[5]);

            if (raceId == selectedRaceId && lap == selectedLap) {
                string time = stripQuotes(tokens[4]);
                bPlusTree.insert({driverId, time, milliseconds});
            }
        } catch (...) {
            continue;
        }
    }

    vector<LapTime> bPlusTreeTop5 = bPlusTree.getTopK(5); // Retrieve top 5 lap times from B+ Tree
    auto endBPlusTree = chrono::high_resolution_clock::now();   // End timer for B+ Tree
    chrono::duration<double> durationBPlusTree = endBPlusTree - startBPlusTree; // Calculate duration

    // Display results
    displayTop5("Min-Heap Results", top5MinHeap, drivers);
    cout << "\nMin-Heap Build & Query Runtime: " << durationMinHeap.count() << " seconds\n";

    displayTop5("B+ Tree Results", bPlusTreeTop5, drivers);
    cout << "\nB+ Tree Build & Query Runtime: " << durationBPlusTree.count() << " seconds\n";

    // Write results
    writeResultsToJson("results.json", top5MinHeap, bPlusTreeTop5, drivers, durationMinHeap.count(), durationBPlusTree.count());

    return 0;
}
