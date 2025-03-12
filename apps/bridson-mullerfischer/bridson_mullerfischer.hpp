#pragma once

#include <memory>
#include <vector>

#include "application/application.hpp"
#include "config.hpp"
#include "geometry/camera.hpp"
#include "platform/mesh.hpp"
#include "platform/shader.hpp"
#include "platform/texture.hpp"
#include "solver.hpp"

class BridsonMullerFischer : public Application {
public:
    BridsonMullerFischer();
    ~BridsonMullerFischer() = default;

    virtual void init() override;
    virtual void update() override;
    virtual void draw() override;

    virtual void onMouseMove(const Vector2D pos) override;
    virtual void onMouseButtonPress(int button, int actions, int mods) override;
    virtual void onKeyPress(int key, int action, int mods) override;
    virtual void onFramebufferResize(const u32 width,
                                     const u32 height) override;

private:
    gl::ShaderProgram mProgram;
    gl::Mesh mQuadMesh;

    Texture mTexture;
    std::vector<GLubyte> mTexData;

    bool mMouseButtons[GLFW_MOUSE_BUTTON_LAST + 1];
    Vector2D mPrevMousePos;
    Vector2D mMousePos;

    std::unique_ptr<Solver> mSolver;
    Config mConfig;
};
