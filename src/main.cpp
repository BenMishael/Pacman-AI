// Command to compile:
// make clean
// make
#include "init.hpp"
#include <fstream>

// Since MapPoint is used as a key in the Map, it needs a comparison operator
inline bool operator<(const MapPoint &a, const MapPoint &b)
{
    return std::tie(a.x, a.y) < std::tie(b.x, b.y);
}

struct Game
{
    Player player;
    Scores scores;
    std::list<Ghost> ghosts;
    draw::Draw draw;
    physics::Physics phy;
    Block maze[MAZE_SIDE_WIDTH][MAZE_SIDE_LENGTH];
    ai::Ai ai;
    int score;
    MapPoint prev_pacman_position;
    std::string str;

    // Initialize the main structures of the Game
    void init(void)
    {
        score = 0;
        draw.t.init(SCREEN_WIDTH, SCREEN_LENGTH, "PAC MAN");
        draw.t.play_menuMusic();
        draw.load_background();
        draw.scoreboard_bubblesort(&scores);
        load_maze();
        ai.init_Vertexs(maze);
        ai.initContainsPacman(14, 20);
        init_player();
        phy.initPhy(14, 20);
        updatePaths();
    }

    void init_ghost(int x, int y)
    {
        Ghost ghost;
        ghost.cir.center = {(float)x * MAZE_WALL_WIDTH + BORDER + (MAZE_WALL_WIDTH / 2), (float)y * MAZE_WALL_LENGTH + BORDER + (MAZE_WALL_LENGTH / 2)};
        ghost.cir.radius = PACMAN_RADIUS;
        ghost.pos_coord = {x, y};

        ghost.moving = 0;
        ghost.speed = 4; // Increased from 1 to 2
        ghosts.push_back(ghost);
    }

    void init_player()
    {
        player.state = State::menu;
        player.option = 0;
        player.move_x = 0;
        player.move_y = 0;
        player.speed = 5; // Increased from 3 to 5
        player.cir.radius = PACMAN_RADIUS;
        // draw.load_pacman_sprites(player.sprites);
    }

    void load_maze()
    {
        int numOfBonus = 0;
        std::string line;
        std::ifstream fileStream{MAZE_FILE};
        ghosts.clear(); // Clear the list of ghosts before initializing

        for (auto i = 0; std::getline(fileStream, line); i++)
            for (auto l = 0; line[l] != '\0'; l++)
            {
                maze[l][i].hasPoint = false;
                maze[l][i].hasBonus = false;
                switch (line[l])
                {
                case '-':
                    maze[l][i].type = BlockTypes::path;
                    maze[l][i].hasPoint = true;
                    break;
                case '+':
                    maze[l][i].type = BlockTypes::intersection;
                    maze[l][i].hasPoint = true;
                    maze[l][i].hasBonus = false;
                    if (numOfBonus < MAX_NUM_OF_BONUS && sin(rand()) > 0.9)
                    {
                        maze[l][i].hasPoint = false;
                        maze[l][i].hasBonus = false;
                        numOfBonus++;
                    }
                    break;
                case '0':
                    maze[l][i].type = BlockTypes::wall;
                    break;
                case '|':
                    maze[l][i].type = BlockTypes::path;
                    maze[l][i].hasPoint = true;
                    break;
                case '*':
                    maze[l][i].type = BlockTypes::path;
                    player.cir.center = {(float)l * MAZE_WALL_WIDTH + BORDER + (MAZE_WALL_WIDTH / 2), (float)i * MAZE_WALL_LENGTH + BORDER + (MAZE_WALL_LENGTH / 2)};
                    break;
                case '#':
                    if (ghosts.size() < 3) // Ensure only three ghosts are initialized
                    {
                        maze[l][i].type = BlockTypes::path;
                        maze[l][i].hasPoint = true;
                        init_ghost(l, i);
                    }
                    break;
                case ' ':
                    maze[l][i].type = BlockTypes::path;
                    break;
                case 'P':
                    maze[l][i].type = BlockTypes::portal;
                    break;
                }
            }
    }

    void updatePaths()
    {
        // Update to use A* search instead of BFS
        ai.a_star_search(&(ai.graph[phy.get_pacman_position()]), &(ai.graph[prev_pacman_position]));
        for (auto &ghost : ghosts)
        {
            ghost.path.clear();
            ai.shortest_path(phy.get_pacman_position(), ghost.pos_coord, &ghost.path);
            ghost.path.pop_back(); // Remove the element that contains the ghost's location
        }
    }

    void move_ghosts(void)
    {
        updatePaths();
        phy.move_ghosts_2(&ghosts);
    }

    // Updates the game
    void update(void)
    {
        // Read last key
        player.key = draw.t.key();
        draw.t.clear();
        if (player.state == State::menu)
            draw.draw_main_menu(&player);

        else if (player.state == State::init)
        {
            score = 0;
            load_maze();
            ai.initContainsPacman(14, 20);
            phy.initPhy(14, 20);
            player.state = State::menu;
            str.clear();
            updatePaths();
        }
        else if (player.state == State::playing)
        {
            int prev_score = score;
            draw.draw_map(maze);
            draw.draw_figures(player, ghosts);
            phy.verify_collision(&player, ai.graph, maze);
            move_ghosts();
            score = phy.pacman_score(&player, maze, score);
            if (prev_score != score)
            {
                phy.win_condition(&player, maze);
                draw.t.play_wakasfx();
            }
            draw.draw_score(score);
            phy.defeat_condition(&player, &ghosts);
        }
        else if (player.state == State::score)
        {
            draw.scoreboard_bubblesort(&scores);
            draw.draw_scoreboard(&player, &scores);
        }
        else if (player.state == State::win)
            draw.draw_win(&player, score, &str);
        else if (player.state == State::defeat)
            draw.draw_defeat(&player, score, &str);

        // Update the screen
        draw.t.show();
        draw.draw_background();
        draw.t.wait(16.66);
    }
};

int main(int argc, char **argv)
{
    Game game;
    game.init();
    while (game.player.state != State::end)
        game.update();
    game.draw.finish();
    return 0;
}
