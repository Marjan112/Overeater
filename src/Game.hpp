#include <iostream>
#include <random>

#if defined(__linux__)
#	include "../include/SFML_Linux/Graphics.hpp"
#	include "../include/SFML_Linux/Audio.hpp"
#elif defined(_MSC_VER)
#	include "../include/SFML_MSC_Windows/Graphics.hpp"
#	include "../include/SFML_MSC_Windows/Audio.hpp"
#endif

#include "resources.hpp"
#include "GameEntities.hpp"

class Game {
public:
	Game();
	void game_loop();
	~Game();
private:
	bool initialize();
	void restart();
	void handle_events();
	void handle_keyword();
	void check_collision();
	void render();
	void destroy();

	std::random_device random_device;
	std::mt19937 random_generator;
	std::uniform_real_distribution<float> from_0_to_width;
	std::uniform_real_distribution<float> from_0_to_height;

	sf::Clock delta_clock;
	float delta_time;

	GameEntities entities;
	sf::Vector2f start_pos;
	sf::Vector2f worm_pos;
	sf::FloatRect next_pos;
	
	sf::Image image_background;
	sf::Vector2u screen_dimension;

	sf::VideoMode video_mode;
	sf::RenderWindow* window;

	sf::Font* font;
	sf::Text* score_text;
	int score;

	sf::Texture* background_texture;
	sf::Sprite* background_sprite;

	bool is_initialized;
};