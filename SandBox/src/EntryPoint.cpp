#include "Viewer.h"

int main() {
	Eternal::Viewer* app = Eternal::Viewer::create();
	app->run();
	delete app;

	return 0;
}