#include <eternal/core/Logger.h>
#include <eternal/core/Engine.h>

#include <iostream>

int main() {

	Eternal::Logger::Init();
	Eternal::Engine* mEngine = Eternal::Engine::Builder()
		.applicationName("Eternal Application")
		.build();

	mEngine->run();

	delete mEngine;

	return 0;
}