#pragma once

#include <vector>

#include "application/application.hpp"
#include "geometry/camera.hpp"
#include "platform/mesh.hpp"
#include "platform/shader.hpp"
#include "platform/texture.hpp"
#include "stam_rtfd_solver.hpp"

class StamRTFD : public Application {
public:
    StamRTFD();
    ~StamRTFD() = default;

    virtual void init() override;
    virtual void update() override;
    virtual void draw() override;

    virtual void onMouseMove(const Vector2D pos) override;
    virtual void onMouseButtonPress(int button, int actions, int mods) override;
    virtual void onKeyPress(int key, int action, int mods) override;
    virtual void onFramebufferResize(const u32 width,
                                     const u32 height) override;

private:
    static constexpr u32 N = 100;

    StamRTFDSolver<N> mSolver;

    gl::ShaderProgram mProgram;
    gl::Mesh mQuadMesh;

    Texture mTexture;
    std::vector<GLubyte> mTexData;

    bool mMouseButtons[GLFW_MOUSE_BUTTON_LAST + 1];
    Vector2D mPrevMousePos;
    Vector2D mMousePos;

    struct Config {
        f32 timestep;
        f32 viscosity;
        f32 diffusion;
        u32 gaussSeidelIterations;

        f32 densityIncrement;
        f32 forceMultiplier;

    } mConfig;
};
