#ifndef CITYNETWORK_CITYNETWORK_H
#define CITYNETWORK_CITYNETWORK_H

#include <list>
#include <string>
#include <utility>
#include <vector>
#include <cmath>
#include "CSVReader.h"

/**
 * @class CityNetwork
 * @brief Represents a city network with nodes and edges.
 *
 * The CityNetwork class manages a city network consisting of nodes and edges.
 * It provides various operations and algorithms for working with the city network.
 */
class CityNetwork {
    enum GraphType {
        graphNormal,
        graphLabeled,
        graphLatLon,
    };
    GraphType graphType;
public:
    /**
     * @struct Edge
     * @brief Represents an edge between two nodes in the city network.
     *
     * The Edge struct represents an edge between two nodes in the city network.
     * It stores the origin node, destination node, and the distance between them.
     */
    struct Edge {
        int origin; /**< The ID of the origin node. */
        int dest; /**< The ID of the destination node. */
        double dist; /**< The distance between the origin and destination nodes. */
        bool real; /**< Flag indicating if the edge is real, meaning it was given by the file when initializing. */
        bool valid; /**< Flag indicating if the edge is valid. */
        Edge() : origin(-1), dest(-1), dist(INFINITY), real(false), valid(false) {}
        Edge(int origin, int dest, double dist, bool real = true, bool valid = true) :
            origin(origin), dest(dest), dist(dist), real(real), valid(valid) {}
        [[nodiscard]] Edge reverse() const { return {dest, origin, dist, real}; }
    };

    /**
     * @struct Node
     * @brief Represents a node in the city network.
     *
     * The Node struct represents a node in the city network.
     * It stores the ID of the node, a list of adjacent edges, label, latitude, longitude, and visited flag.
     */
    struct Node {
        int id; /**< The ID of the node. */
        std::vector<Edge> adj; /**< The vector of adjacent edges. */
        std::string label; /**< The label of the node. */
        double lat; /**< The latitude of the node. */
        double lon; /**< The longitude of the node. */
        int prev = -1; /**< Value used to calculate MST. (Previous Node ID) */
        bool visited = false; /**< Flag indicating if the node has been visited. */
        /**
          * @brief Default constructor.
          *
          * Constructs an uninitialized node.
          */
        Node() : id(-1), lat(INFINITY), lon(INFINITY) {};
        /**
         * @brief Constructs a node with the given ID and the adjacent nodes (empty if none given).
         * @param id The ID of the node.
         * @param adj The vector of adjacent edges.
         */
        explicit Node(int id, std::vector<Edge> adj = {}) :
            id(id), adj(std::move(adj)), lat(INFINITY), lon(INFINITY) {}
        /**
         * @brief Constructs a node with the given ID, label and the adjacent nodes (empty if none given).
         * @param id The ID of the node.
         * @param label The label of the node.
         * @param adj The vector of adjacent edges.
         */
        Node(int id, std::string label, std::vector<Edge> adj = {}) :
            id(id), label(std::move(label)), adj(std::move(adj)), lat(INFINITY), lon(INFINITY) {}
        /**
         * @brief Constructs a node with the given ID, latitude, longitude and the adjacent nodes (empty if none given).
         * @param id The ID of the node.
         * @param lat The latitude of the node.
         * @param lon The longitude of the node.
         * @param adj The vector of adjacent edges.
         */
        Node(int id, double lat, double lon, std::vector<Edge> adj = {}) :
            id(id), lat(lat), lon(lon), adj(std::move(adj)) {}

        /* TODO DOCUMENTATION */
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

    /**
     * @class Path
     * @brief Represents a path in the city network.
     *
     * The Path class represents a path in the city network.
     * It stores a list of edges that form the path and the total distance of the path.
     */
    class Path {
        std::list<Edge> path; /**< The list of edges forming the path. */
        double distance; /**< The total distance of the path. */

    public:
        /**
         * @brief Default constructor.
         *
         * Constructs an empty path with zero distance.
         */
        explicit Path(std::list<Edge> path = {}, double distance = 0.0) :
            path(std::move(path)), distance(distance) {}
        /**
         * @brief Get the list of edges forming the path.
         * @return The list of edges forming the path.
         */
        [[nodiscard]] const std::list<Edge>& getPath() const { return path; }
        /**
        * @brief Get the total distance of the path.
        * @return The total distance of the path.
        */
        [[nodiscard]] double getDistance() const { return distance; }
        /**
         * @brief Get the number of edges in the path.
         * @return The number of edges in the path.
         */
        [[nodiscard]] size_t getPathSize() const { return path.size(); }
        /**
         * @brief Check if the path is valid (i.e. has a finite distance).
         * @return True if the path is valid, false otherwise.
         */
        [[nodiscard]] bool isValid() const { return distance != INFINITY; }
        /**
         * @brief Add an edge to the path and update the total distance.
         * @param edge The edge to add to the path.
         */
        void addToPath(Edge edge) {
            path.push_back(edge);
            distance += edge.dist;
        }
        /**
         * @brief Remove the last edge from the path and update the total distance.
         */
        void removeLast() {
            distance -= path.back().dist;
            path.pop_back();
        }
        /**
         * @brief Compare two paths based on their total distance.
         * @param pathObj The path object to compare with.
         * @return True if this path has a smaller distance than the other path, false otherwise.
         */
        bool operator<(const Path& pathObj) const {
            return distance < pathObj.distance;
        }
    };

private:
    std::vector<Node> nodes; /**< The list of nodes in the city network. */
    unsigned int nodeCount; /**< The total number of nodes in the city network. */
    unsigned long long edgeCount; /**< The total number of edges in the city network. */
    unsigned long long fakeEdgeCount; /**< The number of fake edges (i.e. edges that were not given by the user's file) in the city network. */

