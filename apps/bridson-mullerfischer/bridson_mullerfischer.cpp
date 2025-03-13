#include "bridson_mullerfischer.hpp"

#include "quad.hpp"
// #include "util/files.hpp"

BridsonMullerFischer::BridsonMullerFischer() {
}

void BridsonMullerFischer::init() {
    glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    // files::Json config = files::read_to_json(asset("config.json").c_str());
    // mConfig.rows = config["grid_rows"];
    // mConfig.cols = config["grid_cols"];
    // mConfig.cellSize = config["cell_size"];
    // mConfig.timestep = config["timestep"];

    // // Shader program
    // std::string vertex_shader =
    //     files::read_to_string(asset("shaders/shader.vs").c_str());
    // mProgram.addStage(gl::ShaderKind::Vertex, vertex_shader.c_str());

    // std::string fragment_shader =
    //     files::read_to_string(asset("shaders/shader.fs").c_str());
    // mProgram.addStage(gl::ShaderKind::Fragment, fragment_shader.c_str());

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

    // Texture
    mTexData.resize(mConfig.rows * mConfig.cols * 3);
    mProgram.setUniform<i32>("sampler", 0);

    // mSolver = std::make_unique<Solver>(mConfig);
}

void BridsonMullerFischer::update() {
    for (Index row = 0; row < mConfig.rows; ++row) {
        for (Index col = 0; col < mConfig.cols; ++col) {
            const Index i = row * mConfig.cols + col;
            const GLubyte d = 100;
            mTexData[i * 3] = d;
            mTexData[i * 3 + 1] = d;
            mTexData[i * 3 + 2] = d;
        }
    }
}

void BridsonMullerFischer::draw() {
    mTexture.image(mConfig.rows, mConfig.cols, mTexData.data());
    mTexture.bind(0);

    mQuadMesh.render();
}

void BridsonMullerFischer::onMouseMove(const Vector2D pos) {
    mPrevMousePos = mMousePos;
    mMousePos = pos;
}

void BridsonMullerFischer::onMouseButtonPress(int button, int actions,
                                              int mods) {
    mMouseButtons[button] = actions == GLFW_PRESS;
}

void BridsonMullerFischer::onKeyPress(int key, int action, int mods) {
    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_Q) {
            quit();
        }
    }
}

void BridsonMullerFischer::onFramebufferResize(const u32 width,
                                               const u32 height) {
    glViewport(0, 0, static_cast<GLsizei>(width), static_cast<GLsizei>(height));
}
