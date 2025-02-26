#include "stam_rtfd.hpp"

#include "quad.hpp"
#include "util/files.hpp"

// TODO: (GUI)
// - force attenuation
// - borders on or off
// - fluid fidelity
// - fluid color
// - gravity

StamRTFD::StamRTFD() : mTexData(N * N * 3) {
}

void StamRTFD::init() {
    glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    // Shader program
    std::string vertex_shader =
        read_file_to_string("apps/stam-rtfd/shaders/shader.vs");
    mProgram.addStage(gl::ShaderKind::Vertex, vertex_shader.c_str());

    std::string fragment_shader =
        read_file_to_string("apps/stam-rtfd/shaders/shader.fs");
    mProgram.addStage(gl::ShaderKind::Fragment, fragment_shader.c_str());

    mProgram.build();

    glUseProgram(mProgram.handle());
    glep();

    const Matrix4D projection =
        orthographic(0.0f, static_cast<f32>(mWindowWidth), 0.0f,
                     static_cast<f32>(mWindowHeight), -1.0f, 1.0f);
    mProgram.setUniform<Matrix4D>("projection", projection);

    Matrix4D model =
        scale(Vector3D(mWindowWidth, mWindowHeight, 0.0f)) * translate(0.5f);
    mProgram.setUniform<Matrix4D>("model", model);

    // Vertex data and attributes
    mQuadMesh = Quad();

    // Solver
    mSolver.init();

    // Texture
    mProgram.setUniform<i32>("sampler", 0);
}

void StamRTFD::update() {
    mSolver.reset();

    if (mMouseButtons[GLFW_MOUSE_BUTTON_RIGHT]) {
        const Index col = mMousePos[0] / static_cast<f32>(mWindowWidth) * N;
        const Index row =
            N - mMousePos[1] / static_cast<f32>(mWindowHeight) * N;
        mSolver.addDensity(row, col, 10.0f);
    }

    if (mMouseButtons[GLFW_MOUSE_BUTTON_LEFT]) {
        const Vector2D force = Vector2D(mMousePos[0] - mPrevMousePos[0],
                                        mPrevMousePos[1] - mMousePos[1]) /
                               2.0f;
        const Index col = mMousePos[0] / static_cast<f32>(mWindowWidth) * N;
        const Index row =
            N - mMousePos[1] / static_cast<f32>(mWindowHeight) * N;
        mSolver.addVelocity(row, col, force);
    }

    for (Index row = 1; row <= N; ++row) {
        for (Index col = 1; col <= N; ++col) {
            const Index i = (row - 1) * N + (col - 1);
            GLubyte d = static_cast<GLubyte>(
                math::clamp(mSolver.density()(row, col), 0.0f, 1.0f) * 255.0f);
            mTexData[i * 3] = static_cast<GLubyte>(d);
            mTexData[i * 3 + 1] = static_cast<GLubyte>(d);
            mTexData[i * 3 + 2] = static_cast<GLubyte>(d);
        }
    }

    mSolver.step();
}

void StamRTFD::draw() {
    mTexture.image(N, N, mTexData.data());
    mTexture.bind(0);

    mQuadMesh.render();
}

void StamRTFD::onMouseMove(const Vector2D pos) {
    mPrevMousePos = mMousePos;
    mMousePos = pos;
}

void StamRTFD::onMouseButtonPress(int button, int actions, int mods) {
    mMouseButtons[button] = actions == GLFW_PRESS;
}

void StamRTFD::onKeyPress(int key, int action, int mods) {
    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_Q) {
            quit();
        }
    }
}

void StamRTFD::onFramebufferResize(const u32 width, const u32 height) {
    glViewport(0, 0, static_cast<GLsizei>(width), static_cast<GLsizei>(height));
}
