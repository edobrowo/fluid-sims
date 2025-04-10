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

class BridsonDensityLabelled : public Application {
public:
    BridsonDensityLabelled();
    ~BridsonDensityLabelled() = default;

    virtual void init() override;
    virtual void update() override;
    virtual void draw() override;

    virtual void onKeyPress(int key, int action, int mods) override;
    virtual void onFramebufferResize(const u32 width,
                                     const u32 height) override;

private:
    void saveCurrentFrame() const;

    gl::ShaderProgram mProgram;
    gl::Mesh mQuadMesh;

    Texture mTexture;
    std::vector<GLubyte> mTexData;

    std::unique_ptr<Solver> mSolver;
    Config mConfig;

    u32 mFrameCounter;

    bool mUpdateOnce;
    bool mUpdateContinuous;
};
