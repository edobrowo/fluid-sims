#include "example.hpp"

#include "geometry/cube.hpp"
#include "math/numeric.hpp"
#include "util/files.hpp"

const f32 Example::sAttenutation = 0.1f;

Example::Example()
    : mCamera(Vector3F(0.0f, 0.0f, 3.0f)),
      mFirstUpdate(true),
      mPrevMousePosition(0.0f),
      mYaw(-90.0f),
      mPitch(0.0f),
      mSpeed(0.05f),
      mDeltaTime(0.0f),
      mLastFrame(0.0f) {
}

Example::~Example() {
}

void Example::init() {
    glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Shader program
    std::string vertex_shader =
        files::read_to_string(asset("shaders/shader.vs").c_str());
    mProgram.addStage(gl::ShaderKind::Vertex, vertex_shader.c_str());

    std::string fragment_shader =
        files::read_to_string(asset("shaders/shader.fs").c_str());
    mProgram.addStage(gl::ShaderKind::Fragment, fragment_shader.c_str());

    mProgram.build();
    mProgram.use();

    // Vertex data and attributes
    mCubeMesh = geometry::Cube();
}

void Example::update() {
    const f64 current_frame = glfwGetTime();
    mDeltaTime = current_frame - mLastFrame;
    mLastFrame = current_frame;

    mSpeed = 2.5f * static_cast<f32>(mDeltaTime);

    if (getKeyRaw(GLFW_KEY_W) == GLFW_PRESS) {
        mCamera.setPosition(mCamera.position() + mSpeed * mCamera.front());
    }
    if (getKeyRaw(GLFW_KEY_S) == GLFW_PRESS) {
        mCamera.setPosition(mCamera.position() - mSpeed * mCamera.front());
    }
    if (getKeyRaw(GLFW_KEY_A) == GLFW_PRESS) {
        mCamera.setPosition(mCamera.position() - mSpeed * mCamera.right());
    }
    if (getKeyRaw(GLFW_KEY_D) == GLFW_PRESS) {
        mCamera.setPosition(mCamera.position() + mSpeed * mCamera.right());
    }
}

void Example::draw() {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glep();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glep();

    Matrix4F projection = perspective_fovy(
        math::rad(mCamera.fovY()), framebufferAspect(), 0.1f, 100.0f);
    mProgram.setUniform<Matrix4F>("projection", projection);

    const Matrix4F view = mCamera.view();
    mProgram.setUniform<Matrix4F>("view", view);

    // clang-format off
    const Vector3F cube_positions[] = {
        Vector3F(0.0f,  0.0f,  0.0f),
        Vector3F(2.0f,  5.0f,  -15.0f),
        Vector3F(-1.5f, -2.2f, -2.5f),
        Vector3F(-3.8f, -2.0f, -12.3f),
        Vector3F(2.4f,  -0.4f, -3.5f),
        Vector3F(-1.7f, 3.0f,  -7.5f),
        Vector3F(1.3f,  -2.0f, -2.5f),
        Vector3F(1.5f,  2.0f,  -2.5f),
        Vector3F(1.5f,  0.2f,  -1.5f),
        Vector3F(-1.3f, 1.0f,  -1.5f)
    };
    // clang-format on

    for (Index i = 0; i < 10; ++i) {
        Matrix4F model = translate(cube_positions[i]) *
                         rotate(static_cast<f32>(math::rad(i * 20.0f)),
                                Vector3F(1.0f, 0.3f, 0.5f));
        mProgram.setUniform<Matrix4F>("model", model);

        mCubeMesh.render();
    }
}

void Example::onMouseMove(const Vector2F pos) {
    if (mFirstUpdate) {
        mPrevMousePosition = pos;
        mFirstUpdate = false;
    }

    Vector2F offset(pos[0] - mPrevMousePosition[0],
                    mPrevMousePosition[1] - pos[1]);
    offset *= sAttenutation;

    mPrevMousePosition = pos;

    mYaw = mYaw + offset[0];
    mPitch = math::clamp(mPitch + offset[1], -89.0, 89.0);

    mCamera.rotate(mYaw, 0.0f, mPitch);
}

void Example::onMouseScroll(const Vector2F offset) {
    const f32 fovy = math::clamp(mCamera.fovY() - offset[1], 1.0f, 45.0f);
    mCamera.setFovY(fovy);
}

void Example::onKeyPress(int key, int action, int mods) {
    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_Q) {
            quit();
        }
    }
}

void Example::onFramebufferResize(const u32 width, const u32 height) {
    glViewport(0, 0, static_cast<GLsizei>(width), static_cast<GLsizei>(height));
}
