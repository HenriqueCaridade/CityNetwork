/**
 * @file App.h
 * @brief App class header file. Contains declaration of App class and its member functions.
 */

#ifndef CITYNETWORK_APP_H
#define CITYNETWORK_APP_H

#include <string>
#include <vector>
#include <list>
#include <unordered_set>
#include "CityNetwork.h"

/**
 * @class App
 * @brief Represents the application (I/O).
 *
 * App takes care of the input and output of the program, it has functions to get multiple different ways to get input and has different ways of outputting information.
 */
class App {
    const static int titleSize = 65;
    const static int spaceBetween = 2;
    const static char corner = 'o';
    const static char vertical = '|';
    const static char horizontal = '-';
    std::string datasetPathFull;
    std::string datasetPath;
    CityNetwork cityNet;
    long long progress;

    /**
     * @brief Gets a Floating Point input from the user.
     * @param question The question to display to the user.
     * @param invalidMessage The message to display if the user inputs an invalid value.
     * @param isValid Pointer to a function that takes a double and returns a bool indicating if the input is valid.
     * @return The user's input as a string.
     * @details The time complexity of this function is O(1).
     */
    static std::string getDoubleString(const std::string& question, const std::string& invalidMessage, bool (*isValid)(double));

    /**
     * @brief Gets a General input from the user.
     * @tparam T The type of input to get.
     * @param question The question to display to the user.
     * @param invalidMessage The message to display if the user inputs an invalid value.
     * @param validOptions The set of valid options.
     * @return The user's input.
     * @details The time complexity of this function is O(1).
     */
    template <class T>
    static T getInput(const std::string& question, const std::string& invalidMessage, const std::unordered_set<T>& validOptions);

    /**
     * @brief Gets a Multi Word input from the user.
     * @param question The question to display to the user.
     * @param invalidMessage The message to display if the user inputs an invalid value.
     * @param validOptions The set of valid options.
     * @return The user's input as a string.
     * @details The time complexity of this function is O(1).
     */
    static std::string getLine(const std::string& question, const std::string& invalidMessage, const std::unordered_set<std::string>& validOptions);

    /**
     * @brief Fills the line with the correct size.
     * @param str The string to be filled.
     * @return The filled string.
     * @details The time complexity of this function is O(1).
     */
    static std::string fillLine(const std::string& str);

    /**
     * @brief Pads string with the necessary characters to complete the menu line format.
     * @param str The string to be padded.
     * @return The padded string.
     * @details The time complexity of this function is O(1).
     */
    static std::string getMenuLine(const std::string& str);

    /**
     * @brief Centers the string with the title line format.
     * @param title The string to be centered.
     * @return The centered string.
     * @details The time complexity of this function is O(1).
     */
    static std::string getTitle(const std::string& title);

    /**
     * @brief Returns the bottom line string.
     * @return The bottom line string.
     * @details The time complexity of this function is O(1).
     */
    static std::string getBottomLine();

    /**
     * @brief Returns the correct string to build the menu.
     * @param options The vector of options.
     * @param index The index of the option.
     * @return The string for the specified option.
     * @details The time complexity of this function is O(1).
     */
    static std::string getOption(const std::vector<std::string>& options, int index);

    /**
     * @brief Draws the menu on the console with title and options.
     * @param title The title of the menu.
     * @param options The vector of options.
     * @details The time complexity of this function is O(n), where n is the number of options.
     */
    static void drawMenu(const std::string& title, const std::vector<std::string>& options);

    /**
     * @brief Main Menu. (Calls runMenu)
     * @details The time complexity of this function depends on the complexity of the functions called inside the lambda functions.
     */
    void mainMenu();

    /**
     * @brief Data Selection Menu.
     * @details The time complexity of this function depends on the complexity of the functions called inside the lambda functions.
     */
    void dataSelectionMenu();

    /**
     * @brief Runs a menu with the given title, image, options, and processes the choice for every valid option.
     * @tparam Lambda The type of the lambda function.
     * @param title The title of the menu.
     * @param options The vector of options.
     * @param f The lambda function to process the choice.
     * @param clearFirst Flag indicating whether to clear the console before displaying the menu.
     * @param clearLast Flag indicating whether to clear the console after processing the choice.
     * @details The time complexity of this function depends on the complexity of the lambda function passed.
     */
    template <typename Lambda>
    void runMenu(const std::string& title, const std::vector<std::pair<char, std::string>>& options, Lambda f, bool clearFirst = true, bool clearLast = true);
    /**
     * @brief Initializes the data.
     * @details The time complexity of this function depends on the complexity of the initializeData function in the cityNet object.
     */
    void initializeData();
    /**
     * @brief Runs the heuristic algorithms for all testing graphs.
     * @param outFile The filename of the file where the output is going to go.
     * @param fullPaths Flag indicating if it is to output the complete paths found.
     */
    static void getAll(const std::string& outFile, bool fullPaths = false);
public:
    /**
     * @brief Default constructor.
     * @details The time complexity of this function is O(1).
     */
    App();

    /**
     * @brief Starts the app. (Calls mainMenu()).
     * @details The time complexity of this function is O(1).
     */
    void start();
};

#endif // CITYNETWORK_APP_H