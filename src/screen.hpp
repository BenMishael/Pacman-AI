
#pragma once

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_color.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_audio.h>

#include "geom.hpp"

using namespace geom;

namespace screen
{

// structure that represents a color, with the components
// red, green, and blue varying between 0 and 1.
struct Color
{
    float r;
    float g;
    float b;
};

// defines a screen
struct Screen
{
    ALLEGRO_DISPLAY *display;   // X display
    ALLEGRO_COLOR bg_color;     // background color
    ALLEGRO_COLOR fg_color;     // default color
    ALLEGRO_EVENT_QUEUE *queue; // event queue
    ALLEGRO_FONT *font;         // default font
    ALLEGRO_FONT *font2;        // another font because allegro has this
    ALLEGRO_FONT *font3;
    ALLEGRO_BITMAP *pacmanbmp;   // little pacman image
    ALLEGRO_SAMPLE *menuMusic;
    ALLEGRO_SAMPLE *menuScroll;
    ALLEGRO_SAMPLE *menuSelect;
    ALLEGRO_SAMPLE *wakasfx;
    /* ALLEGRO_TIMER *timer;  // timer */
    Rectangle window; // rectangle that contains our window
    Size screen_size; // size of the window
    Point _mouse;     // where the mouse is
    bool _button;     // mouse button state
    int _key;         // last key pressed

    // initializes the screen; must be called at the beginning of the program execution
    void init(int width, int height, const char *name);

    // finalizes the screen; must be called at the end of the program
    void finish();

    // clears the entire screen
    void clear();

    // makes what was drawn appear in the window
    void show();

    // changes the color of the next drawings of line/rectangle/characters/etc
    void color(Color c);

    // calculates the number of pixels (horizontal) needed for string s
    int strlen(const char *s) const;

    // writes the text s starting from position p on the screen
    void text(Point p, const char *s);

    // writes the text s starting from position p on the screen
    void text2(Point p, const char *s);

    void text3(Point p, const char *s);

    void text_score_name(Point p, const char *s);

    void text_score_points(Point p, const char *s);

    void image_menu(Point p);

    void play_menuMusic(void);

    void play_menuScroll(void);

    void play_menuSelect(void);

    void play_wakasfx(void);

    // returns the code of the next key pressed (or 0 if no key is pressed)
    int key();

    // returns true if the mouse button is pressed
    bool button();

    // returns the position of the mouse cursor
    Point mouse();

    // screen wait time in milliseconds
    void wait(double ms);

    // draws a line from point p1 to point p2
    void line(Point p1, Point p2);

    // draws a rectangle
    void rectangle(Rectangle r);

    // draws a circle
    void circle(Circle c);

    // size needed to write the text s
    Size text_size(const char *s);

    // Screen size.
    Size size() const;

    // processes screen events
    void process_events();
};

}; // namespace screen
