#pragma once
#define SCREEN_WIDTH 1470
#define SCREEN_LENGTH 1890
#define MAZE_SIDE_LENGTH 111
#define MAZE_SIDE_WIDTH 87
#define MAZE_WALL_WIDTH 45
#define MAZE_WALL_LENGTH 45
#define BORDER 90
#define MAX_NUM_OF_BONUS 36
#define PACMAN_RADIUS 21
#define PACMAN_SPRITES 15

#include "screen.hpp"

using namespace screen;
using namespace geom;
#include <iostream>
#include <list>
#include <map>

#define BACKGROUND_LOSS_FILE "img/background_loss.png"
#define BACKGROUND_WIN_FILE "img/background_win.png"
#define BACKGROUND_FILE "img/background.gif"
#define BLOCK_FILE "img/block.png"
#define PACMAN_FILE "img/pacman.bmp"
#define MAZE_FILE "data/maze.txt"

enum State
{
    menu,
    nothing,
    score,
    playing,
    init,
    win,
    end,
    defeat,
    pause_state
};

enum BlockTypes
{
    path,
    intersection,
    wall,
    portal,
};

enum GraphColor
{
    WHITE,
    GRAY,
    BLACK
};

struct Block
{
    BlockTypes type;
    bool hasPoint;
    bool hasBonus;
};

struct Player
{
    Circle cir;
    State state;
    Color color;
    float speed;
    int move_x, move_y;
    int key; // Last key pressed by the user
    int option, score;
    time_t time;
    ALLEGRO_BITMAP *sprites[PACMAN_SPRITES];
};

struct Scores
{
    std::string *names;
    int *points, size;
};

struct Pacman
{
    Color color;
    Circle circle;
};

struct MapPoint
{
    int x, y;
};

struct Ghost
{
    Circle cir;
    MapPoint pos_coord;
    int moving, move_x, move_y;
    std::list<MapPoint> path;
    float speed;
    Color color;
};

struct Vertex
{
    int dist; // You might consider renaming this to g_cost for clarity
    int g_cost; // Cost from the start node
    int h_cost; // Heuristic cost to the goal
    int f_cost; // Total cost: g_cost + h_cost
    Vertex *prev; // To reconstruct the path
    MapPoint key; // Coordinates of the vertex
    bool containsPacman; // Specific to your game logic
    GraphColor color; // Used for marking the state in searches
    std::list<Vertex *> edges; // Adjacency list
};

#include "physics.hpp"
#include "ai.hpp"
#include "geom.hpp"
#include "draw.hpp"
