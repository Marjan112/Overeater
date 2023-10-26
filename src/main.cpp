#include <iostream>
#include <fstream>

#include "termcolor.hpp"
#include "../include/SFML/Graphics.hpp"
#include "../include/SFML/Audio.hpp"

#ifdef _WIN32
#	include <winsock.h>
#elif __linux__
#	include <arpa/inet.h>
#endif

namespace logging {
	template<typename StreamT, typename... ArgsT>
	void print(StreamT& stream, ArgsT&&... args) {
		(stream << ... << args);
		(stream << "\n");
	}

	template<typename... ArgsT>
	void print_info(ArgsT&&... args) {
		print(std::cout, termcolor::green<char>, "[INFO]: ", termcolor::reset<char>, args...);
	}

	template<typename... ArgsT>
	void print_error(ArgsT&&... args) {
		print(std::cerr, termcolor::red<char>, "[ERROR]: ", termcolor::reset<char>, args...);
	}
}

class Game {
private:
	const std::string resource_background = "resources/img/bg.png";
	const std::string resource_font = "resources/fonts/Roboto-Bold.ttf";
	const std::string resource_fish_up = "resources/img/fish_up.png";
	const std::string resource_worm = "resources/img/worm.png";
	const std::string resource_pou_eating = "resources/sound/pou_eating.wav";

	sf::Clock delta_clock;
	sf::Vector2f velocity;
	float delta_time;
	float mov_speed;

	int score;
	
	sf::Vector2i* screen_dimension;
	sf::Vector2f* fish_dimension;
	sf::Vector2f* worm_dimension;

	sf::VideoMode video_mode;

	sf::RenderWindow* window;
	sf::RectangleShape* fish;
	sf::RectangleShape* worm;

	sf::Vector2f start_pos;

	sf::Font* font;
	sf::Text* score_text;

	sf::Texture* background_texture;
	sf::Texture* fish_texture;
	sf::Texture* worm_texture;

	sf::Sprite* background_sprite;

	sf::SoundBuffer* sound_buffer;
	sf::Sound* beep;

	sf::Vector2f worm_pos;

	sf::FloatRect next_pos;
	sf::FloatRect fish_bounds;
	sf::FloatRect worm_bounds;

	sf::Vector2i get_dimension_of_image(const std::string& filepath) {
		std::ifstream image(filepath, std::ios::binary);

		if(!image.good()) {
			logging::print_error("Could not open file \"", filepath, "\".");
			exit(1);
		}

		sf::Vector2i dimension;

		image.seekg(16);
		image.read((char*)&dimension.x, 4);
		image.read((char*)&dimension.y, 4);

		dimension.x = ntohl(dimension.x);
		dimension.y = ntohl(dimension.y);

		return dimension;
	}

