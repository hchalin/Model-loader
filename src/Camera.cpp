//
// Created by Hayden Chalin on 5/29/25.
//

#include "Camera.h"

Camera::Camera(const Vector3f& position, const Vector3f& target):
    camPos(position), camTarget(target) {
    /*
     *  In the next few lines I will set up the basis vectors required for the camera.
     *  Resource: https://learnopengl.com/Getting-started/Camera
     */
    // ^ Cameras direction
    camDirection = (camPos-camTarget).normalized();    // * the position minus the target - This will test perspective
    // ! Use this to NOT look at the origin.
    //camDirection = Vector3f(0, 0, 1);               // * Use this if you want no perspective
    //camDirection.normalize();

    // ^ camRight
    Vector3f up(0.0f, 1.0f, 0.0f);      // * Arbitrary up direction
    camRight = up.cross(camDirection);
    camRight.normalize();                   // Note: This is more efficient than using normalized()

    // ^ camUp
    camUp = camDirection.cross(camRight);

    // ^ Look At matrix
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

Matrix4f &Camera::getViewMatrix() {
    return viewMatrix;
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
    // ! check this logic, should this apply a rotation instead?
    // ! Move left / Righe
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
