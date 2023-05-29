
#include <string>
#include <fstream>
#include <sstream>

#include "CSVReader.h"

using namespace std;

CSV CSVReader::read(const string& file){
    ifstream in (file);
    CSV out;
    string line;
    while(getline(in, line)){
        istringstream iss (line);
        CSVLine csvLine;
        string str;
        while(getline(iss, str, ',')) {
            size_t pos = str.find('"');
            if (pos != string::npos){
                size_t pos2 = str.find('"', pos + 1);
                string aux;
                if (pos2 != string::npos) {
                    // No commas inside the quote marks
                    str = str.substr(pos + 1, pos2 - pos - 1);
                } else {
                    // Commas inside the quote marks
                    getline(iss, aux, '"');
                    str.push_back(','); // Put back the comma
                    str.append(aux);
                    str = str.substr(pos + 1);
                }
                getline(iss, aux, ','); // Consume until ,
            }
            csvLine.push_back(str);
        }
        out.push_back(csvLine);
    }
    return out;
}
