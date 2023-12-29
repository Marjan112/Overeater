#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

struct GameEntityFish {
	float mov_speed;
	sf::Vector2f velocity;
	sf::Image image;
	sf::Vector2u dimension;
	sf::Vector2f fish_pos;
	sf::Texture* texture;
	sf::SoundBuffer* sound_buffer;
	sf::Sound* eating_sound;
	sf::RectangleShape* shape;
};

struct GameEntityWorm {
	sf::Image image;
	sf::Vector2u dimension;
	sf::Vector2f worm_pos;
	sf::Texture* texture;
	sf::RectangleShape* shape;
};

struct GameEntities {
	GameEntityFish entity_fish;
	GameEntityWorm entity_worm;
};