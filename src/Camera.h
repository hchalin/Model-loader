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
    Vector3f &getPosition();

    // Movement
    void moveUp(float dt);
    void moveDown(float dt);
    void moveLeft(float dt);
    void moveRight(float dt);
    void zoom(float aZoom);
    void move(float aTurn);
    void rotate(float xTurn);   // Only around y basis vector

    private:
    Vector3f camPos;
    Vector3f camTarget = Vector3f(0,0,0); // origin
    Vector3f camDirection;
    Vector3f camLookAt;
    Vector3f camUp;
    Vector3f camRight;


    Matrix4f viewMatrix;     // Note: World Space -> Camera Space

    friend std::ostream & operator<<(std::ostream &os, const Camera &camera);
};

