#pragma once

#include "math/arithmetic.hpp"
#include "math/transform.hpp"
#include "math/vector.hpp"
#include "util/common.hpp"

class Camera {
public:
    Camera(const Vector3F& pos);
    ~Camera() = default;

    Vector3F position() const;
    void setPosition(const Vector3F& pos);

    Vector3F front() const;
    Vector3F up() const;
    Vector3F right() const;

    Matrix4F view() const;

    void rotate(const f32 yaw, const f32 roll, const f32 pitch);
    void rotate(const Vector3F& euler);

    f32 fovY() const;
    void setFovY(const f32 fovy);

private:
    Vector3F mPosition;
    Vector3F mFront;
    Vector3F mUp;
    Vector3F mRight;
    f32 mFovY;
};
