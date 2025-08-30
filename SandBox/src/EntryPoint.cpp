#include "Editor.h"

int main() {
	Eternal::Editor* app = Eternal::Editor::create();
	app->run();
	delete app;

	return 0;
}