#pragma once

#include <iostream>
#include <random>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include "resources.hpp"
#include "msgbox.hpp"
#include "GameEntities.hpp"

#define DEFAULT_FISH_MOV_SPEED 700.f

class Game {
public:
	Game(int argc, char** agrv);
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
	
	sf::Image image_background;
	sf::Vector2u screen_dimension;

	sf::VideoMode video_mode;
	sf::RenderWindow* window;

	sf::Font* font;
	sf::Text* score_text;
	unsigned int score;

	sf::Texture* background_texture;
	sf::Sprite* background_sprite;

	bool is_initialized;
};