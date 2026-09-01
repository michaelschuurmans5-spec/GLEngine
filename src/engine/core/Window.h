#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>


class Window {
public:
	// WINDOW CLASS
	Window(int width, int height, const char* title);
	~Window();
	// FUNCTIONS
	bool shouldClose() const;
	void swapBuffers() const;
	void pollEvents() const;
	void clear(float r, float g, float b, float a) const;
	void toggleFullScreen();
	void processInput();

	//GETTERS
	GLFWwindow* getNativeWindow() const { return m_window; }

private:
	// WINDOW PROPERTIES
	GLFWwindow* m_window;
	int m_width;
	int m_height;
	const char* m_title;
	// BUTTONS 
	bool m_isFullScreen;
	// WINDOW POSITION AND SIZE
	int m_windowedX; 
	int m_windowedY;
	int	m_windowedWidth;
	int m_windowedHeight;


	// INITIALIZATION
	bool init(int width, int height, const char* title);
	// RESIZE WINDOW 
	static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
};