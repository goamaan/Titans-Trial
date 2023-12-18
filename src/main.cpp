
#define GL3W_IMPLEMENTATION
#include <gl3w.h>

// stlib
#include <chrono>

// internal
#include "physics_system.hpp"
#include "render_system.hpp"
#include "world_system.hpp"

using Clock = std::chrono::high_resolution_clock;
// Entry point
int main()
{
	// Global systems
	WorldSystem world_system;
	RenderSystem render_system;
	PhysicsSystem physics_system;
	
	// Initializing window
	GLFWwindow* window = world_system.create_window();
	if (!window) {
		// Time to read the error message
		printf("Press any key to exit");
		getchar();
		return EXIT_FAILURE;
	}

	// initialize the main systems
	render_system.init(window);
	world_system.init(&render_system);
	
	// variable timestep loop
	auto t = Clock::now();
	while (!world_system.is_over()) {
		// Processes system messages, if this wasn't present the window would become unresponsive
		glfwPollEvents();
		// Calculating elapsed times in milliseconds from the previous iteration
        auto now = Clock::now();
		
        if (!world_system.pause && !world_system.isTitleScreen) {
            float elapsed_ms =
                    min((float)(std::chrono::duration_cast<std::chrono::microseconds>(now - t)).count() / 1000, 50.0f);
            world_system.step(elapsed_ms);
			physics_system.step(elapsed_ms, world_system.dialogue_screen_active);
            world_system.handle_collisions();
        }
        t = now;

		render_system.draw(world_system.pause, world_system.debug, world_system.dialogue_screen_active);
	}

	return EXIT_SUCCESS;
}
