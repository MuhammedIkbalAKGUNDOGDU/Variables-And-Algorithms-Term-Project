#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NODES 100
#define MAX_EDGES 100

// Struct representing an edge in the graph
typedef struct Edge {
    char start;  // Starting vertex of the edge
    char end;    // Ending vertex of the edge
    int weight;  // Weight of the edge
} Edge;

// Struct representing a node in the adjacency list of the graph
typedef struct Node {
    char vertex;       // Vertex identifier
    struct Node* next; // Pointer to the next node in the list
    int weight;        // Weight of the edge to this node
} Node;

// Struct representing the graph
typedef struct Graph {
    Node* adjList[MAX_NODES]; // Array of adjacency lists for each vertex
    int numVertices;          // Number of vertices in the graph
    char vertices[MAX_NODES]; // Array of vertex identifiers
} Graph;

// Struct representing a cycle in the graph
typedef struct Cycle {
    char nodes[MAX_NODES]; // Array of vertices forming the cycle
    int length;            // Length of the cycle
    int perimeter;         // Perimeter (total weight) of the cycle
} Cycle;

int edgeIndex = 0;       // Index to track the number of edges
Edge edges[MAX_EDGES];   // Array to store all edges of the graph
int numCycles = 0;       // Counter for the number of cycles found
Cycle cycles[MAX_EDGES]; // Array to store the cycles found

/*
@brief Creates a graph with the given number of vertices.

@param vertices Number of vertices in the graph

@return Pointer to the created graph
*/
Graph* createGraph(int vertices) {
    Graph* graph = (Graph*)malloc(sizeof(Graph)); // Allocate memory for the graph
    graph->numVertices = vertices; // Set the number of vertices
    for (int i = 0; i < vertices; i++) {
        graph->adjList[i] = NULL; // Initialize adjacency list for each vertex
    }
    return graph; // Return the created graph
}

/*
@brief Creates a new node with the given vertex and weight.

@param vertex Vertex identifier
@param weight Weight of the edge to this vertex

@return Pointer to the created node
*/
Node* createNode(char vertex, int weight) {
    Node* newNode = (Node*)malloc(sizeof(Node)); // Allocate memory for the node
    newNode->vertex = vertex; // Set the vertex
    newNode->weight = weight; // Set the weight
    newNode->next = NULL; // Initialize next pointer to NULL
    return newNode; // Return the created node
}

/*
@brief Adds an edge to the graph.

@param graph Pointer to the graph
@param start Starting vertex of the edge
@param end Ending vertex of the edge
@param weight Weight of the edge
*/
void addEdge(Graph* graph, char start, char end, int weight) {
    int i;
    // Find the index of the starting vertex in the vertices array
    for (i = 0; i < graph->numVertices; i++) {
        if (graph->vertices[i] == start) {
            break;
        }
    }
    // Create a new node for the end vertex and add it to the adjacency list of the start vertex
    Node* newNode = createNode(end, weight);
    newNode->next = graph->adjList[i];
    graph->adjList[i] = newNode;

    // Find the index of the ending vertex in the vertices array
    for (i = 0; i < graph->numVertices; i++) {
        if (graph->vertices[i] == end) {
            break;
        }
    }
    // Create a new node for the start vertex and add it to the adjacency list of the end vertex
    newNode = createNode(start, weight);
    newNode->next = graph->adjList[i];
    graph->adjList[i] = newNode;
}

/*
@brief Reads the graph from a file and adds the edges to the graph.

@param filename Name of the file containing the graph edges
@param graph Pointer to the graph
*/
void readGraphFromFile(char* filename, Graph* graph) {
    FILE* file = fopen(filename, "r"); // Open the file for reading
    if (!file) { // Check if the file was opened successfully
        printf("Error opening file.\n");
        exit(1);
    }

    char start, end;
    int weight;
    // Read the edges from the file and add them to the graph
    while (fscanf(file, " %c %c %d", &start, &end, &weight) == 3) {
        edges[edgeIndex].start = start; // Store the start vertex of the edge
        edges[edgeIndex].end = end; // Store the end vertex of the edge
        edges[edgeIndex].weight = weight; // Store the weight of the edge
        edgeIndex++; // Increment the edge index
        addEdge(graph, start, end, weight); // Add the edge to the graph
    }

    fclose(file); // Close the file
}

/*
@brief Checks if a vertex is visited in the current path.

@param path Array representing the current path
@param length Length of the path
@param vertex Vertex to check

@return 1 if the vertex is visited, 0 otherwise
*/
int isVisited(char* path, int length, char vertex) {
    for (int i = 0; i < length; i++) {
        if (path[i] == vertex) {
            return 1; // Vertex is visited
        }
    }
    return 0; // Vertex is not visited
}

