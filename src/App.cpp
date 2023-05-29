
#include <exception>
#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <filesystem>

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
    cityNet.initializeData(datasetPath, filesystem::is_directory(datasetPath));
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
    runMenu("City Manager", {
            {'1', "Backtracking Algorithm"},
            {'2', "Triangular Approximation Heuristic"},
            {'3', "Other Heuristics"},
            {'d', "Data Selection"},
            {'x', "Exit App"}
    }, [this](char choice) -> bool {
        switch(choice){
            case '1': /* TODO */ break;
            case '2': /* TODO */ break;
            case '3': /* TODO */ break;
            case 'd': dataSelectionMenu(); return true;
            case 'x': return false;
        }
        clear_screen();
        return true;
    }, false);
}

// =================== //
// DATA SELECTION MENU //
// =================== //

void App::dataSelectionMenu() {
    const string title = "Data Selection";
    clear_screen();
    bool running = true;
    while (running) {
        const string projectPath = filesystem::current_path().parent_path().string() + '\\';
        string text = string("Current Path: ") + projectPath;
        cout << "\n" << getTitle(title) << string(spaceBetween, ' ') << '\n'
             << vertical << ' ' << text << '\n' << getBottomLine() << endl;

        string pathChosen;
        while (true){
            pathChosen = "";
            cout << vertical << " Path:" << flush;
            cin >> pathChosen;
            if (pathChosen == "x") { running = false; break; }
            pathChosen = projectPath + pathChosen;
            for (char& c : pathChosen) if (c == '/') c = '\\';
            if (filesystem::is_directory(pathChosen)) {
                char c = pathChosen[pathChosen.size() - 1];
                if (c != '\\') pathChosen += '\\';
            }
            cout << getBottomLine() << endl;
            if (filesystem::exists(pathChosen)) break;
            cout << vertical << " Path Doesn't Exist. Try Again. (x to Leave)" << endl;
        }
        if (!running) break;
        bool allGood = true;
        if (filesystem::is_directory(pathChosen)){
            if (!filesystem::exists(pathChosen + "edges.csv")) {
                allGood = false;
                cout << vertical << ' ' << pathChosen + "edges.csv not found in folder!" << endl;
            }
            if (!filesystem::exists(pathChosen + "nodes.csv")){
                allGood = false;
                cout << vertical << ' ' << pathChosen + "nodes.csv not found in folder!" << endl;
            }
        } else if (filesystem::path(pathChosen).extension().string() != ".csv") {
            allGood = false;
            cout << vertical << ' ' << pathChosen << endl;
            cout << vertical << " File given is not an csv file! (is " << filesystem::path(pathChosen).extension().string() << ')' << endl;
        }

        if (allGood) {
            datasetPath = pathChosen;
            running = false;
        } else cout << vertical << " Path doesn't contain the necessary files.\n" << getBottomLine() << endl;
    }
    clear_screen();
    initializeData();
}
