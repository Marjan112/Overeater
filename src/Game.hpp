#pragma once

#include <random>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include "Fish.hpp"
#include "Worm.hpp"

class Game {
public:
	Game();
	void game_loop();
private:
	void initialize();
	void restart();
	void handle_events();
	void handle_keyboard();
	void check_collision();
	void render();

	std::random_device random_device;
	std::mt19937 random_generator;
	std::uniform_real_distribution<float> from_0_to_width;
	std::uniform_real_distribution<float> from_0_to_height;

	sf::Clock delta_clock;
	float delta_time;

	Fish fish;
	Worm worm;
	
	sf::Vector2f start_pos;
	
	sf::Vector2u screen_dimension;

	sf::VideoMode video_mode;
	sf::RenderWindow window;

	sf::Font font;
	sf::Text score_text;
	unsigned int score;

	sf::Texture background_texture;
	sf::Sprite background_sprite;
};