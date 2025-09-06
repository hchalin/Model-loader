//
// Created by Hayden Chalin on 5/29/25.
//

#include "Camera.h"

#include <__ranges/transform_view.h>
using namespace Eigen;

Camera::Camera(const Vector3f& position, const Vector3f& target, float aRatio):
    camPos(position), camTarget(target), aRatio(aRatio){
    /*
     *  In the next few lines I will set up the basis vectors required for the camera.
     *  Resource: https://learnopengl.com/Getting-started/Camera
     */
    Vector3f up(0.0f, 1.0f, 0.0f);      // * Arbitrary up direction
    // ^ Cameras direction
    camDirection = (camPos-camTarget).normalized();    // * the position minus the target - This will test perspective
    // camDirection = (camTarget-camPos).normalized();    // * the position minus the target - This will test perspective
    camRight = up.cross(camDirection).normalized();
    // ^ camUp
    camUp = camDirection.cross(camRight);

    // @ Set up the transform


    setViewMatrix();            // I added this to set the viewMatrix on initialization
    std::cout << "aspect ratio: " << aRatio << std::endl;
    setProjectionMatrix(aRatio);


    viewProjectionMatrix = projectionMatrix * viewMatrix;

    std::cout << "view projection matrix: " << viewProjectionMatrix << std::endl;

}


void Camera::setViewMatrix() {
    // ^ View Matrix
    // Note: Eigen uses Column Major Order! So set the columns to the basis vectors
    // Note: https://eigen.tuxfamily.org/dox/group__TopicStorageOrders.html
    // ! This could be a problen area with CMO
    viewMatrix.setIdentity();           // * Reset
    // Rotation from basis vectors
    viewMatrix.block<3,1> (0,0) = camRight;
    viewMatrix.block<3,1> (0,1) = camUp;
    viewMatrix.block<3,1> (0,2) = camDirection;

    // Translation
    viewMatrix(0,3) = -camRight.dot(camPos);
    viewMatrix(1,3) = -camUp.dot(camPos);
    viewMatrix(2,3) = -camDirection.dot(camPos);


}

void Camera::setProjectionMatrix(const float aRatio) {

    // ^ Reset projectionMatrix
    projectionMatrix.setIdentity();

    float tanHalfFovy = std::tan(fovY / 2.0f);

    // Set up perspective matrix (column-major order in Eigen)
    projectionMatrix(0, 0) = 1.0f / (aRatio * tanHalfFovy); // Scale X
    projectionMatrix(1, 1) = 1.0f / tanHalfFovy; // Scale Y
    projectionMatrix(2, 2) = -(farPlane + nearPlane) / (farPlane - nearPlane); // Scale and translate Z
    projectionMatrix(2, 3) = -(2.0f * farPlane * nearPlane) / (farPlane - nearPlane); // Perspective divide term
    projectionMatrix(3, 2) = -1.0f; // Enables perspective division
    projectionMatrix(3, 3) = 0.0f; // Required for perspective
}

void Camera::updateAspectRatio(float aR) {
    aRatio = aR;
}


Matrix4f &Camera::getProjectionMatrix() {
    return projectionMatrix;
}
Matrix4f &Camera::getViewMatrix() {
    return viewMatrix;
}
Matrix4f &Camera::getViewProjectionMatrix() {
    return viewProjectionMatrix;
}

BroMath::Transform &Camera::getTransformMatrix() {
    return cameraTransform;
}

Vector3f &Camera::getPosition() {
    return camPos;
}

void Camera::moveUp(float dt) {
    // Move camera position along the up vector
    camPos += camUp * dt;

    // Update the view matrix's translation components
    viewMatrix(0,3) = -camRight.dot(camPos);
    viewMatrix(1,3) = -camUp.dot(camPos);
    viewMatrix(2,3) = -camDirection.dot(camPos);
}

void Camera::moveDown(float dt) {
    // Move camera position along the up vector
    camPos -= camUp * dt;

    // Update the view matrix's translation components
    viewMatrix(0,3) = -camRight.dot(camPos);
    viewMatrix(1,3) = -camUp.dot(camPos);
    viewMatrix(2,3) = -camDirection.dot(camPos);
}

void Camera::moveLeft(float dt) {
    camPos -= camRight * dt;
    viewMatrix(0,3) = -camRight.dot(camPos);
    viewMatrix(1,3) = -camUp.dot(camPos);
    viewMatrix(2,3) = -camDirection.dot(camPos);
}
void Camera::moveRight(float dt) {
    camPos += camRight * dt;
    viewMatrix(0,3) = -camRight.dot(camPos);
    viewMatrix(1,3) = -camUp.dot(camPos);
    viewMatrix(2,3) = -camDirection.dot(camPos);
}
void Camera::zoom(float aZoom) {
    camPos += camDirection * aZoom;
    viewMatrix(0,3) = -camRight.dot(camPos);
    viewMatrix(1,3) = -camUp.dot(camPos);
    viewMatrix(2,3) = -camDirection.dot(camPos);
}
void Camera::move(float aTurn) {
    camPos -= camRight * aTurn;
    viewMatrix(0,3) = -camRight.dot(camPos);
    viewMatrix(1,3) = -camUp.dot(camPos);
    viewMatrix(2,3) = -camDirection.dot(camPos);
}
void Camera::rotate(float xTurn) {
     // float to radian
    float degress = xTurn * (M_PI / 180.0f); // remap from 0-180?
    std::cout << "Degrees: " << degress << "*" << std::endl;
    /*
      | cos θ    0   sin θ| |x|   | x cos θ + z sin θ|   |x'|
    |   0      1       0| |y| = |         y        | = |y'|
    |−sin θ    0   cos θ| |z|   |−x sin θ + z cos θ|   |z'|
     create rotation matrix
     */
    Matrix4f rotationMatrix;
    rotationMatrix.setIdentity();
    // x
    rotationMatrix(0,0) = cos(degress);
    rotationMatrix(0,2) = sin(degress);
    // y
    rotationMatrix(2,0) = -sin(degress);
    rotationMatrix(2,2) = cos(degress);
    // z = 1
}

std::ostream & operator<<(std::ostream &os, const Camera &camera) {
    os << "_____________________\n";
 	os << "\033[4m Camera Information \033[24m\n ";
    os << "\t\033[4mPosition(x,y,z)\033[24m \n" << camera.camPos;
    os << "\n\t\033[4mDirection(x,y,z)\033[24m \n" << camera.camDirection;
    os << "\n_____________________\n";
    return os;
}
