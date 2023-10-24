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

	sf::Vector2i* screen_dimension = new sf::Vector2i(get_dimension_of_image(resource_background));
	sf::Vector2f* fish_dimension = new sf::Vector2f(static_cast<sf::Vector2f>(get_dimension_of_image(resource_fish_up)));
	sf::Vector2f* worm_dimension = new sf::Vector2f(static_cast<sf::Vector2f>(get_dimension_of_image(resource_worm)));

	print_info("screen_dimension->x = ", screen_dimension->x, "px");
	print_info("screen_dimension->y = ", screen_dimension->y, "px");

	print_info("fish_dimension->x = ", fish_dimension->x, "px");
	print_info("fish_dimension->y = ", fish_dimension->y, "px");

	print_info("worm_dimension->x = ", worm_dimension->x, "px");
	print_info("worm_dimension->y = ", worm_dimension->y, "px");

	const sf::VideoMode video_mode(screen_dimension->x, screen_dimension->y);

	sf::RenderWindow* window = new sf::RenderWindow(video_mode, "Overeater", sf::Style::Close);
	sf::RectangleShape* fish = new sf::RectangleShape(*fish_dimension);
	sf::RectangleShape* worm = new sf::RectangleShape(*worm_dimension);

	sf::Vector2f start_pos(screen_dimension->x / 2 - fish_dimension->x / 2, screen_dimension->y / 2 - fish_dimension->y / 2);

	print_info("start_pos.x = ", start_pos.x);
	print_info("start_pos.y = ", start_pos.y);

	sf::Font* font = new sf::Font();

	if(font->loadFromFile(resource_font)) {
		print_info("Resource \"", resource_font, "\" is loaded successfully.");
	}

	sf::Text* score_text = new sf::Text();
	score_text->setFont(*font);
	score_text->setCharacterSize(50);
	score_text->setFillColor(sf::Color::Cyan);
	score_text->setStyle(sf::Text::Bold);
	score_text->setString("Score: " + std::to_string(score));
	score_text->setPosition(screen_dimension->x / 2 - score_text->getGlobalBounds().width / 2, screen_dimension->y / 2 - score_text->getGlobalBounds().height / 2);

	sf::Texture* background_texture = new sf::Texture();
	sf::Texture* fish_texture = new sf::Texture();
	sf::Texture* worm_texture = new sf::Texture();

	if(background_texture->loadFromFile(resource_background)) {
		print_info("Resource \"", resource_background, "\" is loaded successfully.");
	}
	
	if(fish_texture->loadFromFile(resource_fish_up)) {
		print_info("Resource \"", resource_fish_up, "\" is loaded successfully.");
	}

	if(worm_texture->loadFromFile(resource_worm)) {
		print_info("Resource \"", resource_worm, "\" is loaded successfully.");
	}

	sf::Sprite* background_sprite = new sf::Sprite(*background_texture);

	fish->setTexture(*&fish_texture);
	fish->setPosition(start_pos);

	sf::SoundBuffer* sound_buffer = new sf::SoundBuffer();

	if(sound_buffer->loadFromFile(resource_pou_eating)) {
		print_info("Resource \"", resource_pou_eating, "\" is loaded successfully.");
	}
	
	sf::Sound* beep = new sf::Sound();
	beep->setBuffer(*sound_buffer);
	beep->setVolume(50);

	srand(time(nullptr));

	sf::Vector2f worm_pos(static_cast<float>(rand() % (screen_dimension->x - 4)), static_cast<float>(rand() % (screen_dimension->y - 4)));

	worm->setTexture(*&worm_texture);
	worm->setPosition(worm_pos.x, worm_pos.y);

	sf::FloatRect next_pos;
	sf::FloatRect fish_bounds;
	sf::FloatRect worm_bounds;

	print_info("mov_speed = ", mov_speed);

	window->setFramerateLimit(60);

	sf::Event e;
	while (window->isOpen()) {
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
			print(std::cout, "========================================");
			print_info("Restarting the game...");
			print(std::cout, "========================================");

			score = 0;

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

			screen_dimension = new sf::Vector2i(get_dimension_of_image(resource_background));
			fish_dimension = new sf::Vector2f(static_cast<sf::Vector2f>(get_dimension_of_image(resource_fish_up)));
			worm_dimension = new sf::Vector2f(static_cast<sf::Vector2f>(get_dimension_of_image(resource_worm)));

			print_info("screen_dimension->x = ", screen_dimension->x, "px");
			print_info("screen_dimension->y = ", screen_dimension->y, "px");

			print_info("fish_dimension->x = ", fish_dimension->x, "px");
			print_info("fish_dimension->y = ", fish_dimension->y, "px");

			print_info("worm_dimension->x = ", worm_dimension->x, "px");
			print_info("worm_dimension->y = ", worm_dimension->y, "px");

			window = new sf::RenderWindow(video_mode, "Overeater", sf::Style::Close);
			fish = new sf::RectangleShape(*fish_dimension);
			worm = new sf::RectangleShape(*worm_dimension);

			start_pos = sf::Vector2f(screen_dimension->x / 2 - fish_dimension->x / 2, screen_dimension->y / 2 - fish_dimension->y / 2);

			print_info("start_pos.x = ", start_pos.x);
			print_info("start_pos.y = ", start_pos.y);

			font = new sf::Font();

			if(font->loadFromFile(resource_font)) {
				print_info("Resource \"", resource_font, "\" is loaded successfully.");
			}

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
				print_info("Resource \"", resource_background, "\" is loaded successfully.");
			}

			if(fish_texture->loadFromFile(resource_fish_up)) {
				print_info("Resource \"", resource_fish_up, "\" is loaded successfully.");
			}

			if(worm_texture->loadFromFile(resource_worm)) {
				print_info("Resource \"", resource_worm, "\" is loaded successfully.");
			}

			background_sprite = new sf::Sprite(*background_texture);

			fish->setTexture(*&fish_texture);
			fish->setPosition(start_pos);

			sound_buffer = new sf::SoundBuffer();
			
			if(sound_buffer->loadFromFile(resource_pou_eating)) {
				print_info("Resource \"", resource_pou_eating, "\" is loaded successfully.");
			}

			beep = new sf::Sound();
			beep->setBuffer(*sound_buffer);
			beep->setVolume(50);

			srand(time(nullptr));

			worm_pos = sf::Vector2f(static_cast<float>(rand() % (screen_dimension->x - 4)), static_cast<float>(rand() % (screen_dimension->y - 4)));

			worm->setTexture(*&worm_texture);
			worm->setPosition(worm_pos.x, worm_pos.y);

			print_info("mov_speed = ", mov_speed);

			window->setFramerateLimit(60);
			continue;
		}

		fish->move(velocity);

		fish_bounds = fish->getGlobalBounds();
		worm_bounds = worm->getGlobalBounds();
		next_pos = fish_bounds;
		next_pos.left += velocity.x;
		next_pos.top += velocity.y;
		if(worm_bounds.intersects(next_pos)) {
			beep->play();
			worm_pos.x = static_cast<float>(rand() % (screen_dimension->x - 4));
			worm_pos.y = static_cast<float>(rand() % (screen_dimension->y - 4));
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

	return 0;
}