	void initialize() {
		screen_dimension = new sf::Vector2i(get_dimension_of_image(resource_background));
		fish_dimension = new sf::Vector2f(static_cast<sf::Vector2f>(get_dimension_of_image(resource_fish_up)));
		worm_dimension = new sf::Vector2f(static_cast<sf::Vector2f>(get_dimension_of_image(resource_worm)));
	
		logging::print_info("screen_dimension = (", screen_dimension->x, ", ", screen_dimension->y, ")");

		video_mode = sf::VideoMode(screen_dimension->x, screen_dimension->y);
		window = new sf::RenderWindow(video_mode, "Overeater", sf::Style::Close);
		fish = new sf::RectangleShape(*fish_dimension);
		worm = new sf::RectangleShape(*worm_dimension);
	
		start_pos = sf::Vector2f(screen_dimension->x / 2 - fish_dimension->x / 2, screen_dimension->y / 2 - fish_dimension->y / 2);
	
		font = new sf::Font();
		if(font->loadFromFile(resource_font)) {
			logging::print_info("Resource \"", resource_font, "\" is loaded successfully.");
		}

		score = 0;

		score_text = new sf::Text();
		score_text->setFont(*font);
		score_text->setCharacterSize(50);
		score_text->setFillColor(sf::Color::Cyan);
		score_text->setStyle(sf::Text::Bold);
		score_text->setString("Score: " + std::to_string(score));
		score_text->setPosition(screen_dimension->x / 2 - score_text->getGlobalBounds().width / 2, screen_dimension->y / 2 - score_text->getGlobalBounds().height / 2);

		background_texture = new sf::Texture();
		fish_texture = new sf::Texture();
		worm_texture = new sf::Texture();

		if(background_texture->loadFromFile(resource_background)) {
			logging::print_info("Resource \"", resource_background, "\" is loaded successfully.");
		}
		
		if(fish_texture->loadFromFile(resource_fish_up)) {
			logging::print_info("Resource \"", resource_fish_up, "\" is loaded successfully.");
		}

		if(worm_texture->loadFromFile(resource_worm)) {
			logging::print_info("Resource \"", resource_worm, "\" is loaded successfully.");
		}
	
		background_sprite = new sf::Sprite(*background_texture);
	
		fish->setTexture(*&fish_texture);
		fish->setPosition(start_pos);

		sound_buffer = new sf::SoundBuffer();

		if(sound_buffer->loadFromFile(resource_pou_eating)) {
			logging::print_info("Resource \"", resource_pou_eating, "\" is loaded successfully.");
		}
	
		beep = new sf::Sound();

		beep->setBuffer(*sound_buffer);
		beep->setVolume(50);

		srand(time(nullptr));

		worm_pos = sf::Vector2f(static_cast<float>(rand() % (screen_dimension->x - 10)), static_cast<float>(rand() % (screen_dimension->y - 10)));
	
		worm->setTexture(*&worm_texture);
		worm->setPosition(worm_pos.x, worm_pos.y);

		mov_speed = 700.f;
	
		window->setFramerateLimit(60);
	}

	void restart() {
		logging::print(std::cout, "========================================");
		logging::print_info("Restarting the game...");
		logging::print(std::cout, "========================================");

		delete screen_dimension;
		delete fish_dimension;
		delete worm_dimension;
		delete window;
		delete fish;
		delete worm;
		delete font;
		delete score_text;
		delete background_texture;
		delete fish_texture;
		delete worm_texture;
		delete background_sprite;
		delete sound_buffer;
		delete beep;

		initialize();
	}

public:
	Game() {
		initialize();
	}

	void game_loop() {
		sf::Event e;
		while(window->isOpen()) {
			delta_time = delta_clock.restart().asSeconds();

			while(window->pollEvent(e)) {
				switch(e.type) {
					case sf::Event::Closed: {
						window->close();
						break;
					}
				}
			}

			velocity.x = velocity.y = 0;

			if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) {
				fish->setRotation(-90.f);
				velocity.x += -mov_speed * delta_time;
			} else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
				fish->setRotation(90.f);
				velocity.x += mov_speed * delta_time;
			} else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) {
				fish->setRotation(0.f);
				velocity.y += -mov_speed * delta_time;
			} else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) {
				fish->setRotation(-180.f);
				velocity.y += mov_speed * delta_time;
			} else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::R)) {
				restart();
			}

			fish->move(velocity);

			fish_bounds = fish->getGlobalBounds();
			worm_bounds = worm->getGlobalBounds();
			next_pos = fish_bounds;
			next_pos.left += velocity.x;
			next_pos.top += velocity.y;
			if(worm_bounds.intersects(next_pos)) {
				beep->play();
				worm_pos.x = static_cast<float>(rand() % (screen_dimension->x - 10));
				worm_pos.y = static_cast<float>(rand() % (screen_dimension->y - 10));
				score_text->setString("Score: " + std::to_string(++score));
				worm->setPosition(worm_pos);
				worm->setRotation(1.0f);
			}

			window->clear();
			window->draw(*background_sprite);
			window->draw(*score_text);
			window->draw(*fish);
			window->draw(*worm);
			window->display();
		}
	}

	~Game() {
		delete screen_dimension;
		delete fish_dimension;
		delete worm_dimension;
		delete window;
		delete fish;
		delete worm;
		delete font;
		delete score_text;
		delete background_texture;
		delete fish_texture;
		delete worm_texture;
		delete background_sprite;
		delete sound_buffer;
		delete beep;
	}
};

int main() {
	Game().game_loop();
	return 0;
}