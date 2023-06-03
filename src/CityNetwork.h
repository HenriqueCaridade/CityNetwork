//
// Created by user2 on 29/05/2023.
//

#ifndef CITYNETWORK_CITYNETWORK_H
#define CITYNETWORK_CITYNETWORK_H

#include <list>
#include <string>
#include <utility>
#include <vector>
#include <cmath>
#include "CSVReader.h"


class CityNetwork {
    enum GraphType {
        graphNormal,
        graphLabeled,
        graphLatLon,
    };
    GraphType graphType;
public:
    struct Edge {
        int origin;
        int dest;
        double dist;
        bool real;
        bool valid;
        Edge() : origin(-1), dest(-1), dist(INFINITY), real(false), valid(false) {}
        Edge(int origin, int dest, double dist, bool real = true, bool valid = true) :
            origin(origin), dest(dest), dist(dist), real(real), valid(valid) {}
        [[nodiscard]] Edge reverse() const { return {dest, origin, dist, real}; }
    };

    struct Node {
        int id;
        std::vector<Edge> adj;
        std::string label;
        double lat;
        double lon;
        int prev = -1;
        bool visited = false;
        Node() : id(-1), lat(INFINITY), lon(INFINITY) {};
        explicit Node(int id, std::vector<Edge> adj = {}) :
            id(id), adj(std::move(adj)), lat(INFINITY), lon(INFINITY) {}
        Node(int id, std::string label, std::vector<Edge> adj = {}) :
            id(id), label(std::move(label)), adj(std::move(adj)), lat(INFINITY), lon(INFINITY) {}
        Node(int id, double lat, double lon, std::vector<Edge> adj = {}) :
            id(id), lat(lat), lon(lon), adj(std::move(adj)) {}

        constexpr double operator-(const Node& other) const {
            // Haversine Formula
            if (other.lat == INFINITY || other.lon == INFINITY) return INFINITY; // Invalid.
            if (this->lat == INFINITY || this->lon == INFINITY) return INFINITY; // Invalid.
            const double radLat1 = this->lat * M_PI / 180;
            const double radLat2 = other.lat * M_PI / 180;
            const double deltaLat = radLat2 - radLat1;
            const double deltaLon = (other.lon - this->lon) * M_PI / 180;
            const double sinLat = sin(deltaLat / 2);
            const double sinLon = sin(deltaLon / 2);
            double aux = sinLat * sinLat + cos(radLat1) * cos(radLat2) * sinLon * sinLon;
            aux = 2.0 * atan2 (sqrt(aux), sqrt(1.0 - aux));
            return 6371000 * aux;
        }
    };

    class Path {
        std::list<Edge> path;
        double distance;
    public:
        explicit Path(std::list<Edge> path = {}, double distance = 0.0) :
            path(std::move(path)), distance(distance) {}
        [[nodiscard]] const std::list<Edge>& getPath() const { return path; }
        [[nodiscard]] double getDistance() const { return distance; }
        [[nodiscard]] size_t getPathSize() const { return path.size(); }
        [[nodiscard]] bool isValid() const { return distance != INFINITY; }
        void addToPath(Edge edge) {
            path.push_back(edge);
            distance += edge.dist;
        }
        void removeLast() {
            distance -= path.back().dist;
            path.pop_back();
        }
        bool operator<(const Path& pathObj) const {
            return distance < pathObj.distance;
        }
    };
private:
    std::vector<Node> nodes;
    unsigned int nodeCount;
    unsigned long long edgeCount;
    unsigned long long fakeEdgeCount;

    void initializeEdges(const CSV& edgesCSV);
    void initializeNodes(const CSV& nodesCSV);
    void initializeNetwork(const CSV& networkCSV);
    void clearData();
    void addNode(const Node &node);
    void addEdge(const Edge &edge);
    bool nodeExists(int nodeId);
    Node& getNode(int nodeId);
    std::vector<Edge> getAdj(int nodeId);
    Edge getEdge(int nodeId1, int nodeId2);
    void clearVisits();
    bool isVisited(int nodeId);
    void visit(int nodeId);
    void unvisit(int nodeId);
    void clearPrevs();
    int getPrev(int nodeId);
    void setPrev(int nodeId, int prev);
    void unsetPrev(int nodeId);
    void backtrackingHelper(int currNodeId, Path currentPath, Path& bestPath);
    std::vector<int> calcMST(int rootId);
    void completeEdges();
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

    Path backtracking();

    Path triangularApproxHeuristic();

    Path pureGreedyAlgorithm();

    friend std::ostream& operator<<(std::ostream& os, const CityNetwork& cityNet);

};

std::ostream &operator<<(std::ostream &os, const CityNetwork::Path &cityPath);

#endif //CITYNETWORK_CITYNETWORK_H
