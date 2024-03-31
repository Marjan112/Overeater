#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

struct Entity {
	Entity() = default;
	virtual ~Entity() = default;
    
	sf::Image image;
    sf::Vector2u dimension;
    sf::Vector2f position;
    sf::Texture* texture;
    sf::RectangleShape* shape;
};