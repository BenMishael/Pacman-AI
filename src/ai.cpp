#include "ai.hpp"
#include <list>
#include <map>
#include <queue>

// Since MapPoint is used as a key in the Map, it needs a comparison operator
inline bool operator<(const MapPoint &a, const MapPoint &b)
{
    return std::tie(a.x, a.y) < std::tie(b.x, b.y);
}

namespace ai
{

void Ai::initContainsPacman(int x, int y)
{
    MapPoint pacmanLocation = {x, y};
    graph[pacmanLocation].containsPacman = true;
}

void Ai::init_vertex(int x, int y)
{
    MapPoint vertex = {x, y};
    if (graph.find(vertex) == graph.end()) {
        graph[vertex] = Vertex();
        graph[vertex].g_cost = INT_MAX;  // Initialize with maximum cost
        graph[vertex].h_cost = 0;
        graph[vertex].f_cost = INT_MAX;
        graph[vertex].prev = nullptr;  // Initialize prev to nullptr
        graph[vertex].key = vertex;
    }
}

void Ai::init_Vertexs(Block maze[][MAZE_SIDE_LENGTH])
{
    for (int y = 0; y < MAZE_SIDE_LENGTH; y++)
        for (int x = 0; x < MAZE_SIDE_WIDTH; x++)
        {
            if (maze[x][y].type == BlockTypes::path)
                init_vertex(x, y);

            else if (maze[x][y].type == BlockTypes::intersection)
                init_vertex(x, y);
            else if (maze[x][y].type == BlockTypes::portal)
                init_vertex(x, y);
        }
    init_edges(maze);
}

void Ai::init_edge(MapPoint origin, MapPoint new_connection)
{
    graph[origin].edges.push_back(&(graph[new_connection]));
}

void Ai::check_edges(int x, int y)
{
    MapPoint center = {x, y},
               up = {x, y + 1},
               down = {x, y - 1},
               left = {x - 1, y},
               right = {x + 1, y};

    auto it_up = graph.find(up),
         it_down = graph.find(down),
         it_left = graph.find(left),
         it_right = graph.find(right),
         it_end = graph.end();

    if (it_up != it_end)
        init_edge(center, up);

    if (it_down != it_end)
        init_edge(center, down);

    if (it_right != it_end)
        init_edge(center, right);

    if (it_left != it_end)
        init_edge(center, left);
}

void Ai::init_edges(Block maze[][MAZE_SIDE_LENGTH])
{
    MapPoint current;
    for (int y = 0; y < MAZE_SIDE_LENGTH; y++)
        for (int x = 0; x < MAZE_SIDE_WIDTH; x++)
        {
            current = {x, y};
            auto it = graph.find(current);
            if (it != graph.end())
                check_edges(x, y);
        }
}

// A* search algorithm
void Ai::a_star_search(Vertex *start, Vertex *goal)
{
    // Resetting vertex states might be necessary here
    for (auto& pair : graph) {
        pair.second.g_cost = INT_MAX;
        pair.second.f_cost = INT_MAX;
        pair.second.prev = nullptr;
    }

    start->g_cost = 0;
    start->h_cost = heuristic_estimate(start, goal); // Corrected function name to heuristic_estimate
    start->f_cost = start->g_cost + start->h_cost;

    std::priority_queue<std::pair<int, Vertex*>, std::vector<std::pair<int, Vertex*>>, std::greater<std::pair<int, Vertex*>>> frontier;
    frontier.push({start->f_cost, start});

    while (!frontier.empty()) {
        Vertex *current = frontier.top().second;
        frontier.pop();

        if (current == goal) break;

        for (Vertex* next : current->edges) {
            int new_cost = current->g_cost + 1;  // Assuming cost between nodes is 1
            if (new_cost < next->g_cost) {
                next->prev = current;
                next->g_cost = new_cost;
                next->h_cost = heuristic_estimate(next, goal);
                next->f_cost = next->g_cost + next->h_cost;
                frontier.push({next->f_cost, next});
            }
        }
    }
}

void Ai::shortest_path(MapPoint source, MapPoint destination, std::list<MapPoint> *path) {
    Vertex *s = &(graph[source]);
    Vertex *v = &(graph[destination]);

    if (v->prev == nullptr) {
        std::cerr << "No path exists from (" << s->key.x << "," << s->key.y << ") to (" << v->key.x << "," << v->key.y << ")\n";
        return;
    }

    // Recursively reconstruct the path
    for (Vertex *vert = v; vert != nullptr; vert = vert->prev) {
        path->push_front(vert->key);
    }
}

int ai::Ai::heuristic_estimate(const Vertex* start, const Vertex* goal) {
    return std::abs(start->key.x - goal->key.x) + std::abs(start->key.y - goal->key.y);
}

}; // namespace ai
