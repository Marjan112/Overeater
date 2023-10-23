#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>
#include <thread>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <termcolor.hpp>
#include <cstring>
#include <arpa/inet.h>

using namespace std::chrono_literals;

static const sf::Vector2i SCREEN_DIMENSION(1152, 864);

sf::Clock delta_clock;
sf::Vector2f velocity;
float delta_time;
static float mov_speed = 700.f;
static int score = 0;

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

sf::Vector2i get_dimension_of_image(const std::string& filepath) {
	std::ifstream image(filepath);
	sf::Vector2i dimension;

	if(!image.good()) {
		print_error("Could not open \"", filepath, "\". Reason: ", strerror(errno));
		exit(1);
	}

	image.seekg(16);
	image.read((char*)&dimension.x, 4);
	image.read((char*)&dimension.y, 4);

	dimension.x = ntohl(dimension.x);
	dimension.y = ntohl(dimension.y);
	
	return dimension;
}

int main() {
	const std::string resource_font = "resources/fonts/Roboto-Bold.ttf";
	const std::string resource_fish_up = "resources/img/fish_up.png";
	const std::string resource_worm = "resources/img/worm.png";
	const std::string resource_pou_eating = "resources/sound/pou_eating.wav";

	sf::Vector2f fish_dimension = static_cast<sf::Vector2f>(get_dimension_of_image(resource_fish_up));
	sf::Vector2f worm_dimension = static_cast<sf::Vector2f>(get_dimension_of_image(resource_worm));

	print_info("fish_dimension.x = ", fish_dimension.x, "px");
	print_info("fish_dimension.y = ", fish_dimension.y, "px");

	print_info("worm_dimension.x = ", worm_dimension.x, "px");
	print_info("worm_dimension.y = ", worm_dimension.y, "px");

	sf::RenderWindow window(sf::VideoMode(SCREEN_DIMENSION.x, SCREEN_DIMENSION.y), "Overeater", sf::Style::Close);
	sf::RectangleShape fish(fish_dimension);
	sf::RectangleShape worm(worm_dimension);

	const sf::Vector2f START_POS(SCREEN_DIMENSION.x / 2 - fish_dimension.x / 2, SCREEN_DIMENSION.y / 2 - fish_dimension.y / 2);

	print_info("START_POS.x = ", START_POS.x);
	print_info("START_POS.y = ", START_POS.y);

	sf::Font font;
	font.loadFromFile(resource_font);

	sf::Text text;
	text.setFont(font);
	text.setCharacterSize(50);
	text.setFillColor(sf::Color::Red);
	text.setStyle(sf::Text::Bold);
	text.setString("Poeni: " + std::to_string(score));
	text.setPosition(SCREEN_DIMENSION.x / 2 - text.getGlobalBounds().width / 2, SCREEN_DIMENSION.y / 2 - text.getGlobalBounds().height / 2);

	sf::Texture fish_texture;
	sf::Texture worm_texture;
	fish_texture.loadFromFile(resource_fish_up);
	worm_texture.loadFromFile(resource_worm);

	fish.setTexture(&fish_texture);
	fish.setPosition(START_POS);

	sf::SoundBuffer buf;
	buf.loadFromFile(resource_pou_eating);
	sf::Sound bip;
	bip.setBuffer(buf);
	bip.setVolume(50);

	srand(time(nullptr));

	sf::Vector2f worm_pos(static_cast<float>(rand() % SCREEN_DIMENSION.x), static_cast<float>(rand() % SCREEN_DIMENSION.y));

	worm.setPosition(worm_pos.x, worm_pos.y);
	worm.setTexture(&worm_texture);

	sf::FloatRect next_pos;
	sf::FloatRect fish_bounds;
	sf::FloatRect worm_bounds;

	print_info("SCREEN_DIMENSION.x = ", SCREEN_DIMENSION.x);
	print_info("SCREEN_DIMENSION.y = ", SCREEN_DIMENSION.y);
	print_info("mov_speed = ", mov_speed);

	sf::Event e;
	while (window.isOpen()) {
		delta_time = delta_clock.restart().asSeconds();

		while(window.pollEvent(e)) {
			switch(e.type) {
				case sf::Event::Closed: {
					window.close();
					break;
				}
			}
		}

		velocity.x = velocity.y = 0;

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) {
			fish.setRotation(-90.f);
			velocity.x += -mov_speed * delta_time;
		} else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
			fish.setRotation(90.f);
			velocity.x += mov_speed * delta_time;
		} else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) {
			fish.setRotation(0.f);
			velocity.y += -mov_speed * delta_time;
		} else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) {
			fish.setRotation(-180.f);
			velocity.y += mov_speed * delta_time;
		} else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::R)) {
			// Restart the game
		}

		fish.move(velocity);

		fish_bounds = fish.getGlobalBounds();
		worm_bounds = worm.getGlobalBounds();
		next_pos = fish_bounds;
		next_pos.left += velocity.x;
		next_pos.top += velocity.y;
		if(worm_bounds.intersects(next_pos)) {
			bip.play();
			worm_pos.x = static_cast<float>(rand() % SCREEN_DIMENSION.x);
			worm_pos.y = static_cast<float>(rand() % SCREEN_DIMENSION.y);
			text.setString("Points: " + std::to_string(++score));
			worm.setPosition(worm_pos);
			worm.setRotation(1.0f);
		}

		window.clear();
		window.draw(text);
		window.draw(fish);
		window.draw(worm);
		window.display();
	}
	return 0;
}