#pragma once
#include "init.hpp"
namespace physics
{
struct Physics
{
  void move_ghosts_2(std::list<Ghost> *ghosts);
  void verify_collision(Player *pacman, std::map<MapPoint, Vertex> graph, Block maze[][MAZE_SIDE_LENGTH]);
  int pacman_score(Player *pacman, Block maze[][MAZE_SIDE_LENGTH], int score);
  void move_pacman(Player *pacman);
  void win_condition(Player *player, Block maze[][MAZE_SIDE_LENGTH]);
  void defeat_condition(Player *player, std::list<Ghost> *ghosts);
  double distance2d(Point p1, Point p2);
  int ghostChangedNode(Ghost *ghost);
  void initPhy(int x, int y);
  MapPoint get_pacman_position(void);
  int pacmanIsMoving(void);

private:
  int block_contains_pacman(int x, int y, Player pacman);
  int block_contains_ghost(int x, int y, Ghost *ghost);
  int pacmanChangedNode(Player *pacman);
  int number_of_rows;
  MapPoint pacmanPosition, previousBlock;
  bool moving;
};
}; // namespace physics
