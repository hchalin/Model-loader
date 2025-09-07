//
// Created by Hayden Chalin on 5/29/25.
//

#pragma once
#include <eigen/Eigen/Dense>
#include <iostream>

#include "Window.h"
#include "common/BroMath/Transform.h"


// TODO: the cameras view matrix and projection matrix needs to be derived from the transform class,
// TODO: (cont) not the way it is now, setting the transform based off the view/proj matrices

// Camera invariants / TODO:
// - Single source of truth: Camera::transform (position + rotation; no scale).
// - viewMatrix = inverseRigid(transform)  // recompute whenever transform changes
// - projectionMatrix = perspective(fovY, aspect, near, far)  // recompute on param changes
// - viewProjectionMatrix = projectionMatrix * viewMatrix
// Avoid setting transform FROM view/projection. If using lookAt, set transform from target,
// then rebuild view (and VP) from transform.


class Camera {
    public:
    Camera(const Eigen::Vector3f& position, const Eigen::Vector3f& target, float aRatio);

    Eigen::Matrix4f &getViewMatrix();
    Eigen::Matrix4f &getProjectionMatrix();
    Eigen::Matrix4f &getViewProjectionMatrix();
    BroMath::Transform &getTransformMatrix();
    Eigen::Vector3f &getPosition();

    void updateAspectRatio(float aspect_ratio);

    // Movement
    void moveUp(float dt);
    void moveDown(float dt);
    void moveLeft(float dt);
    void moveRight(float dt);
    void zoom(float aZoom);
    void move(float aTurn);
    void rotate(float xTurn);   // Only around y basis vector

    void setProjectionMatrix(const float aR);

private:
    const float fovY {45.0f * (M_PI / 180.0f)};
    const float nearPlane {0.1f};
    const float farPlane {100.0f};
    float aRatio;
    Eigen::Vector3f camPos;
    Eigen::Vector3f camTarget = Eigen::Vector3f(0,0,0); // origin
    Eigen::Vector3f camDirection;
    Eigen::Vector3f camLookAt;
    Eigen::Vector3f camUp;
    Eigen::Vector3f camRight;


    BroMath::Transform cameraTransform;
    Eigen::Matrix4f projectionMatrix;  // Note: Camera Space -> clip space
    Eigen::Matrix4f viewMatrix;     // Note: World Space -> Camera Space
    Eigen::Matrix4f viewProjectionMatrix;

    void setViewMatrix();
    friend std::ostream & operator<<(std::ostream &os, const Camera &camera);
};

