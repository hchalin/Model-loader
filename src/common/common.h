#pragma once
#include "./BroMath/vec4.h"
#include <eigen/Eigen/Core>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif


// Vertex structure
struct Vertex {
    Eigen::Vector4f position;  // x, y, z, w
    Eigen::Vector4f color;
    Eigen::Vector3f normal;    // x, y, z
    Eigen::Vector2f texCoord;  // u, v
    uint32_t materialIndex{0};

    Vertex()
        : position(Eigen::Vector4f::Zero()),
          color(Eigen::Vector4f::Zero()),
          normal(Eigen::Vector3f::Zero()),
          texCoord(Eigen::Vector2f::Zero()) {}

    Vertex(const Eigen::Vector4f &pos,
           const Eigen::Vector4f &color,
           const Eigen::Vector3f &nrm,
           const Eigen::Vector2f &texCoord)
        : position(pos), color(color), normal(nrm), texCoord(texCoord) {
    }

    /**
        * \brief Equality operator to compare two vertices.
        * \param other The vertex to compare with.
        * \return True if all components are equal, false otherwise.
        */

        // NOTE: if you update the Vertex structure (add members), you must add
        // NOTE(cont.) the properties here and to the hashCombine below
    bool operator==(const Vertex& other) const {
        // ^ This is used for the models unordered_map of uniqueVertices
        return
                position == other.position &&
                color == other.color &&
                normal == other.normal &&
                texCoord == other.texCoord &&
                materialIndex == other.materialIndex;
    }

};


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
template<typename T, typename... Rest>
void hashCombine(std::size_t &seed, const T &v, const Rest &... rest) {
    // Call the hash function with the Vertex
    seed ^= std::hash<T>{}(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    (hashCombine(seed, rest), ...);
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



// @ HASH COMBINE
namespace std {
    /**
   * \brief Custom specialization of std::hash for Vertex.
   */
    template<>     // This defines the specialization
    struct hash<Vertex> {

    std::size_t operator()(const Vertex &vertex) const noexcept {
        std::size_t seed = 0;
        // Combine hashes for position, color, normal, and texCoord
        hashCombine(seed, vertex.position.x(), vertex.position.y(), vertex.position.z(), vertex.position.w());
        hashCombine(seed, vertex.color.x(), vertex.color.y(), vertex.color.z(), vertex.color.w());
        hashCombine(seed, vertex.normal.x(), vertex.normal.y(), vertex.normal.z());
        hashCombine(seed, vertex.texCoord.x(), vertex.texCoord.y());
        hashCombine(seed, vertex.materialIndex);
        return seed;
    }
    };
}

namespace std {
    /**
    * \brief Custom specialization of std::hash for Eigen::Vector3f.
    */
    template <>         // This defines the specialization
    struct hash<Eigen::Vector3f> {

        /**
         * \brief Computes a hash value for an Eigen::Vector3f object.
         * \param vec The Eigen::Vector3f object to hash.
         * \return The computed hash value.
         */
        std::size_t operator()(const Eigen::Vector3f& vec) const noexcept {
            std::size_t seed = 0;
            hashCombine(seed, vec.x(), vec.y(), vec.z());
            return seed;
        }
    };
}