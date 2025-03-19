#include <eternal/core/Logger.h>
#include <eternal/core/Engine.h>

#include <editor/Editor.h>

#include <iostream>

int main() {
	Eternal::Editor* app = Eternal::Editor::create();
	app->run();
	delete app;

	return 0;
}