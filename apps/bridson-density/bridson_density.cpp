#include "bridson_density.hpp"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#include <iomanip>
#include <sstream>

#include "quad.hpp"
#include "util/files.hpp"

BridsonDensity::BridsonDensity()
    : mUpdateOnce(false), mUpdateContinuous(false) {
}

void BridsonDensity::init() {
    glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    // Load config.
    mConfig = Config::loadFromJson(asset("config.json"));

    // Create shader program.
    std::string vertex_shader =
        files::read_to_string(asset("shaders/shader.vs").c_str());
    mProgram.addStage(gl::ShaderKind::Vertex, vertex_shader.c_str());

    std::string fragment_shader =
        files::read_to_string(asset("shaders/shader.fs").c_str());
    mProgram.addStage(gl::ShaderKind::Fragment, fragment_shader.c_str());

    mProgram.build();
    mProgram.use();

    // Set up 2D projection and textured quad.
    const Matrix4F projection = orthographic(0.0f,
                                             static_cast<f32>(mWindowWidth),
                                             0.0f,
                                             static_cast<f32>(mWindowHeight),
                                             -1.0f,
                                             1.0f);
    mProgram.setUniform<Matrix4F>("projection", projection);

    Matrix4F model =
        scale(Vector3F(mWindowWidth, mWindowHeight, 0.0f)) * translate(0.5f);
    mProgram.setUniform<Matrix4F>("model", model);

    mQuadMesh = Quad();

    mTexData.resize(mConfig.rows * mConfig.cols * 3);
    mProgram.setUniform<i32>("sampler", 0);

    // Initialize the solver.
    mSolver = std::make_unique<Solver>(mConfig);
}

void BridsonDensity::update() {
    if (mUpdateOnce || mUpdateContinuous) {
        mSolver->step();
        mUpdateOnce = false;
    }
}

void BridsonDensity::draw() {
    println("Frame {}", mFrameCounter);

    for (Index row = 0; row < mConfig.rows; ++row) {
        for (Index col = 0; col < mConfig.cols; ++col) {
            const f64 d = mSolver->color(
                Vector2i(static_cast<i32>(col), static_cast<i32>(row)));

            const GLubyte b =
                static_cast<GLubyte>(math::clamp(d, 0.0, 1.0) * 255.0);

            const Index i = row * mConfig.cols + col;
            mTexData[i * 3] = b;
            mTexData[i * 3 + 1] = b;
            mTexData[i * 3 + 2] = b;
        }
    }

    mTexture.image(mConfig.rows, mConfig.cols, mTexData.data());
    mTexture.bind(0);

    mQuadMesh.render();

    if (mConfig.saveFrames) {
        saveCurrentFrame();
    }

    ++mFrameCounter;
}

void BridsonDensity::onKeyPress(int key, int action, int mods) {
    if (action == GLFW_PRESS) {
        switch (key) {
        case GLFW_KEY_Q:
            quit();
            break;
        case GLFW_KEY_SPACE:
            mUpdateContinuous = !mUpdateContinuous;
            break;
        case GLFW_KEY_N:
            mUpdateOnce = true;
            break;
        case GLFW_KEY_R:
            mSolver = std::make_unique<Solver>(mConfig);
            break;
        case GLFW_KEY_D:
            // Print density.
            println("DENSITY\n{}", mSolver->density());
            break;
        case GLFW_KEY_P:
            // Print pressure.
            println("PRESSURE\n{}", mSolver->pressure());
            break;
        case GLFW_KEY_U:
            // Print velocity u component.
            println("U\n{}", mSolver->u());
            break;
        case GLFW_KEY_V:
            // Print velocity v component.
            println("V\n{}", mSolver->v());
            break;
        default:
            break;
        }
    }
}

void BridsonDensity::onFramebufferResize(const u32 width, const u32 height) {
    glViewport(0, 0, static_cast<GLsizei>(width), static_cast<GLsizei>(height));
}

void BridsonDensity::saveCurrentFrame() const {
    if (mTexData.empty()) {
        eprintln("mTextData empty");
        return;
    }

    std::ostringstream filenameStream;
    filenameStream << "Frame_" << std::setw(5) << std::setfill('0')
                   << mFrameCounter << ".png";
    const std::string filename = filenameStream.str();
    const std::string path = root() + "/frames/" + filename;

    const int error = stbi_write_png(path.c_str(),
                                     static_cast<int>(mConfig.cols),
                                     static_cast<int>(mConfig.rows),
                                     3,
                                     mTexData.data(),
                                     static_cast<int>(mConfig.cols * 3));

    if (!error) {
        eprintln("Failed to save the current frame");
    }
}
