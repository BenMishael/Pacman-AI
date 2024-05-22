
#include <iostream>
#include <cstdlib>

#include "screen.hpp"
#include "geom.hpp"

namespace screen
{

/* Conversion of user units to X coordinates and vice versa */
/* Regardless of the window size, we say it is 80x24 */
//#define XU2X(x) ((short)(WIDTH * (x) / 80))
//#define YU2X(y) ((short)(HEIGHT * (y) / 24))
//#define XX2U(x) ((float)(x) / WIDTH * 80)
//#define YX2U(y) ((float)(y) / HEIGHT * 24)
/* Each 1 in user coordinates corresponds to 10 pixels for the X */
//#define XU2X(x) ((short)((x)*10))
//#define YU2X(y) ((short)((y)*10))
//#define XX2U(x) ((float)(x)/10)
//#define YX2U(y) ((float)(y)/10)

constexpr float XU2X(float x)
{
    return x;
}
constexpr float YU2X(float y)
{
    return y;
}
constexpr float XX2U(float x)
{
    return x;
}
constexpr float YX2U(float y)
{
    return y;
}

/* Adjust according to the X server, to get the colors right */
/* If the X server is 16 bits, comment the line below */
#define TWENTY_FOUR
#ifdef TWENTY_FOUR
#define B_R 8
#define B_G 8
#define B_B 8
#else
#define B_R 5
#define B_G 6
#define B_B 5
#endif

void Screen::init(int width, int height, const char *name)
{
    /* initialize mouse data */
    _mouse.x = 0;
    _mouse.y = 0;
    _button = false;
    _key = 0;

    /* initialize allegro */
    if (!al_init())
    {
        std::cerr << "failed to initialize allegro" << std::endl;
        std::abort();
    }

    /* connect to X screen */
    display = al_create_display(width, height);
    if (display == NULL)
    {
        std::cerr << "failed to create allegro display" << std::endl;
        std::abort();
    }

    window.pos.x = 0;
    window.pos.y = 0;
    this->screen_size.width = width;
    this->screen_size.height = height;
    window.size.width = XX2U(this->screen_size.width);
    window.size.height = YX2U(this->screen_size.height);

    /* Screen title */
    al_set_window_title(display, name);

    /* create graphic contexts */
    bg_color = al_map_rgb(0, 0, 0);
    fg_color = al_map_rgb(0, 0, 0);

    /* install mouse and keyboard drivers */
    al_install_mouse();
    al_install_keyboard();
    al_install_audio();
    al_init_primitives_addon();

    /* configure font */
    al_init_font_addon();
    al_init_ttf_addon();
    /* configure bmp */
    al_init_image_addon();
    /* configure audio */
    al_init_acodec_addon();

    pacmanbmp = al_load_bitmap("img/pacmenu.png");

    font = al_load_ttf_font("data/PAC-BIT.ttf", 56, 0);
    font2 = al_load_ttf_font("data/PAC-BIT.ttf", 32, 0);
    font3 = al_load_ttf_font("data/PAC-BIT.ttf", 20, 0);

    menuMusic = al_load_sample("data/menu.ogg");
    menuScroll = al_load_sample("data/menuscroll.wav");
    menuSelect = al_load_sample("data/menuselect.wav");
    wakasfx = al_load_sample("data/pacman_chomp.wav");

    al_reserve_samples(4);

    if (!font || !font2 || !font3)
    {
        std::cerr << "failed to load allegro font" << std::endl;
        std::abort();
    }

    if (!pacmanbmp)
    {
        std::cerr << "failed to load allegro image" << std::endl;
        std::abort();
    }

    /* start the timer */
    /* timer = al_create_timer(1.000 / FPS); */

    /* event queue */
    queue = al_create_event_queue();
    if (queue == NULL)
    {
        std::cerr << "failed to create allegro event queue" << std::endl;
        std::abort();
    }

    /* register to receive screen/keyboard/mouse events */
    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_display_event_source(display));
    al_register_event_source(queue, al_get_mouse_event_source());
    // al_register_event_source(queue,
    // al_get_timer_event_source(timer));
}

void Screen::clear()
{
    /* fill a rectangle the size of the screen with the background color */
    al_clear_to_color(bg_color);
}

void Screen::show()
{
    /* Swap video buffers, displaying what was drawn on the screen */
    // al_flip_display();
}

/* wait time in milliseconds */
void Screen::wait(double ms)
{
    al_rest(ms / 1e3);
}

void Screen::finish()
{
    /* the program is about to end, the end of the connection with the X server closes everything */
    al_destroy_display(display);
    al_destroy_bitmap(pacmanbmp);
    al_destroy_sample(menuMusic);
    al_destroy_sample(menuScroll);
    al_destroy_sample(menuSelect);
    al_destroy_event_queue(queue);
}

