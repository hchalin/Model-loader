//
// Created by Hayden Chalin on 5/29/25.
//

#pragma once
#include <eigen/Eigen/Dense>
#include <iostream>
#include <simd/simd.h>

#include "common/BroMath/Transform.h"

using namespace Eigen;

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
    Camera(const Vector3f& position, const Vector3f& target, float aRatio);

    Matrix4f &getViewMatrix();
    Matrix4f &getProjectionMatrix();
    Matrix4f &getViewProjectionMatrix();
    BroMath::Transform &getTransformMatrix();
    Vector3f &getPosition();

    void updateAspectRatio(float aR);

    // Movement
    void moveUp(float dt);
    void moveDown(float dt);
    void moveLeft(float dt);
    void moveRight(float dt);
    void zoom(float aZoom);
    void move(float aTurn);
    void rotate(float xTurn);   // Only around y basis vector

    private:
    const float fovY {45.0f * (M_PI / 180.0f)};
    const float nearPlane {0.1f};
    const float farPlane {100.0f};
    float aRatio;
    Vector3f camPos;
    Vector3f camTarget = Vector3f(0,0,0); // origin
    Vector3f camDirection;
    Vector3f camLookAt;
    Vector3f camUp;
    Vector3f camRight;


    BroMath::Transform cameraTransform;
    Matrix4f projectionMatrix;  // Note: Camera Space -> clip space
    Matrix4f viewMatrix;     // Note: World Space -> Camera Space
    Matrix4f viewProjectionMatrix;

    void setViewMatrix();
    void setProjectionMatrix(const float aRatio);
    friend std::ostream & operator<<(std::ostream &os, const Camera &camera);
};

