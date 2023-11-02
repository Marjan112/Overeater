#include <iostream>
#include <fstream>

#if defined(__linux__)
#include <arpa/inet.h>

#include "../include/SFML_Linux/Graphics.hpp"
#include "../include/SFML_Linux/Audio.hpp"
#elif defined(_WIN32)
#include <winsock.h>

#if defined(__MINGW32__)
#include "../include/SFML_Windows_MinGW/Graphics.hpp"
#include "../include/SFML_Windows_MinGW/Audio.hpp"
#elif defined(_MSC_VER)
#include "../include/SFML_Vsc/Graphics.hpp"
#include "../include/SFML_Vsc/Audio.hpp"
#endif
#endif

class Game {
public:
	Game() {
		initialize();
		srand(time(nullptr));
	}

	void game_loop() {
		while(window->isOpen()) {
			delta_time = delta_clock.restart().asSeconds();
			velocity = {0, 0};

			handle_events();
			handle_keyword();

			fish->move(velocity);

			check_collision();
			render();
		}
	}

	~Game() {
		destroy();
	}
private:
	// Supports png only
	sf::Vector2i get_dimension_of_image(const std::string& filepath) {
		std::ifstream image(filepath, std::ios::binary);

		if(!image.good()) {
			std::cout << "Could not open file \"" << filepath << "\".\n";
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

		video_mode = sf::VideoMode(screen_dimension->x, screen_dimension->y);
		window = new sf::RenderWindow(video_mode, "Overeater", sf::Style::Close);
		fish = new sf::RectangleShape(*fish_dimension);
		worm = new sf::RectangleShape(*worm_dimension);
	
		start_pos = sf::Vector2f(screen_dimension->x / 2 - fish_dimension->x / 2, screen_dimension->y / 2 - fish_dimension->y / 2);
	
		font = new sf::Font();
		if(font->loadFromFile(resource_font)) {
			std::cout << "Resource \"" << resource_font << "\" is loaded successfully.\n";
		}

		score = 0;

		score_text = new sf::Text();
		score_text->setFont(*font);
		score_text->setCharacterSize(50);
		score_text->setFillColor(sf::Color::Cyan);
		score_text->setStyle(sf::Text::Bold);
		score_text->setString("Score: " + std::to_string(score));
		score_text->setPosition(
			screen_dimension->x / 2 - score_text->getGlobalBounds().width / 2,
			screen_dimension->y / 2 - score_text->getGlobalBounds().height / 2
		);

		background_texture = new sf::Texture();
		fish_texture = new sf::Texture();
		worm_texture = new sf::Texture();

		if(background_texture->loadFromFile(resource_background)) {
			std::cout << "Resource \"" << resource_background << "\" is loaded successfully.\n";
		}
		
		if(fish_texture->loadFromFile(resource_fish_up)) {
			std::cout << "Resource \"" << resource_fish_up << "\" is loaded successfully.\n";
		}

		if(worm_texture->loadFromFile(resource_worm)) {
			std::cout << "Resource \"" << resource_worm << "\" is loaded successfully.\n";
		}
	
		background_sprite = new sf::Sprite(*background_texture);
	
		fish->setTexture(*&fish_texture);
		fish->setPosition(start_pos);

		sound_buffer = new sf::SoundBuffer();

		if(sound_buffer->loadFromFile(resource_pou_eating)) {
			std::cout << "Resource \"" << resource_pou_eating << "\" is loaded successfully.\n";
		}
	
		beep = new sf::Sound();

		beep->setBuffer(*sound_buffer);
		beep->setVolume(50);

		worm_pos = {
			static_cast<float>(rand() % (screen_dimension->x)),
			static_cast<float>(rand() % (screen_dimension->y))
		};
	
		worm->setTexture(*&worm_texture);
		worm->setPosition(worm_pos);

		mov_speed = 700.f;
	
		window->setFramerateLimit(60);
	}

	void restart() {
		std::cout << "========================================\n";
		std::cout << "Restarting the game...\n";
		std::cout << "========================================\n";

		destroy();
		initialize();
	}

	void handle_events() {
		sf::Event event;
		while(window->pollEvent(event)) {
			switch(event.type) {
				case sf::Event::Closed: {
					window->close();
					break;
				}
			}
		}
	}

	void handle_keyword() {
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
		} else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape)) {
			window->close();
			destroy();
			exit(0);
		}
	}

	void check_collision() {
		fish_bounds = fish->getGlobalBounds();
		worm_bounds = worm->getGlobalBounds();
		next_pos = fish_bounds;
		next_pos.left += velocity.x;
		next_pos.top += velocity.y;
		if(worm_bounds.intersects(next_pos)) {
			beep->play();
			worm_pos = {
				static_cast<float>(rand() % screen_dimension->x),
				static_cast<float>(rand() % screen_dimension->y),
			};
			score_text->setString("Score: " + std::to_string(++score));
			worm->setPosition(worm_pos);
			worm->setRotation(1.0f);
		}
	}

	void render() {
		window->clear();
		window->draw(*background_sprite);
		window->draw(*score_text);
		window->draw(*fish);
		window->draw(*worm);
		window->display();
	}

	void destroy() {
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
};

int main() {
	Game().game_loop();
	return 0;
}