    /**
     * @brief Initialize the edges of the city network from a CSV file.
     * @param edgesCSV The CSV object containing edge data.
     */
    void initializeEdges(const CSV& edgesCSV);

    /**
     * @brief Initialize the nodes of the city network from a CSV file.
     * @param nodesCSV The CSV object containing node data.
     */
    void initializeNodes(const CSV& nodesCSV);

    /**
     * @brief Initialize the city network from CSV files.
     * @param networkCSV The CSV object containing network data.
     */
    void initializeNetwork(const CSV& networkCSV);

    /**
     * @brief Clear the data of the city network.
     */
    void clearData();
    /**
     * @brief Add a node to the city network.
     * @param node The node to add.
     */
    void addNode(const Node &node);
    /**
     * @brief Add an edge to the city network.
     * @param edge The edge to add.
     */
    void addEdge(const Edge &edge);
    /**
     * @brief Get a reference to a node in the city network.
     * @param nodeId The ID of the node.
     * @return A reference to the node.
     */
    Node& getNode(int nodeId);
    /**
     * @brief Get the adjacent edges of a node.
     * @param nodeId The ID of the node.
     * @return The vector of adjacent edges.
     */
    std::vector<Edge> getAdj(int nodeId);
    /**
     * @brief Get the edge between two nodes.
     * @param nodeId1 The ID of the first node.
     * @param nodeId2 The ID of the second node.
     * @return The edge between the two nodes.
     */
    Edge getEdge(int nodeId1, int nodeId2);
    /**
     * @brief Check if a node exists in the city network.
     * @param nodeId The ID of the node.
     * @return True if the node exists, false otherwise.
     */
    bool nodeExists(int nodeId);
    /**
     * @brief Clear the visited flag of all nodes in the city network.
     */
    void clearVisits();
    /**
     * @brief Check if a node has been visited.
     * @param nodeId The ID of the node.
     * @return True if the node has been visited, false otherwise.
     */
    bool isVisited(int nodeId);
    /**
     * @brief Mark a node as visited.
     * @param nodeId The ID of the node.
     */
    void visit(int nodeId);
    /**
     * @brief Mark a node as unvisited.
     * @param nodeId The ID of the node.
     */
    void unvisit(int nodeId);

    /* TODO DOCUMENTATION */
    void clearPrevs();
    int getPrev(int nodeId);
    void setPrev(int nodeId, int prev);
    void unsetPrev(int nodeId);
    std::vector<int> calcMST(int rootId);
    void completeEdges();

    /**
     * @brief Recursive helper function for the backtracking algorithm.
     * @param currNodeId The ID of the current node.
     * @param currentPath The current path being explored.
     * @param bestPath The best path found so far.
     */
    void backtrackingHelper(int currNodeId, Path currentPath, Path& bestPath);
public:
    /**
     * @brief Default constructor.
     *
     * Creates a new empty city network manager.
     */
    CityNetwork();

    /**
     * @brief Constructs a new CityNetwork object and initializes it with data from the datasetPath.
     * @param datasetPath The path to the directory containing the CSV files.
     * @param isDirectory Flag indicating if the datasetPath is a directory.
     */
    explicit CityNetwork(const std::string& datasetPath, bool isDirectory);

    /**
     * @brief Initializes the CityNetwork object with data from a CSV file or directory of CSV files.
     * @param datasetPath The path to the directory containing the CSV files or the path to a single CSV file.
     * @param isDirectory Flag indicating if the datasetPath is a directory.
     */
    void initializeData(const std::string& datasetPath, bool isDirectory);

    /**
     * @brief Perform the backtracking algorithm to find the shortest path in the city network.
     * @return The shortest path found by the backtracking algorithm.
     *
     * The time complexity of the backtracking algorithm is O((V - 1)!).
     */
    Path backtracking();
    /**
     * @brief Perform the triangular approximation heuristic algorithm to find an approximate shortest path in the city network.
     * @return An approximate shortest path found by the triangular approximation heuristic algorithm.
     *
     * The time complexity of the triangular approximation heuristic algorithm is O(E*log(V)).
     */
    Path triangularApproxHeuristic();

    /* TODO DOCUMENTATION */
    Path pureGreedyAlgorithm();

    /* TODO DOCUMENTATION */
    friend std::ostream& operator<<(std::ostream& os, const CityNetwork& cityNet);
};

/**
 * @brief Overload the stream insertion operator to print a CityNetwork::Path object.
 * @param os The output stream.
 * @param cityPath The CityNetwork::Path object to print.
 * @return The output stream.
 */
std::ostream& operator<<(std::ostream& os, const CityNetwork::Path& cityPath);

#endif //CITYNETWORK_CITYNETWORK_H
