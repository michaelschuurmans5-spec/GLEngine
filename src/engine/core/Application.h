#pragma once 

#include "core/Window.h"
#include "core/LayerStack.h"

#include <memory>


enum class AppMode { Game, Editor };

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

	// New state management properties
	inline AppMode GetMode() const { return m_currentMode; }
	inline void SetMode(AppMode mode) { m_currentMode = mode; }


private:
	// CORE GAME LOOP PHASES
	void ProcessInput();
	void Update(float deltaTime);
	void Render();

	AppMode m_currentMode = AppMode::Game;

private:
	// POINTERS & SMART POINTERS
	std::unique_ptr<Window> m_window;
	LayerStack m_LayerStack;
	bool m_isRunning = true;


	// SINGLETON INSTANCE POINTER FOR GLOBAL CALLBACK
	static Application* s_instance;
};