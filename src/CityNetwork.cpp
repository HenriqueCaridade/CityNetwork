//
// Created by user2 on 29/05/2023.
//

#include "CityNetwork.h"
#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <queue>
#include <stack>

using namespace std;

CityNetwork::CityNetwork() : nodeCount(0), edgeCount(0), fakeEdgeCount(0) {}

CityNetwork::CityNetwork(const string &datasetPath, bool isDirectory) : nodeCount(0), edgeCount(0), fakeEdgeCount(0) {
    initializeData(datasetPath, isDirectory);
}

void CityNetwork::initializeData(const string &datasetPath, bool isDirectory) {
    clearData();
    if (isDirectory) { // Expect edges.csv and nodes.csv
        graphType = graphLatLon;
        initializeNodes(CSVReader::read(datasetPath + "nodes.csv"));
        initializeEdges(CSVReader::read(datasetPath + "edges.csv"));
    } else {
        CSV networkCSV = CSVReader::read(datasetPath);
        graphType = (networkCSV[0].size() == 5) ? graphLabeled : graphNormal;
        initializeNetwork(networkCSV);
    }
    completeEdges();
}

void CityNetwork::clearData() {
    nodes.clear();
    nodeCount = 0;
    edgeCount = 0;
    fakeEdgeCount = 0;
}

void CityNetwork::initializeNetwork(const CSV &networkCSV) {
    // From a single csv file.
    int skipFirstLine = isalpha(networkCSV[0][0][0]) ? 1 : 0;
    bool hasLabels = graphType == graphLabeled;
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
    }
    for (Node &node : nodes) { node.adj.resize(nodes.size()); }
    for (int i = skipFirstLine; i < networkCSV.size(); i++) {
        const CSVLine &line = networkCSV[i];
        addEdge(Edge(stoi(line[0]), stoi(line[1]), stod(line[2])));
    }
}

