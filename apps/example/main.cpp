#include <array>
#include <fstream>
#include <sstream>

#include "geometry/camera.hpp"
#include "geometry/cube.hpp"
#include "math/common.hpp"
#include "math/matrix.hpp"
#include "math/transform.hpp"
#include "math/vector.hpp"
#include "platform/mesh.hpp"
#include "platform/opengl.hpp"
#include "platform/shader.hpp"
#include "util/common.hpp"
#include "util/format.hpp"

constexpr int gWindowWidth = 800;
constexpr int gWindowHeight = 600;
constexpr f32 gAspect =
    static_cast<f32>(gWindowWidth) / static_cast<f32>(gWindowHeight);

constexpr f32 gMoveSensitivty = 0.1f;

Camera gCamera(Vector3D(0.0f, 0.0f, 3.0f));

namespace mouse {

bool first_update = true;
Vector2D last_pos(static_cast<f32>(gWindowWidth) / 2.0f,
                  static_cast<f32>(gWindowHeight) / 2.0f);
Vector2D scroll_offset;

f64 yaw = -90.0f;
f64 pitch = 0.0f;
}

void on_glfw_error(int error_code, const char* description) {
    eprintln("GLFW error {}: {}", error_code, description);
}

void on_framebuffer_size_change(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void on_mouse_pos_change(GLFWwindow* window, double xpos, double ypos) {
    const Vector2D pos =
        Vector2D(static_cast<f32>(xpos), static_cast<f32>(ypos));

    if (mouse::first_update) {
        mouse::last_pos = pos;
        mouse::first_update = false;
    }

    Vector2D offset(pos[0] - mouse::last_pos[0], mouse::last_pos[1] - pos[1]);
    offset *= gMoveSensitivty;

    mouse::last_pos = pos;

    mouse::yaw = mouse::yaw + offset[0];
    mouse::pitch = math::clamp(mouse::pitch + offset[1], -89.0, 89.0);

    gCamera.rotate(mouse::yaw, 0.0f, mouse::pitch);
}

void on_mouse_scroll_change(GLFWwindow* window, double xoffset,
                            double yoffset) {
    mouse::scroll_offset =
        Vector2D(static_cast<f32>(xoffset), static_cast<f32>(yoffset));

    f32 fovy =
        math::clamp(gCamera.fovY() - mouse::scroll_offset[1], 1.0f, 45.0f);
    gCamera.setFovY(fovy);
}

std::string read_file_to_string(const char* path) {
    std::ifstream file_stream;
    file_stream.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    std::string result;
    std::stringstream string_stream;
    try {
        file_stream.open(path);
        string_stream << file_stream.rdbuf();
        file_stream.close();
        result = string_stream.str();
    } catch (std::ifstream::failure& e) {
        eprintln("File could not be read: {}", e.what());
    }

    return result;
}

int main() {
    // GLFW --------------------------------------------------------------------

    if (!glfwInit()) {
        eprintln("failed to initialize GLFW");
        return -1;
    }

    glfwSetErrorCallback(on_glfw_error);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window =
        glfwCreateWindow(gWindowWidth, gWindowHeight, "Example", NULL, NULL);
    if (window == NULL) {
        eprintln("failed to create GLFW window");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    // Set the scaled viewport size
    {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, static_cast<GLint>(width), static_cast<GLint>(height));
    }

    glfwSetFramebufferSizeCallback(window, on_framebuffer_size_change);
    glfwSetCursorPosCallback(window, on_mouse_pos_change);
    glfwSetScrollCallback(window, on_mouse_scroll_change);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Depth buffer ------------------------------------------------------------
    glEnable(GL_DEPTH_TEST);

    // Shader program ----------------------------------------------------------
    gl::ShaderProgram program;

    std::string vertex_shader =
        read_file_to_string("apps/example/shaders/shader.vs");
    program.addStage(gl::ShaderKind::Vertex, vertex_shader.c_str());

    std::string fragment_shader =
        read_file_to_string("apps/example/shaders/shader.fs");
    program.addStage(gl::ShaderKind::Fragment, fragment_shader.c_str());

    program.build();

    // Vertex data and attributes ----------------------------------------------
    geometry::Cube cube_mesh;

    // clang-format off
    const Vector3D cube_positions[] = {
        Vector3D(0.0f,  0.0f,  0.0f),
        Vector3D(2.0f,  5.0f,  -15.0f),
        Vector3D(-1.5f, -2.2f, -2.5f),
        Vector3D(-3.8f, -2.0f, -12.3f),
        Vector3D(2.4f,  -0.4f, -3.5f),
        Vector3D(-1.7f, 3.0f,  -7.5f),
        Vector3D(1.3f,  -2.0f, -2.5f),
        Vector3D(1.5f,  2.0f,  -2.5f),
        Vector3D(1.5f,  0.2f,  -1.5f),
        Vector3D(-1.3f, 1.0f,  -1.5f)
    };
    // clang-format on

    glUseProgram(program.handle());
    glep();

    // Render loop -------------------------------------------------------------
    f32 speed = 0.05f;

    f64 delta_time = 0.0f;
    f64 last_frame = 0.0f;

    gl::Mesh gl_mesh(cube_mesh);

    while (!glfwWindowShouldClose(window)) {
        const f64 current_frame = glfwGetTime();
        delta_time = current_frame - last_frame;
        last_frame = current_frame;

        speed = 2.5f * static_cast<f32>(delta_time);

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            gCamera.setPosition(gCamera.position() + speed * gCamera.front());
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            gCamera.setPosition(gCamera.position() - speed * gCamera.front());
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            gCamera.setPosition(gCamera.position() - speed * gCamera.right());
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            gCamera.setPosition(gCamera.position() + speed * gCamera.right());

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glep();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glep();

        glUseProgram(program.handle());
        glep();

        Matrix4D projection =
            perspective_fovy(math::rad(gCamera.fovY()), gAspect, 0.1f, 100.0f);
        program.setUniform<Matrix4D>("projection", projection);

        const Matrix4D view = gCamera.view();
        program.setUniform<Matrix4D>("view", view);

        for (Index i = 0; i < 10; ++i) {
            Matrix4D model = translate(cube_positions[i]) *
                             rotate(static_cast<f32>(math::rad(i * 20.0f)),
                                    Vector3D(1.0f, 0.3f, 0.5f));
            program.setUniform<Matrix4D>("model", model);

            gl_mesh.render();
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
