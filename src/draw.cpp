#include <string>
#include <fstream>
#include "draw.hpp"
#include <allegro5/allegro_image.h>

const Color ghosts_colors[3] = {
    {255, 0, 0},   // BLINKY
    {255, 157, 0}, // INKY
    {249, 162, 244} // PINKY
};


namespace draw
{
void Draw::draw_background(void)
{
    // al_draw_bitmap(background, 0, 0, 0);
    al_flip_display();
}

// Draw all the figures on the screen
void Draw::draw_figures(Player player, std::list<Ghost> ghosts)
{
    draw_pacman(player);
    draw_ghosts(ghosts);
}

void load_pacman_sprites(ALLEGRO_BITMAP **sprites)
{
    al_init_image_addon();
    std::string image_name("pac");
    char image_number[10];
    for (int i = 0; i < PACMAN_SPRITES; i++)
    {
        sprintf(image_number, "%d", i);
        image_name.append(image_number);
        image_name.append(".png");
        sprites[i] = al_load_bitmap(image_name.c_str());
    }
}

void Draw::draw_pacman(Player pacPlayer)
{
    Color yellow = {255, 255, 0};
    t.color(yellow);
    t.circle(pacPlayer.cir);
}

void Draw::draw_ghosts(std::list<Ghost> ghosts)
{
    int color_index = 0;
    for (auto it = ghosts.begin(); it != ghosts.end() && color_index < 3; it++, color_index++)
    {
        t.color(ghosts_colors[color_index]);
        t.circle(it->cir);
        t.rectangle({{float(it->cir.center.x - PACMAN_RADIUS + 0.25), it->cir.center.y}, {float(PACMAN_RADIUS * 2 - 0.25), PACMAN_RADIUS}});
    }
}


void Draw::draw_map(Block maze[][MAZE_SIDE_LENGTH])
{
    for (int x = 0; x < MAZE_SIDE_WIDTH; x++)
        for (int y = 0; y < MAZE_SIDE_LENGTH; y++)
        {
            switch (maze[x][y].type)
            {
            case BlockTypes::wall:
                draw_wall(x, y);
                break;

            case BlockTypes::path:
                if (maze[x][y].hasPoint == true)
                    draw_point(x, y, false);
                break;

            case BlockTypes::intersection:
                if (maze[x][y].hasBonus == true)
                    draw_point(x, y, true);
                else if (maze[x][y].hasPoint == true)
                    draw_point(x, y, false);
                break;
            default:
                break;
            }
        }
}

void Draw::draw_wall(int x, int y)
{
    Rectangle wall;
    wall.size = {MAZE_WALL_WIDTH, MAZE_WALL_LENGTH};
    Color wall_color = {0, 0, 255};
    t.color(wall_color);
    wall.pos = {(float)x * MAZE_WALL_WIDTH + BORDER, (float)y * MAZE_WALL_LENGTH + BORDER};
    t.rectangle(wall);
}

void Draw::draw_point(int x, int y, bool bonus)
{
    Circle point;
    Color yellow = {255, 255, 0};
    t.color(yellow);
    point.center = {(float)x * MAZE_WALL_WIDTH + BORDER + (MAZE_WALL_WIDTH / 2), (float)y * MAZE_WALL_LENGTH + BORDER + (MAZE_WALL_LENGTH / 2)};
    point.radius = (bonus) ? MAZE_WALL_WIDTH / 3.5 : MAZE_WALL_WIDTH / 7.5;
    t.circle(point);
}

int Draw::get_ms()
{
    using namespace std::chrono;
    milliseconds ms = duration_cast<milliseconds>(
        system_clock::now().time_since_epoch());
    return ms.count();
}

void Draw::draw_main_menu(Player *player)
{
    t.color({0, 0, 255});
    t.text({SCREEN_WIDTH / 2, 125}, "PAC  MAN");
    t.image_menu({SCREEN_WIDTH / 2 - 50, 80});
    // draw_palms();
    if (player->option == 0)
    {
        if (sin(get_ms() / 75) > -0.5)
        {
            t.color({255, 0, 0});
            t.text2({SCREEN_WIDTH / 2, 345}, "PRESS 'ENTER'");
        }
        if (player->key == ALLEGRO_KEY_ENTER)
        {
            t.play_menuSelect();
            player->option = 1;
        }
    }
    else
    {
        if (player->key == ALLEGRO_KEY_W || player->key == ALLEGRO_KEY_UP)
        {
            player->option--;
            t.play_menuScroll();
            if (player->option < 1)
                player->option = 3;
        }

        if (player->key == ALLEGRO_KEY_S || player->key == ALLEGRO_KEY_DOWN)
        {
            player->option++;
            t.play_menuScroll();
            if (player->option > 3)
                player->option = 1;
        }

        draw_option_switch(player->option);

        if (player->key == ALLEGRO_KEY_ENTER)
        {
            t.play_menuSelect();
            switch (player->option)
            {
            case 1:
                player->state = State::playing;
                break;
            case 2:
                player->state = State::score;
                break;
            case 3:
                sleep(1);
                player->state = State::end;
                break;
            default:
                break;
            }
        }
    }
}

// void Draw::draw_palms()
// {
//     int time_seed = get_ms();
//     for (i = 0; i < time_seed)
// }

void Draw::draw_option_switch(int option)
{
    for (auto i = 0; i < 3; i++)
    {
        float mult = i * 50;
        if ((i + 1) == option)
        {
            main_menu_text_effect({SCREEN_WIDTH / 2, 275 + mult});
            t.color({220, 0, 0});
        }
        else
            t.color({255, 255, 0});

        t.text2({SCREEN_WIDTH / 2, 275 + mult}, options[i]);
    }
}

void Draw::main_menu_text_effect(Point location)
{
    float variance = sin(get_ms() / 50) * 30;
    float cx = location.x, cy = location.y + 15;
    al_draw_filled_ellipse(cx - 5, cy, 39 + variance, 9, {0, 0, 255});
    al_draw_filled_ellipse(cx - 5, cy + variance / 3, 40 + variance, 10, {0, 255, 0});
}

void Draw::scoreboard_bubblesort(Scores *scores)
{
    std::string aux;
    int size = 0;

    std::ifstream input;
    input.open("data/scores.txt");

    while (std::getline(input, aux))
    {
        size++;
    }

    scores->size = size;
    scores->names = new std::string[size];
    scores->points = new int[size];

    input.clear();
    input.seekg(0, std::ios::beg);

    for (auto i = 0; i < size; i++)
    {
        input >> scores->names[i];
        input >> scores->points[i];
    }

    input.close();

    for (auto i = 0; i < size; i++)
    {
        for (int j = (i + 1); j < size; j++)
        {
            if (scores->points[j] > scores->points[i])
            {
                int a = scores->points[j];
                scores->points[j] = scores->points[i];
                scores->points[i] = a;

                aux = scores->names[j];
                scores->names[j] = scores->names[i];
                scores->names[i] = aux;
            }
        }
    }
}

void Draw::draw_scoreboard(Player *player, Scores *scores)
{
    t.color({0, 0, 255});
    t.text({SCREEN_WIDTH / 2, 30}, "SCORES");

    t.color({255, 255, 0});

    for (int i = 0; i < scores->size && i < 10; i++)
    {
        int aux_score = i + 1;

        std::string aux = std::to_string(aux_score);

        if ((i + 1) <= scores->size)
            aux = '0' + aux;
        aux += '.';

        float mult = i * 40;

        t.text_score_name({20, 120 + mult}, aux.c_str());
        t.text_score_name({110, 120 + mult}, scores->names[i].c_str());

        aux = std::to_string(scores->points[i]);
        t.text_score_points({470, 120 + mult}, aux.c_str());
    }

    main_menu_text_effect({SCREEN_WIDTH / 2, 560});

    t.color({220, 0, 0});
    t.text2({SCREEN_WIDTH / 2, 560}, "BACK");

    if (player->key == ALLEGRO_KEY_ENTER)
    {
        t.play_menuSelect();
        player->state = State::menu;
    }
}

void Draw::draw_score(int score)
{
    t.color({255, 255, 0});
    std::string txt = "SCORE: ";
    std::string string_score = std::to_string(score);
    txt += string_score;
    t.text3({SCREEN_WIDTH / 2, 5}, txt.c_str());
}

void Draw::save_to_file(std::string str, int score)
{
    std::ofstream output;
    output.open("data/scores.txt", std::ios::app);
    output << '\n'
           << str << ' ' << score;
    output.close();
}

void Draw::input_text(Player *player, std::string *str, int score)
{
    if (player->key != 0)
    {
        if ((*str).length() < 7)
        {
            if (player->key >= 1 && player->key <= 26)
            {
                char aux = 'A' + player->key - 1;
                (*str).push_back(aux);
            }
            else if (player->key >= 27 && player->key <= 36)
            {
                char aux = '0' + player->key - 1;
                (*str).push_back(aux);
            }
        }
        if (player->key == ALLEGRO_KEY_BACKSPACE)
        {
            if ((*str).length() > 0)
                (*str).pop_back();
        }
        if (player->key == ALLEGRO_KEY_ENTER && (*str).length() > 0)
        {
            save_to_file(*str, score);
            player->state = State::init;
        }
    }
}

void Draw::print_centered(std::string str)
{
    if (str.length() > 0)
    {
        t.color({255, 0, 0});
        t.text2({SCREEN_WIDTH / 2, (SCREEN_LENGTH / 2) + 60}, str.c_str());
    }
}

void Draw::draw_win(Player *player, int score, std::string *str)
{
    std::string txt = "SCORE: ";
    std::string string_score = std::to_string(score);
    txt += string_score;

    t.color({255, 255, 0});
    t.text({SCREEN_WIDTH / 2, 40}, "YOU WIN");
    t.text2({SCREEN_WIDTH / 2, (SCREEN_LENGTH / 2) - 40}, txt.c_str());
    t.text2({SCREEN_WIDTH / 2, SCREEN_LENGTH / 2}, "ENTER YOUR NAME:");

    if (sin(get_ms() / 75) > -0.5)
    {
        t.text3({SCREEN_WIDTH / 2, (SCREEN_LENGTH / 2) + 120}, "PRESS 'ENTER' WHEN READY");
    }

    input_text(player, str, score);
    print_centered(*str);
}

void Draw::draw_defeat(Player *player, int score, std::string *str)
{
    std::string txt = "SCORE: ";
    std::string string_score = std::to_string(score);
    txt += string_score;

    t.color({255, 255, 0});
    t.text({SCREEN_WIDTH / 2, 40}, "YOU LOSE");
    t.text2({SCREEN_WIDTH / 2, (SCREEN_LENGTH / 2) - 40}, txt.c_str());
    t.text2({SCREEN_WIDTH / 2, SCREEN_LENGTH / 2}, "ENTER YOUR NAME:");

    if (sin(get_ms() / 75) > -0.5)
    {
        t.text3({SCREEN_WIDTH / 2, (SCREEN_LENGTH / 2) + 120}, "PRESS 'ENTER' WHEN READY");
    }

    input_text(player, str, score);
    print_centered(*str);
}

void Draw::draw_help(void)
{
    std::cout << "Press: " << std::endl;
    std::cout << " - '^' ,'<' 'v' and '>' to move " << std::endl;
    std::cout << " - 'esc' to exit" << std::endl;
}

void Draw::draw_end()
{
}

void Draw::load_background()
{
}
void Draw::finish(void)
{
    t.finish();
}
}; // namespace draw
