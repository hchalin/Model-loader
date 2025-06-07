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
    camDirection = (camPos-camTarget).normalized();    // * the position minus the target
    // ! Use this to NOT look at the origin.
    camDirection = Vector3f(0, 0, 1);
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
void Camera::turn(float aTurn) {
    // ! check this logic
    camPos -= camRight * aTurn;
    viewMatrix(0,3) = -camRight.dot(camPos);
    viewMatrix(1,3) = -camUp.dot(camPos);
    viewMatrix(2,3) = -camDirection.dot(camPos);
}
