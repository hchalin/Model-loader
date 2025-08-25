//
// Created by Hayden Chalin on 4/8/25.
//

#pragma once


#include <eigen/Eigen/Dense>
namespace BroMath {

/**
 * @class Transform
 * @brief A class to manage transformations (translation, rotation, scaling) for 3D objects.
 *
 * The Transform class is designed to encapsulate and manage transformation matrices
 * for objects in 3D space (only 2D right now). It provides methods to set and retrieve transformations
 * such as translation, rotation, and scaling, and computes the final model matrix (WIP)
 * that combines these transformations.
 *
 * Usage:
 * - Use this class to manage the position, orientation, and size of 3D objects.
 * - The class is intended to be used as a member of other classes, such as a Primitive.
 */
using Matrix4f = Eigen::Matrix4f;

class Transform final {
public:
    Transform();
    ~Transform() = default;

    void setTranslation(float x, float y, float z);
    void setRotation(float angleRadians, float x, float y, float z);
    void setScale(float x, float y, float z);

    // Reset to identity Matrix
    void reset();

    // Opertor overload
    friend std::ostream& operator<<(std::ostream& os, const Transform& transform);
    //friend void operator*(float scale);         // Scale entire matrix by single value

    const Matrix4f &getMatrix() const;

private:
    // Hide implementation
    //Matrix4f translation;
    Matrix4f transformMatrix;
};

}
