#include "window.h"
#include "controller.h"
#include "logger.h"

//#include <glad/glad.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <functional>
#include <iostream>

Window *Window::currentWindow = nullptr;
int Window::primaryScreenWidth, Window::primaryScreenHeight;

class WindowManager {
    WindowManager() {
        // glfw: initialize and configure
        // ------------------------------
        glfwInit();

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

        const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
        Window::primaryScreenWidth = mode->width;
        Window::primaryScreenHeight = mode->height;
        DLOG("GLFW initialized");
    }
    ~WindowManager() {
        glfwTerminate();
        DLOG("GLFW terminated");
    }

  public:
    static WindowManager *getInstance() {
        static WindowManager winManager;
        return &winManager;
    }
};

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    auto flipScreenKey = glfwGetKey(window, GLFW_KEY_F);
    static bool sIsUp = true;
    if (flipScreenKey == GLFW_PRESS && sIsUp) {
        Window::currentWindow->toggleFullscreen();
        sIsUp = false;
    }
    if (flipScreenKey == GLFW_RELEASE) {
        sIsUp = true;
    }
    Controller::get()->processInput(window, key, scancode, action, mode);
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    Window::currentWindow->resize(width, height);
    glViewport(0, 0, width, height);
    Window::currentWindow->getAspectRatio();
}

void Window::resize(int width, int height) {

    if (m_fullScreen) {
        //primaryScreenWidth = width;
        //primaryScreenHeight = height;
    } else {
        m_WindowedWidth = width;
        m_WindowedHeight = height;
    }
}

void Window::toggleFullscreen() {
    if (m_fullScreen) {
        glfwSetWindowMonitor(m_window, nullptr, m_xPos, m_yPos, m_WindowedWidth, m_WindowedHeight, 1);
        glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        m_fullScreen = false;
        DLOGN(m_WindowedWidth, m_WindowedHeight);
    } else {
        glfwGetWindowPos(m_window, &m_xPos, &m_yPos);
        glfwSetWindowMonitor(m_window, glfwGetPrimaryMonitor(), 0, 0, primaryScreenWidth, primaryScreenHeight, 1);
        glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
        m_fullScreen = true;
        DLOGN(primaryScreenWidth, primaryScreenHeight);
    }
}

Window::Window(int width, int height, const char *_name, bool _fullScreen) : m_WindowedWidth(width), m_WindowedHeight(height), m_fullScreen(_fullScreen) {

    WindowManager::getInstance();
    glfwWindowHint(GLFW_SAMPLES, 16);

    m_WindowedWidth = width;
    m_WindowedHeight = height;
    m_xPos = (primaryScreenWidth - width) / 2;
    m_yPos = (primaryScreenHeight - height) / 2;

    if (_fullScreen) {
        m_window = glfwCreateWindow(primaryScreenWidth, primaryScreenHeight, _name, glfwGetPrimaryMonitor(), nullptr);
        glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    } else {
        m_window = glfwCreateWindow(m_WindowedWidth, m_WindowedHeight, _name, nullptr, nullptr);
        glfwSetWindowPos(m_window, m_xPos, m_yPos);
        glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
    if (m_window == nullptr) {
        glfwTerminate();
        THROW("Failed to create GLFW window");
    }
    glfwMakeContextCurrent(m_window);

    glfwSetInputMode(m_window, GLFW_STICKY_KEYS, GL_TRUE);
    glfwSetFramebufferSizeCallback(m_window, framebuffer_size_callback);

    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK) {
        glfwTerminate();
        THROW("Failed to initialize GLEW");
    }
    glfwSwapInterval(1);
    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    DLOG("Window created");
}

void Window::refresh() {
    currentWindow = this;
    glfwMakeContextCurrent(m_window);
    glfwSwapBuffers(m_window);
    glfwPollEvents();
    //processInput(m_window);
    glfwSetKeyCallback(m_window, key_callback);
    glClearColor(0.10f, 0.1f, 0.13f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Window::setTitle(const char *title) {
    glfwSetWindowTitle(m_window, title);
}

float Window::getAspectRatio() {
    if (m_fullScreen)
        return static_cast<float>(primaryScreenWidth) / static_cast<float>(primaryScreenHeight);
    else
        return static_cast<float>(m_WindowedWidth) / static_cast<float>(m_WindowedHeight);
}

bool Window::active() {
    return !glfwWindowShouldClose(m_window);
}

Window::~Window() {
    glfwDestroyWindow(m_window);
    DLOG("Window destroyed");
}
