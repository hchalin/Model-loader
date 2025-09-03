//
// Created by Hayden Chalin on 5/29/25.
//

#pragma once
#include <eigen/Eigen/Dense>
#include <iostream>
#include <simd/simd.h>

using namespace Eigen;


class Camera {
    public:
    Camera(const Vector3f& position, const Vector3f& target);

    Matrix4f &getViewMatrix();
    Matrix4f &getProjectionMatrix();
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


    Matrix4f projectionMatrix;
    Matrix4f viewMatrix;     // Note: World Space -> Camera Space

    void setViewMatrix();
    void setProjectionMatrix(const float aRatio);
    friend std::ostream & operator<<(std::ostream &os, const Camera &camera);
};

