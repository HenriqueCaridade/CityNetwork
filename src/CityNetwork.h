//
// Created by user2 on 29/05/2023.
//

#ifndef CITYNETWORK_CITYNETWORK_H
#define CITYNETWORK_CITYNETWORK_H

#include <list>
#include <string>
#include <unordered_map>
#include "CSVReader.h"

class CityNetwork {
    struct Edge {
        std::string origin;
        std::string dest;
        double dist;

        Edge(std::string origin, std::string dest, double dist) :
            origin(std::move(origin)), dest(std::move(dest)), dist(dist){}
    };

    struct Node {
        std::string name;
        std::list<Edge> adj;

        Node(std::string name, std::list<Edge> adj) :
            name(std::move(name)), adj(std::move(adj)) {}
    };
    std::unordered_map<std::string, Node> nodes;

    void initializeEdges(const CSV& networkCSV);
    void initializeNodes(const CSV& stationsCSV);
    void initializeNetwork(const CSV& networkCSV);
    void clearData();
public:
    /**
     * @brief Default constructor.
     *
     * Creates a new empty city network manager.
     */
    CityNetwork();
    /**
     * @brief Constructs a new CityNetwork object and initializes it with data on the datasetPath.
     * @param datasetPath The path to the directory containing the CSV files.
     */
    explicit CityNetwork(const std::string& datasetPath, bool isDirectory);
    /**
     * @brief Initializes the CityNetwork object with data from a CSV file.
     * @param datasetPath The path to the directory containing the CSV files.
     */
    void initializeData(const std::string& datasetPath, bool isDirectory);
};


#endif //CITYNETWORK_CITYNETWORK_H
