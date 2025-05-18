//
// Created by Hayden Chalin on 4/8/25.
//

#include "Transform.h"

#include <iostream>

/**
 * @class Transform
 * @brief A class for managing 3D transformations, including translation, rotation, and scaling.
 *
 * This class uses Eigen matrices to represent and manipulate 4x4 transformation matrices.
 * It provides methods to apply translation, rotation, and scaling transformations, as well as
 * to reset the transformation matrix to the identity matrix.
 */
Transform::Transform() {
    std::cout << "Transform::Transform()" << std::endl;
    transformMatrix = Eigen::Matrix4f::Identity();
}
/**
 * @brief Applies a translation transformation to the current transformation matrix.
 *
 * @param x The translation along the X-axis.
 * @param y The translation along the Y-axis.
 * @param z The translation along the Z-axis.
 */
void Transform::setTranslation(float x, float y, float z) {

    Eigen::Matrix4f translationMatrix = Eigen::Matrix4f::Identity();
    translationMatrix(0, 3) = x;
    translationMatrix(1, 3) = y;
    translationMatrix(2, 3) = z;

    transformMatrix = translationMatrix * transformMatrix;

}
/**
 * @brief Applies a rotation transformation to the current transformation matrix.
 *
 * @param angleRadians The rotation angle in radians.
 * @param x The X component of the rotation axis.
 * @param y The Y component of the rotation axis.
 * @param z The Z component of the rotation axis.
 */
void Transform::setRotation(float angleRadians, float x, float y, float z) {
    Eigen::Vector3f axis(x, y, z);          // Define axis of rotation as a vec3.
    Eigen::AngleAxisf rotation(angleRadians, axis.normalized());
    Eigen::Matrix3f rotMatrix = rotation.toRotationMatrix();

    Eigen::Matrix4f rotationMatrix = Eigen::Matrix4f::Identity();
    rotationMatrix.block<3,3>(0,0) = rotMatrix;

    transformMatrix = rotationMatrix * transformMatrix;
}
/**
 * @brief Applies a scaling transformation to the current transformation matrix.
 *
 * @param x The scaling factor along the X-axis.
 * @param y The scaling factor along the Y-axis.
 * @param z The scaling factor along the Z-axis.
 */
void Transform::setScale(float x, float y, float z) {
    Eigen::Matrix4f scaleMatrix = Eigen::Matrix4f::Identity();
    scaleMatrix(0, 0) = x;
    scaleMatrix(1, 1) = y;
    scaleMatrix(2, 2) = z;

    transformMatrix = scaleMatrix * transformMatrix;

}
/**
 * @brief Resets the transformation matrix to the identity matrix.
 */
void Transform::reset() {
    transformMatrix = Eigen::Matrix4f::Identity();
}
/**
 * @brief Retrieves the current transformation matrix.
 *
 * @return A constant reference to the 4x4 transformation matrix.
 */
const Eigen::Matrix4f& Transform::getMatrix() const {
    return transformMatrix;
}

/*
 *      Operator overloads  ---------------------
 */

/**
 * @brief Overloads the stream insertion operator to output the transformation matrix.
 *
 * @param os The output stream.
 * @param transform The Transform object to output.
 * @return A reference to the output stream.
 */
std::ostream& operator<<(std::ostream& os, const Transform& transform) {
    os << transform.getMatrix();
    return os;
}

//void operator*(float scale) {
    // TODO: fix this
    //Transform::setScale(scale,scale,scale);
//}