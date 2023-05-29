//
// Created by user2 on 29/05/2023.
//

#include "CityNetwork.h"
#include <stdexcept>
#include <ostream>
using namespace std;

CityNetwork::CityNetwork() : edgeCount(0) {}

CityNetwork::CityNetwork(const string &datasetPath, bool isDirectory) : edgeCount(0) {
    initializeData(datasetPath, isDirectory);
}

void CityNetwork::initializeData(const string &datasetPath, bool isDirectory) {
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
    edgeCount = 0;
}

void CityNetwork::initializeNetwork(const CSV &networkCSV) {
    // From a single csv file.
    int skipFirstLine = isalpha(networkCSV[0][0][0]) ? 1 : 0;
    bool hasLabels = networkCSV[0].size() == 5;
    for (int i = skipFirstLine; i < networkCSV.size(); i++) {
        const CSVLine &line = networkCSV[i];
        if (line.size() != (hasLabels ? 5 : 3)) throw std::invalid_argument("File given isn't formatted correctly!");
        int originId = stoi(line[0]);
        int destId = stoi(line[1]);
        if (!nodeExists(originId)) {
            if (hasLabels) addNode(Node(originId, line[3]));
            else addNode(Node(originId));
        }
        if (!nodeExists(destId)) {
            if (hasLabels) addNode(Node(destId, line[4]));
            else addNode(Node(destId));
        }
        addEdge(Edge(originId, destId, stoi(line[2])));
    }
}

void CityNetwork::initializeNodes(const CSV &nodesCSV) {
    // From nodes.csv
    for (int i = 1; i < nodesCSV.size(); i++) { // Skip first line
        const CSVLine &line = nodesCSV[i];
        if (line.size() != 3) throw std::invalid_argument("nodes.csv isn't formatted correctly!");
        addNode(Node(stoi(line[0]), stod(line[1]), stod(line[2])));
    }
}

void CityNetwork::initializeEdges(const CSV &edgesCSV) {
    // From edges.csv
    for (int i = 1; i < edgesCSV.size(); i++) { // Skip first line
        const CSVLine &line = edgesCSV[i];
        if (line.size() != 3) throw std::invalid_argument("edges.csv isn't formatted correctly!");
        addEdge(Edge(stoi(line[0]), stoi(line[1]), stod(line[2])));
    }
}

void CityNetwork::addNode(const Node &node) {
    if (nodes.size() <= node.id) nodes.resize(node.id + 1);
    nodes.at(node.id) = node;
}

void CityNetwork::addEdge(const CityNetwork::Edge &edge) {
    if (nodes.size() <= edge.origin) throw std::out_of_range("There isn't a node " + to_string(edge.origin) + "!");
    if (nodes.size() <= edge.dest) throw std::out_of_range("There isn't a node " + to_string(edge.dest) + "!");
    edgeCount++;
    getNode(edge.origin).adj.push_back(edge);
    getNode(edge.dest).adj.push_back(edge);
}

list<CityNetwork::Edge> CityNetwork::getAdj(int nodeId) {
    if (nodes.size() <= nodeId) throw std::out_of_range("There isn't a node " + to_string(nodeId) + "!");
    return getNode(nodeId).adj;
}

bool CityNetwork::nodeExists(int nodeId) {
    if (nodes.size() <= nodeId) return false;
    return getNode(nodeId).id >= 0;
}

CityNetwork::Node& CityNetwork::getNode(int nodeId) {
    return nodes.at(nodeId);
}

std::ostream &operator<<(ostream &os, const CityNetwork &cityNet) {
    os << "Nodes: " << cityNet.nodes.size() << '\n'
       << "Edge Count: " << cityNet.edgeCount << flush;
    return os;
}

