#include <core/Logger.h>
#include <core/Engine.h>

#include "Editor.h"

#include <iostream>

int main() {
	Eternal::Editor* app = Eternal::Editor::create();
	app->run();
	delete app;

	return 0;
}