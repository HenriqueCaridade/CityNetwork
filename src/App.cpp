
#include <exception>
#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <filesystem>
#include <chrono>
#include <thread>
#include <future>

#include "App.h"
using namespace std;

static void clear_screen() {
#ifdef _WIN32
    std::system("cls");
#else
    std::system("clear");
#endif
}

App::App() = default;


void App::initializeData() {
    cityNet.initializeData(datasetPathFull, filesystem::is_directory(datasetPathFull));
}


string App::getDoubleString(const string& question, const string& invalidMessage, bool (*isValid)(double)) {
    auto isDouble = [](const string& str){
        char* end = nullptr;
        double val = strtod(str.c_str(), &end);
        return end != str.c_str() && *end == '\0' && val != HUGE_VAL;
    };
    while (true){
        cout << vertical << ' ' << question << flush;
        string aux;
        cin >> aux;
        cout << getBottomLine() << endl;
        if(aux == "x" || (isDouble(aux) && isValid(stod(aux)))) return aux;
        cout << vertical << ' ' << invalidMessage << endl;
    }
}


template<class T>
T App::getInput(const string& question, const string& invalidMessage, const unordered_set<T>& validOptions) {
    while (true){
        cout << vertical << ' ' << question << flush;
        T aux;
        cin >> aux;
        cout << getBottomLine() << endl;
        if(validOptions.empty() || (validOptions.find(aux) != validOptions.end())) return aux;
        cout << vertical << ' ' << invalidMessage << endl;
    }
}
string App::getLine(const string& question, const string& invalidMessage, const unordered_set<string>& validOptions) {
    while (true){
        cout << vertical << ' ' << question << flush;
        string aux;
        getline(cin, aux);
        cout << getBottomLine() << endl;
        if(validOptions.empty() || (validOptions.find(aux) != validOptions.end())) return aux;
        cout << vertical << ' ' << invalidMessage << endl;
    }
}

string App::fillLine(const string& str) {
    return str + string(titleSize - str.size() - 1, ' ') + vertical;
}

string App::getTitle(const string& title){
    int aux = titleSize - (int) title.size();
    if(aux < 4) throw std::exception();
    aux = (aux - 2) / 2;
    string out = corner + string(aux, horizontal) + ' ' + title + ' ';
    out += string(titleSize - out.size() - 1, horizontal) + corner;
    return out;
}

string App::getMenuLine(const string& str){
    return fillLine(vertical + (' ' + str));
}

string App::getBottomLine(){
    return (corner + string(titleSize - 2, horizontal) + corner);
}

string App::getOption(const vector<string>& options, int index){
    if(index >= options.size()) return vertical + string(titleSize - 2, ' ') + vertical;
    return getMenuLine(options[index]);
}

void App::drawMenu(const string& title, const vector<string>& options){
    cout << "\n" << getTitle(title) << string(spaceBetween, ' ') << '\n';
    for(int i = 0; i < options.size(); i++)
        cout << getOption(options, i) << string(spaceBetween, ' ') << '\n';
    cout << getBottomLine() << string(spaceBetween, ' ') << endl;
}

template <typename Lambda>
void App::runMenu(const string &title, const vector<pair<char, string>> &options, Lambda f, bool clearFirst, bool clearLast) {
    if (clearFirst) clear_screen();
    bool running = true;
    vector<string> optionsText;
    unordered_set<char> optionsChar;
    for(const auto& p : options){
        optionsChar.insert(p.first);
        optionsText.push_back(p.first + string(" - ") + p.second);
    }
    while(running){
        drawMenu(title, optionsText);
        char choice = getInput("Choice:", "Invalid Choice. Try Again.", optionsChar);
        running = f(choice);
    }
    if (clearLast) clear_screen();
}


void App::start(){
    dataSelectionMenu();
    mainMenu();
}

