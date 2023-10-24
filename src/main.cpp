#include <iostream>
#include <opencv4/opencv2/opencv.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <termcolor.hpp>
#include <arpa/inet.h>

sf::Vector2i screen_dimension;

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
	cv::Mat image = cv::imread(filepath);

	if(image.empty()) {
		print_error("Could not open \"", filepath, "\".");
		exit(1);
	}

	sf::Vector2i dimension(image.cols, image.rows);
	
	return dimension;
}

int main() {
	const std::string resource_background = "resources/img/bg.jpg";
	const std::string resource_font = "resources/fonts/Roboto-Bold.ttf";
	const std::string resource_fish_up = "resources/img/fish_up.png";
	const std::string resource_worm = "resources/img/worm.png";
	const std::string resource_pou_eating = "resources/sound/pou_eating.wav";

	sf::Vector2i screen_dimension = get_dimension_of_image(resource_background);
	sf::Vector2f fish_dimension = static_cast<sf::Vector2f>(get_dimension_of_image(resource_fish_up));
	sf::Vector2f worm_dimension = static_cast<sf::Vector2f>(get_dimension_of_image(resource_worm));

	print_info("screen_dimension.x = ", screen_dimension.x, "px");
	print_info("screen_dimension.y = ", screen_dimension.y, "px");

	print_info("fish_dimension.x = ", fish_dimension.x, "px");
	print_info("fish_dimension.y = ", fish_dimension.y, "px");

	print_info("worm_dimension.x = ", worm_dimension.x, "px");
	print_info("worm_dimension.y = ", worm_dimension.y, "px");

	sf::RenderWindow window(sf::VideoMode(screen_dimension.x, screen_dimension.y), "Overeater", sf::Style::Close);
	sf::RectangleShape fish(fish_dimension);
	sf::RectangleShape worm(worm_dimension);

	const sf::Vector2f START_POS(screen_dimension.x / 2 - fish_dimension.x / 2, screen_dimension.y / 2 - fish_dimension.y / 2);

	print_info("START_POS.x = ", START_POS.x);
	print_info("START_POS.y = ", START_POS.y);

	sf::Font font;
	font.loadFromFile(resource_font);

	sf::Text text;
	text.setFont(font);
	text.setCharacterSize(50);
	text.setFillColor(sf::Color::Cyan);
	text.setStyle(sf::Text::Bold);
	text.setString("Score: " + std::to_string(score));
	text.setPosition(screen_dimension.x / 2 - text.getGlobalBounds().width / 2, screen_dimension.y / 2 - text.getGlobalBounds().height / 2);

	sf::Texture background_texture;
	sf::Texture fish_texture;
	sf::Texture worm_texture;
	background_texture.loadFromFile(resource_background);
	fish_texture.loadFromFile(resource_fish_up);
	worm_texture.loadFromFile(resource_worm);

	sf::Sprite background_sprite(background_texture);

	fish.setTexture(&fish_texture);
	fish.setPosition(START_POS);

	sf::SoundBuffer buf;
	buf.loadFromFile(resource_pou_eating);
	sf::Sound bip;
	bip.setBuffer(buf);
	bip.setVolume(50);

	srand(time(nullptr));

	sf::Vector2f worm_pos(static_cast<float>(rand() % screen_dimension.x), static_cast<float>(rand() % screen_dimension.y));

	worm.setTexture(&worm_texture);
	worm.setPosition(worm_pos.x, worm_pos.y);

	sf::FloatRect next_pos;
	sf::FloatRect fish_bounds;
	sf::FloatRect worm_bounds;

	print_info("mov_speed = ", mov_speed);

	window.setFramerateLimit(60);

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
			worm_pos.x = static_cast<float>(rand() % screen_dimension.x);
			worm_pos.y = static_cast<float>(rand() % screen_dimension.y);
			text.setString("Score: " + std::to_string(++score));
			worm.setPosition(worm_pos);
			worm.setRotation(1.0f);
		}

		window.clear();
		window.draw(background_sprite);
		window.draw(text);
		window.draw(fish);
		window.draw(worm);
		window.display();
	}
	return 0;
}