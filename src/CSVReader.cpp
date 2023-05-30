
#include <string>
#include <fstream>
#include <sstream>

#include "CSVReader.h"
using namespace std;

CSV CSVReader::read(const string& file) {
    ifstream in(file);
    CSV out;
    string line;
    while (getline(in, line)) {
        istringstream iss(line);
        CSVLine csvLine;
        string str;
        while (getline(iss, str, ',')) {
            csvLine.push_back(str);
        }
        out.push_back(csvLine);
    }
    return out;
}
