#if defined(_MSC_VER)
#	define NOMINMAX
#	pragma comment(lib, "sfml-window.lib")
#	pragma comment(lib, "sfml-audio.lib")
#	pragma comment(lib, "sfml-system.lib")
#	pragma comment(lib, "sfml-graphics.lib")
#endif

#include "Game.hpp"

int main(int argc, char** argv) {
	Game(argc, argv).game_loop();
	return 0;
}