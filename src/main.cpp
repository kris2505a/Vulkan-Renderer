#include "App.hpp"
#include <iostream>
#include <exception>

int main() {
	auto app = new App();
	
	try {
		app->run();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
