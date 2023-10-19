#include <iostream>
#include <string>
#include <vector>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <termcolor.hpp>

static constexpr float GRID_SIZE = 50.f;
static constexpr size_t SCREEN_WIDTH = 1152;
static constexpr size_t SCREEN_HEIGHT = 864;

sf::Clock delta_clock;
sf::Vector2f velocity;
float delta_time;
static float fish_width = 76.0f, fish_height = 162.0f, worm_width = 50.0f, worm_height = 50.0f, mov_speed = 700.f;
static size_t score = 0;

template<typename StreamT, typename... ArgsT>
void print(StreamT& stream, ArgsT&&... args) {
	(stream << ... << args);
	(stream << "\n");
}

template<typename... ArgsT>
void print_info(ArgsT&&... args) {
	print(std::cout, termcolor::green<char>, "[INFO]: ", termcolor::reset<char>, args...);
}

int main() {
	sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "2D Zderonja", sf::Style::Close | sf::Style::Resize);
	sf::RectangleShape fish(sf::Vector2f(fish_width, fish_height));
	sf::RectangleShape worm(sf::Vector2f(worm_width, worm_height));

	const float START_X = SCREEN_WIDTH / 2 - fish_width / 2;
	const float START_Y = SCREEN_HEIGHT / 2 - fish_height / 2;

	print_info("START_X = ", START_X);
	print_info("START_Y = ", START_Y);

	sf::Font font;
	font.loadFromFile("resources/fonts/Roboto-Bold.ttf");

	sf::Text text;
	text.setFont(font);
	text.setCharacterSize(50);
	text.setFillColor(sf::Color::Red);
	text.setStyle(sf::Text::Bold);
	text.setString("Poeni: " + std::to_string(score));
	text.setPosition(SCREEN_WIDTH / 2 - text.getGlobalBounds().width / 2, SCREEN_HEIGHT / 2 - text.getGlobalBounds().height / 2);

	sf::Texture fish_texture;
	sf::Texture worm_texture;
	fish_texture.loadFromFile("resources/img/fish_up.png");
	worm_texture.loadFromFile("resources/img/worm.png");

	fish.setTexture(&fish_texture);
	fish.setPosition(START_X, START_Y);

	srand(time(nullptr));

	sf::SoundBuffer buf;
	buf.loadFromFile("resources/sound/pou_eating.wav");
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

		while (window.pollEvent(e)) {
			switch (e.type) {
			case sf::Event::Closed:
				window.close();
				break;
			case sf::Event::Resized:
				window.clear();
				break;
			}
		}

		velocity.x = 0;
		velocity.y = 0;

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