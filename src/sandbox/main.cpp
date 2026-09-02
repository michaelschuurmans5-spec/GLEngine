#include "core/Application.h"
#include "GameLayer.h"

#include <iostream>

// APPLICATION
int main() {
	// CREATE APPLICATION SANDBOX ENVIRONMENT
	Application app;
	// INSTANTITE YOUR GAMEPLAYER AND PASS IT TO THE ENGINE RUNTIME
	app.PushLayer(new GameLayer());

	// HAND CONTROL TO MAIN SIM LOOP
	app.Run();
	
	return 0;
}