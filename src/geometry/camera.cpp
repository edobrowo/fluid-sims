#include "camera.hpp"

Camera::Camera(const Vector3F& pos)
    : mPosition(pos),
      mFront(Vector3F(0.0f, 0.0f, -1.0f)),
      mUp(Vector3F(0.0f, 1.0f, 0.0f)),
      mRight(cross(mFront, mUp).normalized()),
      mFovY(45.0f) {
}

Vector3F Camera::position() const {
    return mPosition;
}

void Camera::setPosition(const Vector3F& pos) {
    mPosition = pos;
}

Vector3F Camera::front() const {
    return mFront;
}

Vector3F Camera::up() const {
    return mUp;
}

Vector3F Camera::right() const {
    return mRight;
}

Matrix4F Camera::view() const {
    return lookat(mPosition, mPosition + mFront, mUp);
}

void Camera::rotate(const f32 yaw, const f32 roll, const f32 pitch) {
    const Vector3F front(
        static_cast<f32>(::cos(math::rad(yaw)) * ::cos(math::rad(pitch))),
        static_cast<f32>(::sin(math::rad(pitch))),
        static_cast<f32>(::sin(math::rad(yaw)) * ::cos(math::rad(pitch))));
    mFront = front.normalized();
    mRight = cross(mFront, mUp).normalized();
}

void Camera::rotate(const Vector3F& euler) {
    return rotate(euler[0], euler[1], euler[2]);
}

f32 Camera::fovY() const {
    return mFovY;
}

void Camera::setFovY(const f32 fovy) {
    mFovY = fovy;
}