void CityNetwork::initializeNodes(const CSV &nodesCSV) {
    // From nodes.csv
    size_t nodesSize = nodesCSV.size() - 1;
    nodes.resize(nodesSize);
    for (int i = 1; i < nodesCSV.size(); i++) { // Skip first line
        const CSVLine &line = nodesCSV[i];
        if (line.size() != 3) throw std::invalid_argument("nodes.csv isn't formatted correctly!");
        Node n = Node(stoi(line[0]), stod(line[1]), stod(line[2]));
        n.adj.resize(nodesSize);
        addNode(n);
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

void CityNetwork::completeEdges() {
    for (Node &node : nodes) {
        if (node.id < 0) continue;
        for (int id = node.id + 1; id < nodes.size(); id++) {
            if (getNode(id).id < 0) continue;
            Edge &edge = node.adj[id];
            if (edge.origin == -1 or edge.dest == -1) { // Non-existent Edge
                if (graphType == graphLatLon) {
                    addEdge(Edge(node.id, id, node - getNode(id), false));
                } else {
                    addEdge(Edge(node.id, id, INFINITY, false));
                }
                fakeEdgeCount++;
            }
        }
    }
}

void CityNetwork::addNode(const Node &node) {
    if (nodes.size() <= node.id) nodes.resize(node.id + 1);
    nodeCount++;
    nodes.at(node.id) = node;
}

void CityNetwork::addEdge(const CityNetwork::Edge &edge) {
    if (nodes.size() <= edge.origin) throw std::out_of_range("There isn't a node " + to_string(edge.origin) + "!");
    if (nodes.size() <= edge.dest) throw std::out_of_range("There isn't a node " + to_string(edge.dest) + "!");
    edgeCount++;
    getNode(edge.origin).adj.at(edge.dest) = edge;
    getNode(edge.dest).adj.at(edge.origin) = edge.reverse();
}

vector<CityNetwork::Edge>& CityNetwork::getAdj(int nodeId) {
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

CityNetwork::Edge& CityNetwork::getEdge(int nodeId1, int nodeId2) {
    if (nodes.size() <= nodeId1) throw std::out_of_range("There isn't a node " + to_string(nodeId1) + "!");
    if (nodes.size() <= nodeId2) throw std::out_of_range("There isn't a node " + to_string(nodeId2) + "!");
    return getAdj(nodeId1).at(nodeId2);
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

void CityNetwork::clearPrevs() {
    for (Node& node : nodes) node.prev = -1;
}

int CityNetwork::getPrev(int nodeId) {
    return getNode(nodeId).prev;
}

void CityNetwork::setPrev(int nodeId, int prev) {
    getNode(nodeId).prev = prev;
}

void CityNetwork::clearUses() {
    for (Node &node : nodes) {
        for (Edge &edge : node.adj) {
            edge.used = false;
        }
    }
}

bool CityNetwork::isUsed(int originId, int destId) {
    return getEdge(originId, destId).used;
}

void CityNetwork::use(int originId, int destId) {
    getEdge(originId, destId).used = true;
    getEdge(destId, originId).used = true;
}

void CityNetwork::backtrackingHelper(int currentNodeId, Path currentPath, Path& bestPath) {
    if (currentPath.getPathSize() == nodeCount - 1) {
        Edge edge = getEdge(currentNodeId, 0);
        if (!edge.valid || !edge.real) return;
        currentPath.addToPath(edge);
        if (currentPath < bestPath) bestPath = currentPath;
        return;
    }
    for (const Edge& edge : getAdj(currentNodeId)){
        if (!edge.valid) continue;
        if (!isVisited(edge.dest) and edge.real) {
            visit(edge.dest);
            currentPath.addToPath(edge);
            backtrackingHelper(edge.dest, currentPath, bestPath);
            currentPath.removeLast();
            unvisit(edge.dest);
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

vector<int> CityNetwork::calcMST(int rootId) {
    clearPrevs();
    clearVisits();
    priority_queue<pair<double, pair<int, int>>, vector<pair<double, pair<int, int>>>, greater<>> pq;
    pq.emplace(0.0, pair<int,int>{rootId, -1});
    while (!pq.empty()) {
        auto [_, nodeIds] = pq.top(); pq.pop();
        auto [nodeId, prevId] = nodeIds;
        if (isVisited(nodeId)) continue;
        setPrev(nodeId, prevId);
        visit(nodeId);
        for (const Edge& edge : getAdj(nodeId)) {
            if (!edge.valid) continue;
            if (!isVisited(edge.dest) and edge.real) {
                pq.emplace(edge.dist, pair<int,int>{edge.dest, nodeId});
            }
        }
    }
    vector<int> mstPath;
    stack<int> toTraverse;
    toTraverse.push(rootId);
    while (!toTraverse.empty()){
        int nodeId = toTraverse.top();
        toTraverse.pop();
        mstPath.push_back(nodeId);
        const Node& node = getNode(nodeId);
        for (auto it = node.adj.rbegin(); it != node.adj.rend(); it++) {
            const Edge& edge = *it;
            if (!edge.valid) continue;
            if (getPrev(edge.dest) == nodeId) {
                toTraverse.push(edge.dest);
            }
        }
    }
    return mstPath;
}

CityNetwork::Path CityNetwork::triangularApproximation() {
    vector<int> mstPath = calcMST(0);
    Path path;
    for (int i = 0; i < mstPath.size(); i++)
        path.addToPath(getEdge(mstPath[i], mstPath[(i + 1) % mstPath.size()]));
    return path;
}

CityNetwork::Path CityNetwork::nearestNeighbor() {
    clearVisits();
    Path path;
    int currNodeId = 0;
    visit(currNodeId);
    while (path.getPathSize() < nodeCount - 1) {
        Edge minEdge;
        for (Edge &edge: getAdj(currNodeId)) {
            if (!edge.valid) continue;
            if (!isVisited(edge.dest) && edge.dist < minEdge.dist) minEdge = edge;
        }
        if (!minEdge.valid) return Path({}, INFINITY);
        path.addToPath(minEdge);
        currNodeId = minEdge.dest;
        visit(currNodeId);
    }
    path.addToPath(getEdge(currNodeId, 0));
    return path;
}

class GreaterEdge {
public:
    bool operator()(const CityNetwork::Edge& edge1, const CityNetwork::Edge& edge2) {
        return edge2 < edge1;
    }
};

CityNetwork::Path CityNetwork::greedyAlgorithm() {
    clearUses();
    clearVisits();
    // The amount of edges attached to a node.
    vector<pair<int,int>> nodeEdges = vector(nodes.size(), pair<int,int>{0, -1});
    int nodesFinished = 0;
    priority_queue<Edge, vector<Edge>, GreaterEdge> pq;
    for (Node &node : nodes) {
        if (node.id < 0) continue;
        for (int nodeId = node.id + 1; nodeId < nodes.size(); nodeId++) {
            if (getNode(nodeId).id < 0) continue;
            Edge edge = getEdge(node.id, nodeId);
            if (edge.valid) pq.push(edge);
        }
    }
    while (nodesFinished != nodeCount) { // Last 2 nodes to connect.
        Edge edge = pq.top(); pq.pop();
        if (nodeEdges[edge.origin].first == 2) continue;
        if (nodeEdges[edge.dest].first == 2) continue;
        if (nodeEdges[edge.origin].first == 1 && nodeEdges[edge.dest].first == 1) {
            // Verify if it doesn't finish the cycle too early
            if (nodesFinished != nodeCount - 2 && nodeEdges[edge.origin].second == nodeEdges[edge.dest].second) continue; // Cycle
            int prevId = nodeEdges[edge.dest].second;
            for (auto &[count, cycleId]: nodeEdges) {
                if (cycleId == prevId) {
                    cycleId = nodeEdges[edge.origin].second; // Update to the new cycle id
                }
            }
            nodesFinished += 2;
        } else if (nodeEdges[edge.origin].first == 1) {
            nodeEdges[edge.dest].second = nodeEdges[edge.origin].second;
            nodesFinished++;
        } else if (nodeEdges[edge.dest].first == 1) {
            nodeEdges[edge.origin].second = nodeEdges[edge.dest].second;
            nodesFinished++;
        } else {
            nodeEdges[edge.origin].second = edge.origin;
            nodeEdges[edge.dest].second = edge.origin;
        }
        use(edge.origin, edge.dest);
        nodeEdges[edge.origin].first++;
        nodeEdges[edge.dest].first++;
    }
    Path path;
    int currId = 0;
    visit(currId);
    while (path.getPathSize() < nodeCount - 1) {
        for (Edge &edge : getAdj(currId)) {
            if (!edge.valid) continue;
            if (!isVisited(edge.dest) && isUsed(currId, edge.dest)) {
                path.addToPath(edge);
                currId = edge.dest;
                visit(currId);
                break;
            }
        }
    }
    path.addToPath(getEdge(path.back(), path.front()));
    return path;
}

ostream &operator<<(ostream &os, const CityNetwork &cityNet) {
    os << "Nodes: " << cityNet.nodeCount << '\n'
       << "Edge Count: " << cityNet.edgeCount;
    if (cityNet.fakeEdgeCount > 0)
        os << "\nAdded Fake Edges: " << cityNet.fakeEdgeCount;
    os << flush;
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
