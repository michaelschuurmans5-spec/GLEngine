#pragma once 

#include "core/Window.h"
#include "core/LayerStack.h"
#include <memory>


class Application {

public:
	// CONSTRUCTION
	Application();
	~Application();

	// STARTS GAME LOOP
	void Run();
	// SHUTDOWN
	void Close();
	// RUNTIME CAPACITY TO ACCEPT NEW WORKSPACE MODULES
	void PushLayer(Layer* layer);

	// GETTERS
	static Application& Get() { return *s_instance; }
	Window& GetWindow() { return *m_window; }

private:
	// CORE GAME LOOP PHASES
	void ProcessInput();
	void Update();
	void Render();


private:
	std::unique_ptr<Window> m_window;
	LayerStack m_LayerStack;
	bool m_isRunning = true;

	// SINGLETON INSTANCE POINTER FOR GLOBAL CALLBACK
	static Application* s_instance;
};