void App::mainMenu() {
    // Print Data Selection Results
    runMenu("City Manager", {
            {'1', "Backtracking Algorithm"},
            {'2', "Triangular Approximation Heuristic"},
            {'3', "Purely Greedy Algorithm"},
            {'d', "Data Selection"},
            {'x', "Exit App"}
    }, [this](char choice) -> bool {
        switch(choice){
            case '1': {
                cout << "Backtracking Algorithm Solution Loading..." << endl;
                auto start = chrono::high_resolution_clock::now();
                CityNetwork::Path backtrackingPath = cityNet.backtracking();
                auto duration = chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now() - start);
                if (backtrackingPath.isValid()) {
                    cout << "Distance: " << fixed << setprecision(2) << backtrackingPath.getDistance() << endl;
                } else {
                    cout << "No path found!" << endl;
                }
                cout << "Time spent: " << fixed << setprecision(3) << ((double) duration.count() / 1000) << "s" << endl;
            } break;
            case '2': {
                cout << "Triangular Approximation Heuristic Solution Loading..." << endl;
                auto start = chrono::high_resolution_clock::now();
                CityNetwork::Path triangularApproxPath = cityNet.triangularApproxHeuristic();
                auto duration = chrono::duration_cast<chrono::microseconds>(chrono::high_resolution_clock::now() - start);
                if (triangularApproxPath.isValid()) {
                    cout << "Distance: " << fixed << setprecision(2) << triangularApproxPath.getDistance() << endl;
                } else {
                    cout << "No path found!" << endl;
                }
                cout << "Time spent: " << fixed << setprecision(6) << ((double) duration.count() / 1000000)  << "s" << endl;
            } break;
            case '3': {
                cout << "Purely Greedy Algorithm Solution Loading..." << endl;
                auto start = chrono::high_resolution_clock::now();
                CityNetwork::Path greedyPath = cityNet.pureGreedyAlgorithm();
                auto duration = chrono::duration_cast<chrono::microseconds>(chrono::high_resolution_clock::now() - start);
                if (greedyPath.isValid()) {
                    cout << "Distance: " << fixed << setprecision(2) << greedyPath.getDistance() << endl;
                } else {
                    cout << "No path found!" << endl;
                }
                cout << "Time spent: " << fixed << setprecision(6) << ((double) duration.count() / 1000000)  << "s" << endl;
            } break;
            case 'd': dataSelectionMenu(); return true;
            case 'x': return false;
        }
        return true;
    }, false, false);
}

// =================== //
// DATA SELECTION MENU //
// =================== //

void App::getAll(const string& outFile, bool fullPaths) {
    const string projectPath = filesystem::current_path().parent_path().string() + '\\';
    ofstream out(projectPath + outFile);
    CityNetwork cityNetwork;
    out << "Triangular Approximation Heuristic Solutions:\n" << endl;
    for (const char *str : {"graphs-toy/shipping.csv", "graphs-toy/stadiums.csv", "graphs-toy/tourism.csv", "graphs-extra/edges_25.csv", "graphs-extra/edges_50.csv", "graphs-extra/edges_75.csv", "graphs-extra/edges_100.csv", "graphs-extra/edges_200.csv", "graphs-extra/edges_300.csv", "graphs-extra/edges_400.csv", "graphs-extra/edges_500.csv", "graphs-extra/edges_600.csv", "graphs-extra/edges_700.csv", "graphs-extra/edges_800.csv", "graphs-extra/edges_900.csv", "graphs-real/graph1/", "graphs-real/graph2/", "graphs-real/graph3/"}) {
        out << str << " Graph Initialization...\n";
        string fullPath = projectPath + str;
        cout << "Calculating " << str << endl;
        auto start = chrono::high_resolution_clock::now();
        cityNetwork.initializeData(fullPath, filesystem::is_directory(fullPath));
        auto duration = chrono::duration_cast<chrono::microseconds>(chrono::high_resolution_clock::now() - start);
        out << cityNetwork << endl;
        out << "Initialization time: " << ((double) duration.count() / 1000000)  << "s" << endl;
        out << str << " Data:\n" << endl;
        out << "Triangular Approximation Heuristic:" << endl;
        start = chrono::high_resolution_clock::now();
        CityNetwork::Path triangularApproxPath = cityNetwork.triangularApproxHeuristic();
        duration = chrono::duration_cast<chrono::microseconds>(chrono::high_resolution_clock::now() - start);
        if (fullPaths) out << triangularApproxPath << '\n';
        else out << "Distance: " << fixed << setprecision(2) << triangularApproxPath.getDistance() << '\n';
        out << "Time spent: " << fixed << setprecision(6) << ((double) duration.count() / 1000000)  << "s\n" << endl;
        out << "Greedy Algorithm:" << endl;
        start = chrono::high_resolution_clock::now();
        CityNetwork::Path greedyPath = cityNetwork.pureGreedyAlgorithm();
        duration = chrono::duration_cast<chrono::microseconds>(chrono::high_resolution_clock::now() - start);
        if (fullPaths) out << greedyPath << '\n';
        else out << "Distance: " << fixed << setprecision(2) << greedyPath.getDistance() << '\n';
        out << "Time spent: " << fixed << setprecision(6) << ((double) duration.count() / 1000000)  << "s\n" << endl;
    }
}