int Screen::key()
{
    /* returns the last key pressed */
    int key;
    process_events();
    key = _key;
    _key = 0;
    return key;
}

void Screen::process_events()
{
    /* process events from the X server, updating the mouse position
     * and the last key pressed in the screen variable. */
    ALLEGRO_EVENT event;

    // al_wait_for_event(queue, &event);
    while (al_get_next_event(queue, &event))
    {
        switch (event.type)
        {
        /* key was pressed */
        case ALLEGRO_EVENT_KEY_DOWN:
        {
            _key = event.keyboard.keycode;
            break;
        }
        case ALLEGRO_EVENT_MOUSE_AXES:
        {
            _mouse.x = XX2U(event.mouse.x);
            _mouse.y = YX2U(event.mouse.y);
            break;
        }
        case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
        {
            if (event.mouse.button == 1)
                _button = true;
            break;
        }
        case ALLEGRO_EVENT_MOUSE_BUTTON_UP:
        {
            if (event.mouse.button == 1)
                _button = false;
            break;
        }
        case ALLEGRO_EVENT_TIMER:
        {
            break;
        }
#if 0
        /* close window button pressed */
        case ALLEGRO_EVENT_DISPLAY_CLOSE:
            {
                return;
                break;
            }
#endif
        default:
            break;
        }
    }
}

void Screen::rectangle(Rectangle r)
{
    /* fill the rectangle r with the default color */
    al_draw_filled_rectangle(
        /* top left corner */
        XU2X(r.pos.x), YU2X(r.pos.y),
        /* bottom right corner */
        XU2X(r.pos.x + r.size.width), YU2X(r.pos.y + r.size.height), fg_color);
}

void Screen::circle(Circle c)
{
    /* fill the circle c on the screen with the default color */
    al_draw_filled_circle(XU2X(c.center.x), YU2X(c.center.y), XU2X(c.radius),
                          fg_color);
}

void Screen::line(Point p1, Point p2)
{
    /* connect the two points with a line in the default color */
    al_draw_line(XU2X(p1.x), YU2X(p1.y), XU2X(p2.x), YU2X(p2.y), fg_color, 0.0);
}

//#define ADJUST(x) (x < 0 ? 0 : (x > 1 ? 1 : x))
void Screen::color(Color c)
{
    /* change the default color */
    fg_color = al_map_rgb(c.r, c.g, c.b);
}

int Screen::strlen(const char *s) const
{
    return al_get_text_width(font, s);
}

void Screen::text(Point p, const char *s)
{
    /* write the text s at position p on the screen */
    al_draw_text(font, fg_color, XU2X(p.x), YU2X(p.y), ALLEGRO_ALIGN_CENTRE, s);
}

void Screen::text2(Point p, const char *s)
{
    /* write the text s at position p on the screen */
    al_draw_text(font2, fg_color, XU2X(p.x), YU2X(p.y), ALLEGRO_ALIGN_CENTRE, s);
}

void Screen::text3(Point p, const char *s)
{
    /* write the text s at position p on the screen */
    al_draw_text(font3, fg_color, XU2X(p.x), YU2X(p.y), ALLEGRO_ALIGN_CENTRE, s);
}

void Screen::text_score_name(Point p, const char *s)
{
    /* write the text s at position p on the screen */
    al_draw_text(font2, fg_color, XU2X(p.x), YU2X(p.y), ALLEGRO_ALIGN_LEFT, s);
}

void Screen::text_score_points(Point p, const char *s)
{
    /* write the text s at position p on the screen */
    al_draw_text(font2, fg_color, XU2X(p.x), YU2X(p.y), ALLEGRO_ALIGN_RIGHT, s);
}

void Screen::image_menu(Point p)
{
    al_draw_bitmap(pacmanbmp, XU2X(p.x), YU2X(p.y), 0);
}

void Screen::play_menuMusic(void)
{
    al_play_sample(menuMusic, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, 0);
}

void Screen::play_menuScroll(void)
{
    al_play_sample(menuScroll, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, 0);
}

void Screen::play_menuSelect(void)
{
    al_play_sample(menuSelect, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, 0);
}

void Screen::play_wakasfx(void)
{
    al_play_sample(wakasfx, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, 0);
}

Point Screen::mouse()
{
    /* return the mouse position */
    process_events();
    return _mouse;
}

bool Screen::button()
{
    /* return the state of the main mouse button (true == pressed) */
    process_events();
    return _button;
}

Size Screen::text_size(const char *s)
{
    Size text_size;
    int bbx, bby, bbw, bbh;
    al_get_text_dimensions(font, s, &bbx, &bby, &bbw, &bbh);
    text_size.width = bbw;
    text_size.height = bbh;
    return text_size;
}

Size Screen::size() const
{
    return screen_size;
}

}; // namespace screen