/*
@brief Checks if a cycle is unique.

@param nodes Array representing the nodes in the cycle
@param length Length of the cycle
@param perimeter Perimeter (total weight) of the cycle

@return 1 if the cycle is unique, 0 otherwise
*/
int isCycleUnique(char* nodes, int length, int perimeter) {
    for (int i = 0; i < numCycles; i++) {
        if (cycles[i].length == length && cycles[i].perimeter == perimeter) {
            int match = 1;
            for (int j = 0; j < length; j++) {
                if (!isVisited(cycles[i].nodes, length, nodes[j])) {
                    match = 0; // Cycle is not unique
                    break;
                }
            }
            if (match) {
                return 0; // Cycle is not unique
            }
        }
    }
    return 1; // Cycle is unique
}

/*
@brief Performs depth-first search to find cycles in the graph.

@param graph Pointer to the graph
@param start Starting vertex of the search
@param path Array representing the current path
@param length Length of the current path
@param perimeter Perimeter (total weight) of the current path
*/
void dfs(Graph* graph, char start, char* path, int length, int perimeter) {
    if (length > 2 && start == path[0]) { // Check if a cycle is formed
        if (isCycleUnique(path, length, perimeter)) { // Check if the cycle is unique
            Cycle newCycle;
            memcpy(newCycle.nodes, path, length); // Copy the nodes of the cycle
            newCycle.length = length; // Set the length of the cycle
            newCycle.perimeter = perimeter; // Set the perimeter of the cycle
            cycles[numCycles++] = newCycle; // Store the cycle
        }
        return;
    }

    int i;
    // Find the index of the start vertex in the vertices array
    for (i = 0; i < graph->numVertices; i++) {
        if (graph->vertices[i] == start) {
            break;
        }
    }

    Node* adjList = graph->adjList[i];
    while (adjList) {
        // Continue the search if the vertex is not visited or if a cycle can be closed
        if (!isVisited(path, length, adjList->vertex) || (length > 2 && adjList->vertex == path[0])) {
            path[length] = adjList->vertex; // Add the vertex to the path
            dfs(graph, adjList->vertex, path, length + 1, perimeter + adjList->weight); // Recur for the next vertex
        }
        adjList = adjList->next; // Move to the next adjacent vertex
    }
}

/*
@brief Finds all cycles in the graph.

@param graph Pointer to the graph
*/
void findCycles(Graph* graph) {
    char path[MAX_NODES];
    for (int i = 0; i < graph->numVertices; i++) {
        path[0] = graph->vertices[i]; // Start a new path from each vertex
        dfs(graph, graph->vertices[i], path, 1, 0); // Perform DFS to find cycles
    }
}

/*
@brief Prints the found cycles and their counts.
*/
void printCycles() {
    int triangleCount = 0, quadrilateralCount = 0, pentagonCount = 0;
    for (int i = 0; i < numCycles; i++) {
        // Count the cycles based on their lengths
        if (cycles[i].length == 4) triangleCount++;
        else if (cycles[i].length == 5) quadrilateralCount++;
        else if (cycles[i].length == 6) pentagonCount++;
    }

    // Print the counts of the cycles
    printf("Sekil Sayisi: %d\n", numCycles);
    printf("3'gen Sayisi: %d\n", triangleCount);
    printf("4'gen Sayisi: %d\n", quadrilateralCount);
    printf("5'gen Sayisi: %d\n", pentagonCount);

    int triangleIndex = 1, quadrilateralIndex = 1, pentagonIndex = 1;
    for (int i = 0; i < numCycles; i++) {
        // Print each cycle based on its length
        if (cycles[i].length == 4) {
            printf("%d. 3'gen: ", triangleIndex++);
            for (int j = 0; j < cycles[i].length; j++) {
                printf("%c ", cycles[i].nodes[j]);
            }
            printf("Uzunluk: %d\n", cycles[i].perimeter);
        } else if (cycles[i].length == 5) {
            printf("%d. 4'gen: ", quadrilateralIndex++);
            for (int j = 0; j < cycles[i].length; j++) {
                printf("%c ", cycles[i].nodes[j]);
            }
            printf("Uzunluk: %d\n", cycles[i].perimeter);
        } else if (cycles[i].length == 6) {
            printf("%d. 5'gen: ", pentagonIndex++);
            for (int j = 0; j < cycles[i].length; j++) {
                printf("%c ", cycles[i].nodes[j]);
            }
            printf("Uzunluk: %d\n", cycles[i].perimeter);
        }
    }
}

int main() {
    Graph* graph = createGraph(7); // Create a graph with 7 vertices
    // Initialize the vertices of the graph
    graph->vertices[0] = 'A';
    graph->vertices[1] = 'B';
    graph->vertices[2] = 'C';
    graph->vertices[3] = 'D';
    graph->vertices[4] = 'E';
    graph->vertices[5] = 'F';
    graph->vertices[6] = 'G';

    readGraphFromFile("sample.txt", graph); // Read the graph from a file
    findCycles(graph); // Find cycles in the graph
    printCycles(); // Print the found cycles

    return 0;
}