void App::dataSelectionMenu() {
    const string title = "Data Selection";
    clear_screen();
    bool running = true;
    const string projectPath = filesystem::current_path().parent_path().string() + '\\';
    while (running) {
        string text = string("Current Path: ") + projectPath;
        cout << "\n" << getTitle(title) << string(spaceBetween, ' ') << '\n'
             << vertical << ' ' << text << '\n' << getBottomLine() << endl;
        bool calc = false;
        string pathChosen, pathChosenFull;
        while (true){
            pathChosen = pathChosenFull = "";
            cout << vertical << " Path:" << flush;
            cin >> pathChosen;
            if (pathChosen ==  "$ALL") {
                // Get All data to a file. (Without Paths)
                getAll("all_output.txt");
                clear_screen();
                cout << "Calculated All and saved to all_output.txt" << endl;
                calc = true;
                break;
            }
            if (pathChosen == "$ALLP") {
                // Get All data to a file. (With Paths)
                getAll("all_output.txt", true);
                clear_screen();
                cout << "Calculated All and saved to all_output.txt" << endl;
                calc = true;
                break;
            }
            pathChosenFull = projectPath + pathChosen;
            for (char& c : pathChosenFull) if (c == '/') c = '\\';
            if (filesystem::is_directory(pathChosenFull)) {
                char c = pathChosenFull[pathChosen.size() - 1];
                if (c != '\\') pathChosenFull += '\\';
            }
            cout << getBottomLine() << endl;
            if (filesystem::exists(pathChosenFull)) break;
            cout << vertical << " Path Doesn't Exist. Try Again." << endl;
        }
        if (calc) continue;
        bool allGood = true;
        if (filesystem::is_directory(pathChosenFull)){
            if (!filesystem::exists(pathChosenFull + "edges.csv")) {
                allGood = false;
                cout << vertical << " edges.csv not found in folder given!" << endl;
            }
            if (!filesystem::exists(pathChosenFull + "nodes.csv")){
                allGood = false;
                cout << vertical << " nodes.csv not found in folder given!" << endl;
            }
        } else if (filesystem::path(pathChosenFull).extension().string() != ".csv") {
            allGood = false;
            cout << vertical << ' ' << pathChosenFull << endl;
            cout << vertical << " File given is not an csv file! (is " << filesystem::path(pathChosenFull).extension().string() << ')' << endl;
        }

        if (allGood) {
            datasetPath = pathChosen;
            datasetPathFull = pathChosenFull;
            cout << vertical << " Loading..." << endl;
            try {
                initializeData();
                running = false;
            } catch (std::invalid_argument& error) {
                cout << vertical << ' ' << error.what() << '\n' << getBottomLine() << endl;
            }
        } else cout << vertical << " Path doesn't contain the necessary files.\n" << getBottomLine() << endl;
    }
    clear_screen();
    cout << ("Loaded From " + datasetPath + ":\n") << cityNet << endl;
}
