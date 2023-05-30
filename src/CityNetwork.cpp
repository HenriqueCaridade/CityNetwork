//
// Created by user2 on 29/05/2023.
//

#include "CityNetwork.h"
#include <stdexcept>
#include <iostream>

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
    Node n1 = getNode(nodeId1);
    for(Edge& e : n1.adj ) {
        if(e.dest == nodeId2)
            return e;
    }
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

void CityNetwork::backtrackingHelper(int currNodeId, path currentPath, path& bestPath) {
    Node& currNode = getNode(currNodeId);
    if (currentPath.first.size() == nodes.size() - 1) {
        try {
            Edge e = getEdge(currNodeId, 0);
            currentPath.second = currentPath.second + e.dist;
            currentPath.first.push_back(e);
            if(bestPath.second > currentPath.second )
                bestPath = currentPath;
            return;
        }
        catch (out_of_range& exception) {
            return;
        }
    }
    for(Edge e: currNode.adj){
        Node& n = getNode(e.dest);
        if(!n.visited) {
            n.visited = true;
            path newPath = currentPath;
            newPath.first.push_back(e);
            newPath.second = currentPath.second + e.dist;
            backtrackingHelper(n.id, newPath, bestPath);
            n.visited = false;
        }
    }
}

CityNetwork::path CityNetwork::backtracking() {
    clearVisits();
    path bestPath = {{},INFINITY};
    visit(0);
    backtrackingHelper(0, {{}, 0.0}, bestPath);
    return bestPath;
}

CityNetwork::path CityNetwork::triangularAproxHeuristic() {
    clearVisits();
    path currentPath = {{}, 0.0};
    path bestPath = {{}, INFINITY};

    // Start from node 0
    visit(0);

    int currentNodeId = 0;
    Node& currentNode = getNode(currentNodeId);

    while (currentPath.first.size() < nodes.size() - 1) {
        double shortestDistance = numeric_limits<double>::max();
        int nextNodeId = -1;

        for (const Edge& edge : currentNode.adj) {
            Node& neighborNode = getNode(edge.dest);
            if (!isVisited(neighborNode.id) && edge.dist < shortestDistance) {
                shortestDistance = edge.dist;
                nextNodeId = neighborNode.id;
            }
        }

        if (nextNodeId == -1) {
            // If there are no unvisited neighbors, return to node 0
            Edge returnEdge = getEdge(currentNodeId, 0);
            currentPath.second += returnEdge.dist;
            currentPath.first.push_back(returnEdge);
            bestPath = currentPath;
            break;
        }

        Edge nextEdge = getEdge(currentNodeId, nextNodeId);
        currentPath.second += nextEdge.dist;
        currentPath.first.push_back(nextEdge);
        visit(nextNodeId);
        currentNodeId = nextNodeId;
        currentNode = getNode(currentNodeId);
    }

    return bestPath;
}

std::ostream &operator<<(ostream &os, const CityNetwork &cityNet) {
    os << "Nodes: " << cityNet.nodes.size() << '\n'
       << "Edge Count: " << cityNet.edgeCount << flush;
    return os;
}

std::ostream &operator<<(ostream &os, const CityNetwork::path &cityPath) {
    if(cityPath.second == INFINITY )
        os <<"Invalid Path";
    else{
        os << "Path:\n";
        for (const CityNetwork::Edge& e : cityPath.first) os << e.origin << " -> " << e.dest << " [" << e.dist << "]\n";
        os <<"Total distance: "<< cityPath.second << flush;
    }
    return os;
}



