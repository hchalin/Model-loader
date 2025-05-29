#pragma once
#include "./BroMath/vec4.h"
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
// Position structure
struct Position
{
  //! NOT USED
    float x, y, z, w;
};

// Color structure
struct Color
{
  //! NOT USED
    float r, g, b, a;
};

// Vertex structure
struct Vertex
{
    float4 position;
    float4 color;
};

inline double degreesToRadians(double angle){
     return angle * M_PI / 180;
}