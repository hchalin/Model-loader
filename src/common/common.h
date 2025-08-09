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
    Eigen::Vector2f texCoord;  // u, v

    Vertex()
        : position(Eigen::Vector4f::Zero()),
          color(Eigen::Vector4f::Zero()),
          normal(Eigen::Vector3f::Zero()),
          texCoord(Eigen::Vector2f::Zero()) {}

    Vertex(const Eigen::Vector4f& pos,
        const Eigen::Vector4f& color,
           const Eigen::Vector3f& nrm,
           const Eigen::Vector2f& texCoord)
        : position(pos), color(color), normal(nrm), texCoord(texCoord) {}

};

/**
 * \brief Converts an angle from degrees to radians.
 *
 * \param angle The angle in degrees.
 * \return The angle in radians.
 */
inline double degreesToRadians(double angle){
     return angle * M_PI / 180;
}



/**
 * \brief Combines multiple hash values into a single hash value.
 *
 * This function is used for hashing the vertices into an index array.
 *
 * \tparam T The type of the first value to hash.
 * \tparam Rest The types of the remaining values to hash.
 * \param seed The initial hash value, which will be updated.
 * \param v The first value to hash.
 * \param rest The remaining values to hash.
 */
#include <functional>


    // from: https://stackoverflow.com/a/57595105
    template <typename T, typename... Rest>
    void hashCombine(std::size_t& seed, const T& v, const Rest&... rest) {
        seed ^= std::hash<T>{}(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        (hashCombine(seed, rest), ...);
    };
