//
// Created by user2 on 29/05/2023.
//

#include "CityNetwork.h"
#include <stdexcept>
#include <iostream>
#include <iomanip>

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
    getNode(edge.dest).adj.emplace_back(edge.dest, edge.origin, edge.dist);
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

CityNetwork::Edge CityNetwork::getEdge(int nodeId1, int nodeId2) {
    for(const Edge& e : getAdj(nodeId1))
        if(e.dest == nodeId2) return e;
    throw std::out_of_range("There are connections between the two nodes");
}

void CityNetwork::clearVisits() {
    for (Node& node : nodes) node.visited = false;
}

bool CityNetwork::isVisited(int nodeId) {
    return getNode(nodeId).visited;
}

void CityNetwork::visit(int nodeId) {
    getNode(nodeId).visited = true;
}

void CityNetwork::unvisit(int nodeId) {
    getNode(nodeId).visited = false;
}

void CityNetwork::backtrackingHelper(int currentNodeId, Path currentPath, Path& bestPath) {
    if (currentPath.getPathSize() == nodes.size() - 1) {
        Edge edge;
        try {
            edge = getEdge(currentNodeId, 0);
        } catch (out_of_range& exception) {
            return; // No edge found from currNodeId to 0.
        }
        currentPath.addToPath(edge);
        if (currentPath < bestPath) bestPath = currentPath;
        return;
    }
    for (const Edge& e : getAdj(currentNodeId)){
        if (!isVisited(e.dest)) {
            visit(e.dest);
            currentPath.addToPath(e);
            backtrackingHelper(e.dest, currentPath, bestPath);
            currentPath.removeLast();
            unvisit(e.dest);
        }
    }
}

CityNetwork::Path CityNetwork::backtracking() {
    clearVisits();
    visit(0);
    Path bestPath = Path({}, INFINITY);
    backtrackingHelper(0, Path(), bestPath);
    return bestPath;
}

CityNetwork::Path CityNetwork::triangularApproxHeuristic() {
    clearVisits();
    int currentNodeId = 0; // Start and end at node with zero-identifier label
    Path currentPath = Path();
    visit(currentNodeId);

    while (currentPath.getPathSize() < nodes.size() - 1) {
        Edge minEdge;
        for (const Edge& edge : getAdj(currentNodeId)) {
            if (!isVisited(edge.dest) && edge.dist < minEdge.dist) {
                minEdge = edge;
            }
        }

        if (minEdge.dest >= 0) {
            currentPath.addToPath(minEdge);
            currentNodeId = minEdge.dest;
            visit(currentNodeId);
        } else {
            // No cycle possible => return invalid Path.
            return Path({}, INFINITY);
        }
    }

    try {
        // Add edge from the last visited node back to the starting node
        Edge lastEdge = getEdge(currentNodeId, 0);
        currentPath.addToPath(lastEdge);
    } catch (std::out_of_range& error) {
        // No cycle possible => return invalid Path.
        return Path({}, INFINITY);
    }

    return currentPath;
}

ostream &operator<<(ostream &os, const CityNetwork &cityNet) {
    os << "Nodes: " << cityNet.nodes.size() << '\n'
       << "Edge Count: " << cityNet.edgeCount << flush;
    return os;
}

ostream &operator<<(ostream &os, const CityNetwork::Path &cityPath) {
    if (!cityPath.isValid()) os << "Invalid Path";
    else {
        os << "Path:\n";
        for (const CityNetwork::Edge& e : cityPath.getPath()) {
            string origin = to_string(e.origin); origin.append(max((int) (4 - origin.size()), (int) 0), ' ');
            string dest = to_string(e.dest); dest.append(max((int) (4 - dest.size()), (int) 0), ' ');
            os << origin << " -> " << dest << " [" << fixed << setprecision(2) << e.dist << "]\n";
        }
        os << "Total distance: " << fixed << setprecision(2) << cityPath.getDistance() << flush;
    }
    return os;
}



