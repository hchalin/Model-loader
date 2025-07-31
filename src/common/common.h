#pragma once
#include "./BroMath/vec4.h"
#include <eigen/Eigen/Core>
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
struct Vertex {
    Eigen::Vector4f position;  // x, y, z, w
    Eigen::Vector4f color;
    Eigen::Vector3f normal;    // x, y, z
    Eigen::Vector2f uv;  // u, v

    Vertex()
        : position(Eigen::Vector4f::Zero()),
          color(Eigen::Vector4f::Zero()),
          normal(Eigen::Vector3f::Zero()),
          uv(Eigen::Vector2f::Zero()) {}

    Vertex(const Eigen::Vector4f& pos,
        const Eigen::Vector4f& color,
           const Eigen::Vector3f& nrm,
           const Eigen::Vector2f& uv)
        : position(pos), color(color), normal(nrm), uv(uv) {}
};


inline double degreesToRadians(double angle){
     return angle * M_PI / 180;
}