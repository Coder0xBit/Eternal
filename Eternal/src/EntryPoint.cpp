#include <eternal/core/Logger.h>
#include <eternal/core/Engine.h>

#include <iostream>
#include <editor/Application.h>

int main() {

	Eternal::Application* app = Eternal::Application::get();
	app->run();
	delete app;

	return 0;
}