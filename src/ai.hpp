#pragma once
#include "init.hpp"

namespace ai
{
struct Ai
{
  void init_Vertexs(Block maze[][MAZE_SIDE_LENGTH]);
  void shortest_path(MapPoint source, MapPoint destination, std::list<MapPoint> *path);
  void a_star_search(Vertex *start, Vertex *goal); // Renamed from breadth_first_search to a_star_search
  std::map<MapPoint, Vertex> graph;
  void initContainsPacman(int x, int y);
  int heuristic_estimate(const Vertex* start, const Vertex* goal);

private:
  void init_edges(Block maze[][MAZE_SIDE_LENGTH]);
  void init_edge(MapPoint origin, MapPoint new_connection);
  void init_vertex(int x, int y);
  void check_edges(int x, int y);
};
}; // namespace ai
