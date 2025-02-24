#pragma once

#include <memory>

#include "math/vector.hpp"
#include "platform/opengl.hpp"
#include "util/common.hpp"

class Application;

template <typename T>
concept ApplicationSubclass = std::is_base_of_v<Application, T>;

class Application {
public:
    virtual ~Application();

    /// @brief Launch the application.
    /// @tparam ...Args Application argument types.
    /// @tparam T Application subclass type.
    /// @param application Pointer to the application object.
    /// @param width Window width.
    /// @param height Window height.
    /// @param title Window title.
    /// @param fps Framerate.
    /// @param ...args Application arguments.
    template <ApplicationSubclass T, typename... Args>
    static void launch(const u32 width, const u32 height,
                       const std::string& title, f32 fps, Args&&... args);

protected:
    Application();

    /// @brief Run before the event loop begins.
    virtual void init();

    /// @brief Run at the start of every loop.
    virtual void update();

    /// @brief Run every loop iteration after `update()`.
    virtual void gui();

    /// @brief Run every loop iteration after `gui()`.
    virtual void draw();

    /// @brief Run after the event loop ends.
    virtual void cleanup();

    /// @brief Get the current state of the specified key.
    /// @param key GLFW keycode.
    /// @return Key state.
    int getKeyRaw(int key);

    /// @brief Window aspect ratio.
    /// @return Aspect ratio of the window (width / height).
    f32 windowAspect() const;

    /// @brief Framebuffer aspect ratio.
    /// @return Aspect ratio of the framebuffer (width / height).
    f32 framebufferAspect() const;

    /// @brief Quits the application.
    void quit() const;

    GLFWwindow* mWindow;
    std::string mWindowTitle;
    u32 mWindowWidth;
    u32 mWindowHeight;
    u32 mFramebufferWidth;
    u32 mFramebufferHeight;

    /// Virtual events.
    virtual void onCursorEnterWindow(int entered);
    virtual void onMouseMove(const Vector2D pos);
    virtual void onMouseButtonPress(int button, int actions, int mods);
    virtual void onMouseScroll(const Vector2D offset);
    virtual void onKeyPress(int key, int action, int mods);
    virtual void onWindowResize(const u32 width, const u32 height);
    virtual void onFramebufferResize(const u32 width, const u32 height);

private:
    /// @brief Retrieve the application singleton instance.
    /// @return Application instance.
    static std::shared_ptr<Application> getInstance();

    /// @brief Start the application loop.
    /// @param width Window width.
    /// @param height Window height.
    /// @param title Window title.
    /// @param fps Framerate.
    template <ApplicationSubclass T, typename... Args>
    static void run(const u32 width, const u32 height, const std::string& title,
                    f32 fps, Args&&... args);

    /// @brief Register all GLFW callbacks.
    void registerGlfwCallbacks();

    static void errorCallback(int error, const char* description);
    static void cursorEnterWindowCallBack(GLFWwindow* window, int entered);
    static void mouseMoveCallBack(GLFWwindow* window, double xPos, double yPos);
    static void mouseButtonPressCallBack(GLFWwindow* window, int button,
                                         int actions, int mods);
    static void mouseScrollCallBack(GLFWwindow* window, double xOffSet,
                                    double yOffSet);
    static void keyPressCallBack(GLFWwindow* window, int key, int scancode,
                                 int action, int mods);
    static void windowResizeCallBack(GLFWwindow* window, int width, int height);
    static void framebufferResizeCallback(GLFWwindow* window, int width,
                                          int height);

    static std::shared_ptr<Application> mInstance;
    GLFWmonitor* mMonitor;
};

template <ApplicationSubclass T, typename... Args>
void Application::launch(const u32 width, const u32 height,
                         const std::string& title, f32 fps, Args&&... args) {
    if (mInstance == nullptr) {
        Application::run<T, Args...>(width, height, title, fps, args...);
    }
}

template <ApplicationSubclass T, typename... Args>
void Application::run(const u32 width, const u32 height,
                      const std::string& title, f32 fps, Args&&... args) {
    if (!glfwInit()) {
        eprintln("failed to initialize GLFW");
        return;
    }

    glfwSetErrorCallback(errorCallback);

    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_VISIBLE, GL_TRUE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    if (monitor == NULL) {
        glfwTerminate();
        eprintln("failed to retrieve primary monitor");
        return;
    }

    GLFWwindow* window =
        glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
    if (window == NULL) {
        eprintln("failed to create GLFW window");
        glfwTerminate();
        return;
    }

    int framebuffer_width;
    int framebuffer_height;
    glfwGetFramebufferSize(window, &framebuffer_width, &framebuffer_height);

    glfwMakeContextCurrent(window);

    Application* application = new T(args...);
    mInstance = std::shared_ptr<Application>(application);

    mInstance->mWindow = window;
    mInstance->mWindowTitle = title;
    mInstance->mWindowWidth = width;
    mInstance->mWindowHeight = height;
    mInstance->mFramebufferWidth = static_cast<u32>(framebuffer_width);
    mInstance->mFramebufferHeight = static_cast<u32>(framebuffer_height);
    mInstance->mMonitor = monitor;

    mInstance->registerGlfwCallbacks();

    // TODO: init imgui

    mInstance->init();

    while (!glfwWindowShouldClose(mInstance->mWindow)) {
        glfwPollEvents();
        // TODO: ImGui_ImplGlfwGL3_NewFrame

        mInstance->update();

        mInstance->gui();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        mInstance->draw();

        int framebuffer_width;
        int framebuffer_height;
        glfwGetFramebufferSize(mInstance->mWindow, &framebuffer_width,
                               &framebuffer_height);
        mInstance->mFramebufferWidth = static_cast<u32>(framebuffer_width);
        mInstance->mFramebufferHeight = static_cast<u32>(framebuffer_height);

        // TODO: render ImGui

        glfwSwapBuffers(mInstance->mWindow);
    }

    mInstance->cleanup();

    glfwDestroyWindow(mInstance->mWindow);
}
