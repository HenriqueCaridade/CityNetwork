#include <string>
#include <fstream>
#include <sstream>

#include "CSVReader.h"

CSV CSVReader::read(const std::string& file) {
    std::ifstream in(file);
    CSV out;
    std::string line;
    while (std::getline(in, line)) {
        std::istringstream iss(line);
        CSVLine csvLine;
        std::string str;
        while (std::getline(iss, str, ',')) {
            csvLine.push_back(str);
        }
        out.push_back(csvLine);
    }
    return out;
}
