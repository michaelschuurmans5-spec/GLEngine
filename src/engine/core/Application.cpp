#include "core/Application.h"
#include "core/Log.h"
#include "core/Window.h"
#include "GLFW/glfw3.h"


#include <iostream>

// INIT STATIC INSTANCE POINTER
Application* Application::s_instance = nullptr;

// GLFW GLOBAL CALLBACK BRIDGE
void keyCallbackBridge(GLFWwindow* glfwWin, int key, int scancode, int action, int mods)
{
	// F1 TOGGLE FULLSCREEN
	if (key == GLFW_KEY_F1 && action == GLFW_PRESS) {
		Application::Get().GetWindow().toggleFullScreen();
	}
}
// CONSTRUCTOR
Application::Application() {

	s_instance = this;

	// INIT LOG SYSTEM 
	Log::Init();
	ENGINE_INFO("Engine Initializing");
	std::cout << "Engine Initializing" << std::endl;
	// INIT GLFW
	if (!glfwInit()) {
		ENGINE_ERROR("Failed to Initialize GLFW globally!");
		std::cerr << "Critical Error: GLFW initialization failed!" << std::endl;
		m_isRunning = false;
		return;
	}
	// CREATE WINDOW
	m_window = std::make_unique<Window>(800, 600, "GLEngine");

	// ROUTE GLFW CALLBACKS TO SYSTEM ARCHITECTURE
	glfwSetKeyCallback(m_window->getNativeWindow(), keyCallbackBridge);
}
//DESTRUCTOR
Application::~Application() {

	std::cout << "Engine Shutting Down..." << std::endl;
	// SHUTDOWN GLFW
	glfwTerminate();
}
// APPLICATION RUN LOOP
void Application::Run() {

	std::cout << "Engine Running..." << std::endl;

	// MAIN GAME LOOP
	while (m_isRunning && !m_window->shouldClose()) {
		ProcessInput();
		Update();
		Render();
		// SWAP BUFFERS & UPDATE WINDOW EVENTS
		m_window->swapBuffers();
		m_window->pollEvents();
	}
}

void Application::ProcessInput()
{
	// CONTINUOUS INPUT CHECKS
	m_window->processInput();
}

void Application::Update()
{
	// UPDATE GAME LOGIC
	for (Layer* layer : m_LayerStack) {
		layer->OnUpdate();
	}
}

void Application::Render()
{
	// RENDER & CLEAR SCREEN
	m_window->clear(0.2f, 0.3f, 0.3f, 1.0f);
	// DRAW CALLS
	for (Layer* layer : m_LayerStack) {
		layer->OnRender();
	}
}

void Application::Close()
{
	m_isRunning = false;
}

void Application::PushLayer(Layer* layer) {
	// PASS LAYER POINTER DIRECTLY TO THE LAYERSTACK CONTAINER 
	m_LayerStack.PushLayer(layer);

	// LOG THE NEW WORKSPACE MODULE WAS ADDED SUCCESSFULLY
	ENGINE_INFO("Pushed Layer onto stack: " + layer->GetName());
}

