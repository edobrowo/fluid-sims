#pragma once

#include "math/common.hpp"
#include "math/transform.hpp"
#include "math/vector.hpp"
#include "util/common.hpp"

class Camera {
public:
    Camera(const Vector3D& pos);
    ~Camera() = default;

    Vector3D position() const;
    void setPosition(const Vector3D& pos);

    Vector3D front() const;
    Vector3D up() const;
    Vector3D right() const;

    Matrix4D view() const;

    void rotate(const f32 yaw, const f32 roll, const f32 pitch);
    void rotate(const Vector3D& euler);

    f32 fovY() const;
    void setFovY(const f32 fovy);

private:
    Vector3D mPosition;
    Vector3D mFront;
    Vector3D mUp;
    Vector3D mRight;
    f32 mFovY;
};
