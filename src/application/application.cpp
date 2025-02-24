#include "application.hpp"

#include "format.hpp"

std::shared_ptr<Application> Application::mInstance = nullptr;

Application::Application()
    : mWindow(nullptr),
      mWindowTitle(),
      mWindowWidth(0),
      mWindowHeight(0),
      mFramebufferWidth(0),
      mFramebufferHeight(0),
      mMonitor(nullptr) {
}

Application::~Application() {
    glfwTerminate();
}

void Application::init() {
}

void Application::update() {
}

void Application::gui() {
}

void Application::draw() {
}

void Application::cleanup() {
}

void Application::onCursorEnterWindow(int entered) {
}

void Application::onMouseMove(const Vector2D pos) {
}

void Application::onMouseButtonPress(int button, int actions, int mods) {
}

void Application::onMouseScroll(const Vector2D offset) {
}

void Application::onKeyPress(int key, int action, int mods) {
}

void Application::onWindowResize(const u32 width, const u32 height) {
}

void Application::onFramebufferResize(const u32 width, const u32 height) {
}

int Application::getKeyRaw(int key) {
    return glfwGetKey(mWindow, key);
}

f32 Application::windowAspect() const {
    return static_cast<f32>(mWindowWidth) / static_cast<f32>(mWindowHeight);
}

f32 Application::framebufferAspect() const {
    return static_cast<f32>(mFramebufferWidth) /
           static_cast<f32>(mFramebufferHeight);
}

void Application::quit() const {
    glfwSetWindowShouldClose(mWindow, true);
}

std::shared_ptr<Application> Application::getInstance() {
    if (mInstance == nullptr)
        mInstance = std::shared_ptr<Application>(new Application());
    return mInstance;
}

void Application::registerGlfwCallbacks() {
    glfwSetCursorEnterCallback(mWindow, cursorEnterWindowCallBack);
    glfwSetCursorPosCallback(mWindow, mouseMoveCallBack);
    glfwSetMouseButtonCallback(mWindow, mouseButtonPressCallBack);
    glfwSetScrollCallback(mWindow, mouseScrollCallBack);
    glfwSetKeyCallback(mWindow, keyPressCallBack);
    glfwSetWindowSizeCallback(mWindow, windowResizeCallBack);
    glfwSetFramebufferSizeCallback(mWindow, framebufferResizeCallback);
}

void Application::errorCallback(int error, const char* description) {
    eprintln("GLFW error code: {}\nGLFW error description: {}", error,
             description);
}

void Application::cursorEnterWindowCallBack(GLFWwindow* window, int entered) {
    getInstance()->onCursorEnterWindow(entered);
}

void Application::mouseMoveCallBack(GLFWwindow* window, double xPos,
                                    double yPos) {
    getInstance()->onMouseMove(
        Vector2D(static_cast<f32>(xPos), static_cast<f32>(yPos)));
}

void Application::mouseButtonPressCallBack(GLFWwindow* window, int button,
                                           int actions, int mods) {
    getInstance()->onMouseButtonPress(button, actions, mods);
}

void Application::mouseScrollCallBack(GLFWwindow* window, double xOffSet,
                                      double yOffSet) {
    getInstance()->onMouseScroll(
        Vector2D(static_cast<f32>(xOffSet), static_cast<f32>(yOffSet)));
}

void Application::keyPressCallBack(GLFWwindow* window, int key, int scancode,
                                   int action, int mods) {
    getInstance()->onKeyPress(key, action, mods);
}

void Application::windowResizeCallBack(GLFWwindow* window, int width,
                                       int height) {
    getInstance()->onWindowResize(static_cast<u32>(width),
                                  static_cast<u32>(height));
}

void Application::framebufferResizeCallback(GLFWwindow* window, int width,
                                            int height) {
    getInstance()->onFramebufferResize(static_cast<u32>(width),
                                       static_cast<u32>(height));
}
