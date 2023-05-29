//
// Created by user2 on 29/05/2023.
//

#include "CityNetwork.h"

CityNetwork::CityNetwork() = default;

CityNetwork::CityNetwork(const std::string &datasetPath, bool isDirectory) {
    initializeData(datasetPath, isDirectory);
}

void CityNetwork::initializeData(const std::string &datasetPath, bool isDirectory) {
    clearData();
    if (isDirectory) { // Expect edges.csv and nodes.csv
        initializeNodes(CSVReader::read(datasetPath + "nodes.csv"));
        initializeEdges(CSVReader::read(datasetPath + "edges.csv"));
    } else {
        initializeNetwork(CSVReader::read(datasetPath));
    }
}

void CityNetwork::clearData() {
    nodes.clear();
}

void CityNetwork::initializeNetwork(const CSV &networkCSV) {
    // TODO: initializeNetwork
}

void CityNetwork::initializeNodes(const CSV &stationsCSV) {
    // TODO: initializeNodes
}

void CityNetwork::initializeEdges(const CSV &networkCSV) {
    // TODO: initializeEdges
}

