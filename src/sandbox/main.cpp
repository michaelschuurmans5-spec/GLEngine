#include "core/Application.h"

#include <iostream>

// APPLICATION
int main() {
	// CREATE APPLICATION SANDBOX ENVIRONMENT
	Application app;
	// HAND CONTROL TO MAIN SIM LOOP
	app.Run();
	
	return 0;
}