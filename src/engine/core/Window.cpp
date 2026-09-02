#include "core/Window.h"
#include <iostream>

// CONSTRUCTOR
Window::Window(int width, int height, const char* title)
    : m_isFullScreen(false) {
	m_windowedWidth = width;   
	m_windowedHeight = height;
    // WINDOW INIT FAIL CHECK
    if (!init(width, height, title)) {
        std::cerr << "Critical Error: Window initialization failed!" << std::endl;
    }
	// WINDOW MODE STATE CHECK
    if (m_window) {
        glfwGetWindowPos(m_window, &m_windowedX, &m_windowedY);
    }
}
// DESTRUCTOR
Window::~Window() {
    if (m_window) {
        glfwDestroyWindow(m_window);
        m_window = nullptr;
    }
}
// INIT WINDOW DIMENSIONS AND TITLE
bool Window::init(int width, int height, const char* title) {
	// GLFW VERSION AND PROFILE SETUP
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // CREATES WINDOW ONCE
    m_window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!m_window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        return false;
    }
	// SETS CONTEXT FOR OPENGL
    glfwMakeContextCurrent(m_window);
    // INIT GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return false;
    }
	// VIEWPORT SETUP & CALLBACK FOR RESIZING
    glViewport(0, 0, width, height);
    glfwSetFramebufferSizeCallback(m_window, framebufferSizeCallback);

    return true;
}
// WINDOW CLOSE CHECK
bool Window::shouldClose() const {
    return glfwWindowShouldClose(m_window);
}
// SWAP BUFFERS
void Window::swapBuffers() const {
    glfwSwapBuffers(m_window); 
}
// POLL EVENTS
void Window::pollEvents() const {
    glfwPollEvents(); 
}
// CLEAR COLOR BUFFER
void Window::clear(float r, float g, float b, float a) const {
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
// RESIZE WINDOW
void Window::framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height); 
}
// KEY CHECKS
void Window::processInput() {
    if (!m_window) return;
    // ESCAPE KEY CLOSE WINDOW
    if (glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(m_window, true);
    }
}
// TOGGLE FULLSCREEN
void Window::toggleFullScreen() {
    if (!m_window) return;

    m_isFullScreen = !m_isFullScreen;

    if (m_isFullScreen) {
        // SAVE CURRENT WINDOW PROPERTIES BEFORE CHANGE
        glfwGetWindowPos(m_window, &m_windowedX, &m_windowedY);
        glfwSetWindowPos(m_window, m_windowedWidth, m_windowedHeight);

        // FETCH PRIMARY MONITOR DIMENSIONS
        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);

        // TURN FULLSCREEN MODE ON
        glfwSetWindowMonitor(m_window, monitor, 0, 0, mode->width, mode->height,
			mode->refreshRate);
    }else {
        // TURN FULLSCREEN MODE OFF
        glfwSetWindowMonitor(m_window, nullptr, m_windowedX, m_windowedY, 
        m_windowedWidth, m_windowedHeight, 0);
    }
}

