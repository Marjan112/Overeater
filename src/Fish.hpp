#pragma once

#include "Entity.hpp"

struct Fish : Entity {
	float mov_speed;
	sf::Vector2f velocity;
	sf::SoundBuffer sound_buffer;
	sf::Sound sound;
};