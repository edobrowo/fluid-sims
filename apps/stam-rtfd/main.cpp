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
#include "platform/texture.hpp"
#include "stam_rtfd_2d.hpp"
#include "util/common.hpp"
#include "util/format.hpp"

constexpr int gWindowWidth = 800;
constexpr int gWindowHeight = 600;

void on_glfw_error(int error_code, const char* description) {
    eprintln("GLFW error {}: {}", error_code, description);
}

void on_framebuffer_size_change(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
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

class Quad : public geometry::Mesh {
public:
    Quad() {
        // clang-format off
        mVertices = {
            {{-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f}},
            {{ 0.5f, -0.5f, 0.0f}, {1.0f, 0.0f}},
            {{-0.5f,  0.5f, 0.0f}, {0.0f, 1.0f}},
            {{ 0.5f,  0.5f, 0.0f}, {1.0f, 1.0f}},
        };

        mIndices = {
            0, 3, 2,
            0, 1, 3,
        };
        // clang-format on
    }

    ~Quad() = default;
};

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

    GLFWwindow* window = glfwCreateWindow(gWindowWidth, gWindowHeight,
                                          "Stam RTFD 2D", NULL, NULL);
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

    // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Depth buffer ------------------------------------------------------------
    glDisable(GL_DEPTH_TEST);

    // Shader program ----------------------------------------------------------
    gl::ShaderProgram program;

    std::string vertex_shader =
        read_file_to_string("apps/stam-rtfd/shaders/shader.vs");
    program.addStage(gl::ShaderKind::Vertex, vertex_shader.c_str());

    std::string fragment_shader =
        read_file_to_string("apps/stam-rtfd/shaders/shader.fs");
    program.addStage(gl::ShaderKind::Fragment, fragment_shader.c_str());

    program.build();

    // Vertex data and attributes ----------------------------------------------
    Quad quad_mesh;

    glUseProgram(program.handle());
    glep();

    // Stam RTFD 2D solver -----------------------------------------------------
    constexpr u32 N = 100;
    StamRTFDSolver<N> solver;
    solver.init();

    // Texture -----------------------------------------------------------------
    Texture texture;
    program.setUniform<i32>("sampler", 0);

    std::vector<GLubyte> tex_data(N * N * 3);

    // Render loop -------------------------------------------------------------
    gl::Mesh gl_mesh(quad_mesh);

    glUseProgram(program.handle());
    glep();

    Matrix4D projection =
        orthographic(0.0f, static_cast<f32>(gWindowWidth), 0.0f,
                     static_cast<f32>(gWindowHeight), -1.0f, 1.0f);
    program.setUniform<Matrix4D>("projection", projection);

    Matrix4D model =
        scale(Vector3D(gWindowWidth, gWindowHeight, 0.0f)) * translate(0.5f);
    program.setUniform<Matrix4D>("model", model);

    while (!glfwWindowShouldClose(window)) {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        glClearColor(0.5f, 0.5f, 0.75f, 1.0f);
        glep();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glep();

        solver.reset();

        for (Index row = 1; row <= N; ++row) {
            for (Index col = 1; col <= N; ++col) {
                const Index i = (row - 1) * N + (col - 1);
                const GLubyte d = static_cast<GLubyte>(
                    math::clamp(solver.density()(row, col), 0.0f, 1.0f) *
                    255.0f);
                tex_data[i * 3] = static_cast<GLubyte>(d);
                tex_data[i * 3 + 1] = static_cast<GLubyte>(d);
                tex_data[i * 3 + 2] = static_cast<GLubyte>(d);
            }
        }

        solver.step();

        texture.image(N, N, tex_data.data());
        texture.bind(0);

        gl_mesh.render();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
