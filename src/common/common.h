#pragma once
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
// Position structure
struct Position
{
    float x, y, z, w;
};

// Color structure
struct Color
{
    float r, g, b, a;
};

// Vertex structure
struct Vertex
{
    Position position;
    Color color;
};

inline double degreesToRadians(double angle){
     return angle * M_PI / 180;

}