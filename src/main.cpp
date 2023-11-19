#include <iostream>
#include <fstream>
#include <random>
#include <cstring>

#if defined(__linux__)
#	include "../include/SFML_Linux/Graphics.hpp"
#	include "../include/SFML_Linux/Audio.hpp"
#elif defined(_MSC_VER)
#	define NOMINMAX

#	pragma comment(lib, "sfml-window.lib")
#	pragma comment(lib, "sfml-audio.lib")
#	pragma comment(lib, "sfml-system.lib")
#	pragma comment(lib, "sfml-graphics.lib")

#	include "../include/SFML_MSC_Windows/Graphics.hpp"
#	include "../include/SFML_MSC_Windows/Audio.hpp"
#endif

class Game {
public:
	Game(bool& is_initialized) {
		#if !defined(DEBUG)
		sf::err().rdbuf(nullptr);
		#endif
		random_generator.seed(random_device());
		this->is_initialized = initialize();
		is_initialized = this->is_initialized;
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
		if(is_initialized == true) {
			destroy();
		}
	}
private:
	bool initialize() {
		if(image_background.loadFromFile(resource_background)) {
			std::cout << "Successfully loaded dimension of image \"" << resource_background << "\".\n";
		} else {
			std::cerr << "Could not load dimension of image \"" << resource_background << "\". Error: " << strerror(errno) << "\n";
			return false;
		}

		if(image_fish_up.loadFromFile(resource_fish_up)) {
			std::cout << "Successfully loaded image dimension of \"" << resource_fish_up << "\".\n";
		} else {
			std::cerr << "Could not load dimension of image \"" << resource_fish_up << "\". Error: " << strerror(errno) << "\n";
			return false;
		}

		if(image_worm.loadFromFile(resource_worm)) {
			std::cout << "Successfully loaded image dimension of \"" << resource_worm << "\".\n";
		} else {
			std::cerr << "Could not load dimension of image \"" << resource_worm << "\". Error: " << strerror(errno) << "\n";
			return false;
		}

		screen_dimension = image_background.getSize();
		fish_dimension = image_fish_up.getSize();
		worm_dimension = image_worm.getSize();

		video_mode = sf::VideoMode(screen_dimension.x, screen_dimension.y);
		window = new sf::RenderWindow(video_mode, "Overeater", sf::Style::Close);
		fish = new sf::RectangleShape({(float)fish_dimension.x, (float)fish_dimension.y});
		worm = new sf::RectangleShape({(float)worm_dimension.x, (float)worm_dimension.y});
	
		start_pos = {
			(float)screen_dimension.x / 2 - (float)fish_dimension.x / 2,
			(float)screen_dimension.y / 2 - (float)fish_dimension.y / 2
		};
	
		font = new sf::Font();
		if(font->loadFromFile(resource_font)) {
			std::cout << "Resource \"" << resource_font << "\" is loaded successfully.\n";
		} else {
			std::cerr << "Could not load resource \"" << resource_font << "\". Error: " << strerror(errno) << "\n";
			return false;
		}

		score = 0;

		score_text = new sf::Text();
		score_text->setFont(*font);
		score_text->setCharacterSize(50);
		score_text->setFillColor(sf::Color::Cyan);
		score_text->setStyle(sf::Text::Bold);
		score_text->setString("Score: " + std::to_string(score));
		score_text->setPosition(
			screen_dimension.x / 2 - score_text->getGlobalBounds().width / 2,
			screen_dimension.y / 2 - score_text->getGlobalBounds().height / 2
		);

		background_texture = new sf::Texture();
		fish_texture = new sf::Texture();
		worm_texture = new sf::Texture();

		if(background_texture->loadFromImage(image_background)) {
			std::cout << "Resource \"" << resource_background << "\" is loaded successfully.\n";
		} else {
			std::cerr << "Could not load resource \"" << resource_background << "\". Error: " << strerror(errno) << "\n";
			return false;
		}
		
		if(fish_texture->loadFromImage(image_fish_up)) {
			std::cout << "Resource \"" << resource_fish_up << "\" is loaded successfully.\n";
		} else {
			std::cerr << "Could not load resource \"" << resource_fish_up << "\". Error: " << strerror(errno) << "\n";
			return false;
		}

		if(worm_texture->loadFromImage(image_worm)) {
			std::cout << "Resource \"" << resource_worm << "\" is loaded successfully.\n";
		} else {
			std::cerr << "Could not load resource \"" << resource_worm << "\". Error: " << strerror(errno) << "\n";
			return false;
		}
	
		background_sprite = new sf::Sprite(*background_texture);
	
		fish->setTexture(*&fish_texture);
		fish->setOrigin(fish->getSize().x / 2, fish->getSize().x / 2);
		fish->setPosition(start_pos);

		sound_buffer = new sf::SoundBuffer();

		if(sound_buffer->loadFromFile(resource_pou_eating)) {
			std::cout << "Resource \"" << resource_pou_eating << "\" is loaded successfully.\n";
		} else {
			std::cerr << "Could not load resource \"" << resource_pou_eating << "\". Error: " << strerror(errno) << "\n";
			return false;
		}
	
		sound_when_fish_eats = new sf::Sound();

		sound_when_fish_eats->setBuffer(*sound_buffer);
		sound_when_fish_eats->setVolume(50);

		from_0_to_width = std::uniform_real_distribution<float>(0.f, screen_dimension.x);
		from_0_to_height = std::uniform_real_distribution<float>(0.f, screen_dimension.y);

		worm_pos = {
			from_0_to_width(random_generator),
			from_0_to_height(random_generator)
		};
	
		worm->setTexture(*&worm_texture);
		worm->setPosition(worm_pos);
	
		window->setFramerateLimit(60);

		mov_speed = 700.f;
		
		return true;
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
			sound_when_fish_eats->play();
			worm_pos = {
				from_0_to_width(random_generator),
				from_0_to_height(random_generator),
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
		delete sound_when_fish_eats;
	}

	const std::string resource_background = "resources/img/bg.png";
	const std::string resource_font = "resources/fonts/Roboto-Bold.ttf";
	const std::string resource_fish_up = "resources/img/fish_up.png";
	const std::string resource_worm = "resources/img/worm.png";
	const std::string resource_pou_eating = "resources/sound/pou_eating.wav";

	std::random_device random_device;
	std::mt19937 random_generator;
	std::uniform_real_distribution<float> from_0_to_width;
	std::uniform_real_distribution<float> from_0_to_height;

	sf::Clock delta_clock;
	sf::Vector2f velocity;
	float delta_time;
	float mov_speed;

	int score;
	
	sf::Image image_background;
	sf::Image image_fish_up;
	sf::Image image_worm;

	sf::Vector2u screen_dimension;
	sf::Vector2u fish_dimension;
	sf::Vector2u worm_dimension;

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
	sf::Sound* sound_when_fish_eats;

	sf::Vector2f worm_pos;

	sf::FloatRect next_pos;
	sf::FloatRect fish_bounds;
	sf::FloatRect worm_bounds;

	bool is_initialized;
};

int main() {
	bool is_initialized;
	Game game(is_initialized);
	
	if(is_initialized == false) {
		std::cerr << "Could not initialize game.\n";
		return 1;
	}

	game.game_loop();
	return 0;
}