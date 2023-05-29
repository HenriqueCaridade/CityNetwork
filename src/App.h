/**

@file App.h
@brief App class header file. Contains declaration of App class and its member functions.
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
 *
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
    std::string datasetPath;
    CityNetwork cityNet;

    /**
     * @brief Gets a Floating Point input from the user.
     * @param const std::string& question - The question to display to the user.
     * @param const std::string& invalidMessage - The message to display if user inputs an invalid value.
     * @param bool (*isValid)(double) - Pointer to a function that takes a double and returns a bool indicating if the input is valid.
     * @return std::string - The user's input as a string.
     */
    static std::string getDoubleString(const std::string& question, const std::string& invalidMessage, bool (*isValid)(double));
    /**
     * @brief Gets a General input from the user.
     * @tparam T - The type of input to get.
     * @param const std::string& question - The question to display to the user.
     * @param const std::string& invalidMessage - The message to display if user inputs an invalid value.
     * @param const std::unordered_set<T>& validOptions - The set of valid options.
     * @return T - The user's input.
     */
    template <class T> static T getInput(const std::string& question, const std::string& invalidMessage, const std::unordered_set<T>& validOptions);
    /**
     * @brief Gets a Multi Word input from the user.
     * @param const std::string& question - The question to display to the user.
     * @param const std::string& invalidMessage - The message to display if user inputs an invalid value.
     * @param const std::unordered_setstd::string& validOptions - The set of valid options.
     * @return std::string - The user's input as a string.
     */
    static std::string getLine(const std::string& question, const std::string& invalidMessage, const std::unordered_set<std::string>& validOptions);
    /**
     * @brief Fills the line with the correct size.
     * @param const std::string& str - The string to be filled.
     * @return std::string - The filled string.
     */
    static std::string fillLine(const std::string& str);
    /**
     * @brief Pads string with the necessary characters to complete the menu line format.
     * @param const std::string& str - The string to be padded.
     * @return std::string - The padded string.
     */
    static std::string getMenuLine(const std::string& str);
    /**
     * Centers the string with the title line format.
     * @param const std::string& title
     * @return std::string
     */
    static std::string getTitle(const std::string& title);
    /**
     * Returns the bottom line string.
     * @return std::string
     */
    static std::string getBottomLine();
    /**
     * Returns the correct string to build the menu.
     * @param const std::vector<std::string>& options
     * @param int index
     * @return std::string
     */
    static std::string getOption(const std::vector<std::string>& options, int index);
    /**
     * Draws the menu on the console with title and options.
     * @param const std::string& title
     * @param const std::vector<std::string>& options
     */
    static void drawMenu(const std::string& title, const std::vector<std::string>& options);
    /**
     * Main Menu. (Calls runMenu)
     */
    void mainMenu();
    /**
     * Data Selection Menu.
     */
    void dataSelectionMenu();
    template <typename Lambda>
    /**
     * Runs a menu with the given title, image, options and for every valid option calls f(option) to process choice.
     * @tparam Lambda
     * @param const std::string& title
     * @param const std::vector<std::pair<char, std::string>>& options
     * @param Lambda f
     */
    void runMenu(const std::string& title, const std::vector<std::pair<char, std::string>>& options, Lambda f, bool clearFirst = true, bool clearLast = true);

    void initializeData();
public:
    /**
     * Default constructor.
     */
    App();
    /**
     * Starts app. (Calls mainMenu()).
     */
    void start();
};


#endif //CITYNETWORK_APP_H