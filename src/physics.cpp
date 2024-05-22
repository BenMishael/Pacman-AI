#include "physics.hpp"

inline bool operator<(const MapPoint &a, const MapPoint &b)
{
    return std::tie(a.x, a.y) < std::tie(b.x, b.y);
}

namespace physics
{
void Physics::initPhy(int x, int y)
{
    pacmanPosition = {x, y};
    previousBlock = pacmanPosition;
    moving = false;
}

MapPoint Physics::get_pacman_position(void)
{
    return pacmanPosition;
}

Point centerBlock(MapPoint coordinates)
{
    Rectangle block_col;
    block_col.size = {MAZE_WALL_WIDTH, MAZE_WALL_LENGTH};
    block_col.pos = {(float)coordinates.x * MAZE_WALL_WIDTH + BORDER, (float)coordinates.y * MAZE_WALL_LENGTH + BORDER};
    float x1 = block_col.pos.x, y1 = block_col.pos.y;
    float x2 = block_col.pos.x + block_col.size.width, y2 = block_col.pos.y + block_col.size.height;
    Point center{(x1 + x2) / 2, (y1 + y2) / 2};
    return center;
}

double Physics::distance2d(Point p1, Point p2)
{
    double part1 = pow((p1.x - p2.x), 2);
    double part2 = pow((p1.y - p2.y), 2);
    return sqrt(part1 + part2);
}

void Physics::defeat_condition(Player *player, std::list<Ghost> *ghosts)
{
    for (auto &ghost : (*ghosts))
    {
        if (distance2d(ghost.cir.center, (*player).cir.center) < PACMAN_RADIUS)
        {
            player->state = State::defeat;
        }
    }
}

int Physics::block_contains_pacman(int x, int y, Player pacman)
{
    Point block_center = centerBlock({x, y});
    double dist = distance2d(block_center, pacman.cir.center);
    if (dist < PACMAN_RADIUS * 2)
        return 1;
    return 0;
}

int Physics::block_contains_ghost(int x, int y, Ghost *ghost)
{
    Point block_center = centerBlock({x, y});
    double dist = distance2d(block_center, ghost->cir.center);
    if (dist < PACMAN_RADIUS * 2)
        return 1;
    return 0;
}

int hasPath(Player pacman, MapPoint pacmanPosition, std::map<MapPoint, Vertex> graph)
{
    MapPoint desiredPosition = pacmanPosition;
    desiredPosition.x += pacman.move_x;
    desiredPosition.y += pacman.move_y;
    for (auto it = graph[pacmanPosition].edges.begin(); it != graph[pacmanPosition].edges.end(); it++)
    {
        MapPoint explored_key = (**it).key;
        if (explored_key.x == desiredPosition.x && explored_key.y == desiredPosition.y)
            return 1;
    }
    return 0;
}

int Physics::pacmanChangedNode(Player *pacman)
{
    if (pacman->move_x < 0)
    {
        if (!block_contains_pacman(pacmanPosition.x, pacmanPosition.y, *pacman))
        {
            pacmanPosition.x += -1;
            pacman->cir.center = centerBlock(pacmanPosition);
            return 1;
        }
    }
    else if (pacman->move_y < 0)
    {
        if (!block_contains_pacman(pacmanPosition.x, pacmanPosition.y, *pacman))
        {
            pacmanPosition.y += -1;
            pacman->cir.center = centerBlock(pacmanPosition);
            return 1;
        }
    }
    else if (pacman->move_x > 0)
    {
        if (!block_contains_pacman(pacmanPosition.x, pacmanPosition.y, *pacman))
        {
            pacmanPosition.x += 1;
            pacman->cir.center = centerBlock(pacmanPosition);
            return 1;
        }
    }
    else if (pacman->move_y > 0)
    {
        if (!block_contains_pacman(pacmanPosition.x, pacmanPosition.y, *pacman))
        {
            pacmanPosition.y += 1;
            pacman->cir.center = centerBlock(pacmanPosition);
            return 1;
        }
    }
    return 0;
}

int changedDirection(Player *pacman)
{
    switch (pacman->key)
    {
    case ALLEGRO_KEY_UP:
        if (pacman->move_y == +1)
            return 1;
        else
            return 0;
    case ALLEGRO_KEY_DOWN:
        if (pacman->move_y == -1)
            return 1;
        else
            return 0;
    case ALLEGRO_KEY_RIGHT:
        if (pacman->move_x == -1)
            return 1;
        else
            return 0;
    case ALLEGRO_KEY_LEFT:
        if (pacman->move_x == 1)
            return 1;
        else
            return 0;
    default:
        return 0;
    }
}

int Physics::pacmanIsMoving()
{
    return moving;
}

void Physics::verify_collision(Player *pacman, std::map<MapPoint, Vertex> graph, Block maze[][MAZE_SIDE_LENGTH])
{
    if (!moving)
    {
        move_pacman(pacman);
        if (hasPath(*pacman, pacmanPosition, graph))
            moving = 1;
    }

    if (moving == 1)
    {
        if (pacman->move_x < 0)
        {
            pacman->cir.center.x -= pacman->speed;
        }
        else if (pacman->move_x > 0)
        {
            pacman->cir.center.x += pacman->speed;
        }
        else if (pacman->move_y > 0)
        {
            pacman->cir.center.y += pacman->speed;
        }
        else if (pacman->move_y < 0)
        {
            pacman->cir.center.y -= pacman->speed;
        }
    }
    if (pacmanChangedNode(pacman))
    {
        moving = 0;
        if (maze[pacmanPosition.x][pacmanPosition.y].type == BlockTypes::intersection)
        {
            pacman->move_x = 0;
            pacman->move_y = 0;
        }
        if (maze[pacmanPosition.x][pacmanPosition.y].type == BlockTypes::portal)
        {
            if (pacmanPosition.x == 28)
                pacmanPosition = {1, 17};
            else
                pacmanPosition = {27, 17};
        }
    }
    if (changedDirection(pacman) && moving == 1)
    {
        moving = 0;
        pacman->move_x *= -1;
        pacman->move_y *= -1;
    }
}

int Physics::pacman_score(Player *pacman, Block maze[][MAZE_SIDE_LENGTH], int score)
{
    if (maze[pacmanPosition.x][pacmanPosition.y].hasPoint == true)
    {
        score += 10;
        maze[pacmanPosition.x][pacmanPosition.y].hasPoint = false;
    }

    if (maze[pacmanPosition.x][pacmanPosition.y].hasBonus == true)
    {
        score += 40;
        maze[pacmanPosition.x][pacmanPosition.y].hasBonus = false;
    }

    return score;
}

void Physics::win_condition(Player *player, Block maze[][MAZE_SIDE_LENGTH])
{
    for (int i = 0; i < MAZE_SIDE_WIDTH; i++)
    {
        for (int j = 0; j < MAZE_SIDE_LENGTH; j++)
        {
            if (maze[i][j].type != BlockTypes::wall)
            {
                if (maze[i][j].hasPoint == true)
                {
                    return;
                }
            }
        }
    }
    player->state = State::win;
}

void detectGhostDirection(MapPoint ghost, MapPoint step, int *move_x, int *move_y)
{
    if (step.x > ghost.x)
    {
        (*move_x) = 1;
        (*move_y) = 0;
    }
    else if (step.x < ghost.x)
    {
        (*move_x) = -1;
        (*move_y) = 0;
    }
    else if (step.y > ghost.y)
    {
        (*move_y) = -1;
        (*move_x) = 0;
    }
    else if (step.y < ghost.y)
    {
        (*move_y) = 1;
        (*move_x) = 0;
    }
}

void move_ghost(Ghost *ghost, int move_x, int move_y)
{
    if (move_x < 0)
        ghost->cir.center.x -= ghost->speed;

    else if (move_x > 0)
        ghost->cir.center.x += ghost->speed;

    else if (move_y > 0)
        ghost->cir.center.y -= ghost->speed;

    else if (move_y < 0)
        ghost->cir.center.y += ghost->speed;
}

int Physics::ghostChangedNode(Ghost *ghost)
{
    if (ghost->move_x < 0)
    {
        if (!block_contains_ghost(ghost->pos_coord.x, ghost->pos_coord.y, ghost))
        {
            ghost->pos_coord.x += -1;
            ghost->cir.center = centerBlock(ghost->pos_coord);
            return 1;
        }
    }
    else if (ghost->move_x > 0)
    {
        if (!block_contains_ghost(ghost->pos_coord.x, ghost->pos_coord.y, ghost))
        {
            ghost->pos_coord.x += 1;
            ghost->cir.center = centerBlock(ghost->pos_coord);
            return 1;
        }
    }
    else if (ghost->move_y < 0)
    {
        if (!block_contains_ghost(ghost->pos_coord.x, ghost->pos_coord.y, ghost))
        {
            ghost->pos_coord.y -= -1;
            ghost->cir.center = centerBlock(ghost->pos_coord);
            return 1;
        }
    }
    else if (ghost->move_y > 0)
    {
        if (!block_contains_ghost(ghost->pos_coord.x, ghost->pos_coord.y, ghost))
        {
            ghost->pos_coord.y -= 1;
            ghost->cir.center = centerBlock(ghost->pos_coord);
            return 1;
        }
    }
    return 0;
}

void Physics::move_ghosts_2(std::list<Ghost> *ghosts)
{
    for (auto &ghost : *ghosts)
    {
        if (ghost.moving == 0)
        {
            MapPoint step = ghost.path.back();
            detectGhostDirection(ghost.pos_coord, step, &(ghost.move_x), &(ghost.move_y));
            ghost.moving = 1;
        }

        if (ghostChangedNode(&ghost))
        {
            ghost.path.pop_back();
            ghost.moving = 0;
        }
        else
            move_ghost(&ghost, ghost.move_x, ghost.move_y);
    }
}

void Physics::move_pacman(Player *pacman)
{
    switch (pacman->key)
    {
    case ALLEGRO_KEY_UP:
        pacman->move_y = -1;
        pacman->move_x = 0;
        break;
    case ALLEGRO_KEY_DOWN:
        pacman->move_y = 1;
        pacman->move_x = 0;
        break;
    case ALLEGRO_KEY_RIGHT:
        pacman->move_x = 1;
        pacman->move_y = 0;
        break;
    case ALLEGRO_KEY_LEFT:
        pacman->move_x = -1;
        pacman->move_y = 0;
        break;
    }
}
}; // namespace physics
