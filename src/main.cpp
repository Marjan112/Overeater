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

static constexpr float GRID_SIZE = 50.f;
static constexpr int SCREEN_WIDTH = 1152;
static constexpr int SCREEN_HEIGHT = 864;

sf::Clock delta_clock;
sf::Vector2f velocity;
float delta_time;
float fish_width, fish_height, worm_width, worm_height;
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

	fish_width = get_dimension_of_image(resource_fish_up).x;
	fish_height = get_dimension_of_image(resource_fish_up).y;

	worm_width = get_dimension_of_image(resource_worm).x;
	worm_height = get_dimension_of_image(resource_worm).y;

	print_info("fish_width = ", fish_width, "px");
	print_info("fish_height = ", fish_height, "px");

	print_info("worm_width = ", worm_width, "px");
	print_info("worm_height = ", worm_height, "px");

	sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "2D Zderonja", sf::Style::Close);
	sf::RectangleShape fish(sf::Vector2f(fish_width, fish_height));
	sf::RectangleShape worm(sf::Vector2f(worm_width, worm_height));

	const float START_X = SCREEN_WIDTH / 2 - fish_width / 2;
	const float START_Y = SCREEN_HEIGHT / 2 - fish_height / 2;

	print_info("START_X = ", START_X);
	print_info("START_Y = ", START_Y);

	sf::Font font;
	font.loadFromFile(resource_font);

	sf::Text text;
	text.setFont(font);
	text.setCharacterSize(50);
	text.setFillColor(sf::Color::Red);
	text.setStyle(sf::Text::Bold);
	text.setString("Poeni: " + std::to_string(score));
	text.setPosition(SCREEN_WIDTH / 2 - text.getGlobalBounds().width / 2, SCREEN_HEIGHT / 2 - text.getGlobalBounds().height / 2);

	sf::Texture fish_texture;
	sf::Texture worm_texture;
	fish_texture.loadFromFile(resource_fish_up);
	worm_texture.loadFromFile(resource_worm);

	fish.setTexture(&fish_texture);
	fish.setPosition(START_X, START_Y);

	srand(time(nullptr));

	sf::SoundBuffer buf;
	buf.loadFromFile(resource_pou_eating);
	sf::Sound bip;
	bip.setBuffer(buf);
	bip.setVolume(50);

	float worm_x = static_cast<float>(rand() % SCREEN_WIDTH);
	float worm_y = static_cast<float>(rand() % SCREEN_HEIGHT);

	worm.setPosition(worm_x, worm_y);
	worm.setTexture(&worm_texture);

	sf::FloatRect next_pos;
	sf::FloatRect fish_bounds;
	sf::FloatRect worm_bounds;

	print_info("SCREEN_WIDTH = ", SCREEN_WIDTH);
	print_info("SCREEN_HEIGHT = ", SCREEN_HEIGHT);
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
		}

		fish.move(velocity);

		fish_bounds = fish.getGlobalBounds();
		worm_bounds = worm.getGlobalBounds();
		next_pos = fish_bounds;
		next_pos.left += velocity.x;
		next_pos.top += velocity.y;
		if(worm_bounds.intersects(next_pos)) {
			bip.play();
			worm_x = static_cast<float>(rand() % SCREEN_WIDTH);
			worm_y = static_cast<float>(rand() % SCREEN_HEIGHT);
			text.setString("Points: " + std::to_string(++score));
			worm.setPosition(worm_x, worm_y);
			worm.setRotation(1.0f);
			fish.setSize(sf::Vector2f(fish_width, fish_height));
		}

		window.clear();
		window.draw(text);
		window.draw(fish);
		window.draw(worm);
		window.display();
	}
	return 0;
}