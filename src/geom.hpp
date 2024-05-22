// geom.hpp

#pragma once

#include <cmath>

namespace geom
{

// A point, consisting of an x and y coordinate
struct Point
{
    float x;
    float y;
};

// Size, consisting of width and height, of type float
struct Size
{
    float width;
    float height;
};

// Rectangle, consisting of a point (which contains the coordinates of the bottom left corner of the rectangle) and a size
struct Rectangle
{
    Point pos;
    Size size;
};

// Circle, consisting of a point (the center of the circle) and a float (the radius)
struct Circle
{
    Point center;
    float radius;
};

// Functions

// Returns the distance between two points
inline float distance(Point p1, Point p2)
{
    return std::sqrt(std::pow((p2.x - p1.x), 2) + std::pow((p2.y - p1.y), 2));
}

// Returns true if the point is inside the circle, false otherwise
inline bool is_point_in_circle(Point p, Circle c)
{
    return distance(p, c.center) <= c.radius;
}

// Returns true if the point is inside the rectangle
inline bool is_point_in_rectangle(Point p, Rectangle r)
{
    return (r.pos.x < p.x && r.pos.x + r.size.width > p.x && r.pos.y < p.y &&
            r.pos.y + r.size.height > p.y);
}

// Returns true if there is an intersection between the circle and the rectangle
inline bool is_circle_rectangle_intersection(Circle c, Rectangle r)
{
    Point limit;
    if (is_point_in_rectangle(c.center, r))
        return true;
    else
    {
        if (c.center.x < r.pos.x)
            limit.x = r.pos.x - c.center.x;
        else if (c.center.x > r.pos.x + r.size.width)
            limit.x = c.center.x - r.pos.x + r.size.width;
        else
            limit.x = 0;

        if (c.center.y < r.pos.y)
            limit.y = r.pos.y - c.center.y;
        else if (c.center.y > r.pos.y + r.size.height)
            limit.y = c.center.y - r.pos.y + r.size.height;
        else
            limit.y = 0;

        return distance(c.center, limit) < c.radius;
    }
}

// Returns true if there is an intersection between the two rectangles
inline bool is_rectangle_rectangle_intersection(Rectangle r1, Rectangle r2)
{
    Point _r1, _r2;
    _r1.x = r1.pos.x + r1.size.width;
    _r1.y = r1.pos.y + r1.size.height;
    _r2.x = r2.pos.x + r2.size.width;
    _r2.y = r2.pos.y + r2.size.height;

    return (is_point_in_rectangle(r1.pos, r2) || is_point_in_rectangle(r2.pos, r1) || 
            is_point_in_rectangle(_r1, r2) || is_point_in_rectangle(_r2, r1));
}

// Returns true if there is an intersection between the two circles
inline bool is_circle_circle_intersection(Circle c1, Circle c2)
{
    return distance(c1.center, c2.center) <= c1.radius + c2.radius;
}

}; // namespace geom
