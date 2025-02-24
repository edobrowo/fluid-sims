#pragma once

#include "application/application.hpp"
#include "geometry/camera.hpp"
#include "platform/mesh.hpp"
#include "platform/shader.hpp"

class Example : public Application {
public:
    Example();
    virtual ~Example();

protected:
    virtual void init() override;
    virtual void update() override;
    virtual void draw() override;

    virtual void onMouseMove(const Vector2D pos) override;
    virtual void onMouseScroll(const Vector2D offset) override;
    virtual void onKeyPress(int key, int action, int mods) override;
    virtual void onFramebufferResize(const u32 width,
                                     const u32 height) override;

private:
    static const f32 sAttenutation;

    gl::ShaderProgram mProgram;
    gl::Mesh mCubeMesh;

    Camera mCamera;

    bool mFirstUpdate;
    Vector2D mPrevMousePosition;
    f64 mYaw;
    f64 mPitch;

    f32 mSpeed;
    f64 mDeltaTime;
    f64 mLastFrame;
};
