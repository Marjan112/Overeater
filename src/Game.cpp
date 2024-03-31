#include "Game.hpp"

#include <iostream>
#include <sstream>
#include <cstring>

#if defined(__linux__)
#	include <gtk/gtk.h>
#endif

#include "resources.hpp"
#include "msgbox.hpp"

Game::Game() {
	#if defined(__linux__)
	gtk_init(nullptr, nullptr);
	#endif
	#if !defined(DEBUG)
	sf::err().rdbuf(nullptr);
	#endif
	is_initialized = initialize();
	if(is_initialized == false) {
		std::cerr << "Could not initialize game.\n";
	} else {
		random_generator.seed(random_device());
	}
}

void Game::game_loop() {
	while(is_initialized == true && window->isOpen()) {
		delta_time = delta_clock.restart().asSeconds();
		fish.velocity = sf::Vector2f(0, 0);
		fish.position = fish.shape->getPosition();

		handle_events();
		handle_keyboard();

		fish.shape->move(fish.velocity);

		check_collision();
		render();
	}
}

Game::~Game() {
	destroy();
}

bool Game::initialize() {
	std::stringstream err_msg;
	if(image_background.loadFromFile(resource_background)) {
		std::cout << "Resource \"" << resource_background << "\" is loaded successfully.\n";
	} else {
		err_msg << "Could not load resource \"" << resource_background << "\". Error: " << strerror(errno);
		std::cerr << err_msg.str() << "\n";
		show_msg_box(err_msg.str());
		return false;
	}

	if(fish.image.loadFromFile(resource_fish_up)) {
		std::cout << "Resource \"" << resource_fish_up << "\" is loaded successfully.\n";
	} else {
		err_msg << "Could not load resource \"" << resource_fish_up << "\". Error: " << strerror(errno);
		std::cerr << err_msg.str() << "\n";
		show_msg_box(err_msg.str());
		return false;
	}

	if(worm.image.loadFromFile(resource_worm)) {
		std::cout << "Resource \"" << resource_worm << "\" is loaded successfully.\n";
	} else {
		err_msg << "Could not load resource \"" << resource_worm << "\". Error: " << strerror(errno);
		std::cerr << err_msg.str() << "\n";
		show_msg_box(err_msg.str());
		return false;
	}

	screen_dimension = image_background.getSize();
	fish.dimension = fish.image.getSize();
	worm.dimension = worm.image.getSize();

	video_mode = sf::VideoMode(screen_dimension.x, screen_dimension.y);
	window = new sf::RenderWindow(video_mode, "Overeater", sf::Style::Close);
	if(window == nullptr) {
		std::cerr << "Could not allocate memory for window.\n";
		return false;
	}

	fish.shape = new sf::RectangleShape(sf::Vector2f(
		fish.dimension.x,
		fish.dimension.y
	));
	if(fish.shape == nullptr) {
		std::cerr << "Could not allocate memory for fish shape.\n";
		return false;
	}

	worm.shape = new sf::RectangleShape(sf::Vector2f(
		worm.dimension.x,
		worm.dimension.y
	));
	if(worm.shape == nullptr) {
		std::cerr << "Could not allocate memory for worm shape.\n";
		return false;
	}
	
	start_pos = sf::Vector2f(screen_dimension.x / 2, screen_dimension.y / 2);
	
	font = new sf::Font();
	if(font == nullptr) {
		std::cerr << "Could not allocate memory for font.\n";
		return false;
	}

	if(font->loadFromFile(resource_font)) {
		std::cout << "Resource \"" << resource_font << "\" is loaded successfully.\n";
	} else {
		err_msg << "Could not load resource \"" << resource_font << "\". Error: " << strerror(errno);
		std::cerr << err_msg.str() << "\n";
		show_msg_box(err_msg.str());
		return false;
	}

	score = 0;

	score_text = new sf::Text("Score: " + std::to_string(score), *font, 50);
	if(score_text == nullptr) {
		std::cerr << "Could not allocate memory for score text.\n";
		return false;
	}

	score_text->setFillColor(sf::Color::Cyan);
	score_text->setStyle(sf::Text::Bold);
	score_text->setPosition(10, 10);

	background_texture = new sf::Texture();
	if(background_texture == nullptr) {
		std::cerr << "Could not allocate memory for background texture.\n";
		return false;
	}

	fish.texture = new sf::Texture();
	if(fish.texture == nullptr) {
		std::cerr << "Could not allocate memory for fish entity.\n";
		return false;
	}

	worm.texture = new sf::Texture();
	if(worm.texture == nullptr) {
		std::cerr << "Could not allocate memory for worm entity.\n";
		return false;
	}

	background_texture->loadFromImage(image_background);
	fish.texture->loadFromImage(fish.image);
	worm.texture->loadFromImage(worm.image);

	background_sprite = new sf::Sprite(*background_texture);
	if(background_sprite == nullptr) {
		std::cerr << "Could not allocate memory for background sprite.\n";
		return false;
	}

	fish.shape->setTexture(*&fish.texture);
	fish.shape->setOrigin(
		fish.dimension.x / 2,
		fish.dimension.y / 2
	);
	fish.shape->setPosition(start_pos);

	fish.sound_buffer = new sf::SoundBuffer();
	if(fish.sound_buffer == nullptr) {
		std::cerr << "Could not allocate memory for fish sound buffer.\n";
		return false;
	}

	if(fish.sound_buffer->loadFromFile(resource_pou_eating)) {
		std::cout << "Resource \"" << resource_pou_eating << "\" is loaded successfully.\n";
	} else {
		err_msg << "Could not load resource \"" << resource_pou_eating << "\". Error: " << strerror(errno);
		std::cerr << err_msg.str() << "\n";
		show_msg_box(err_msg.str());
		return false;
	}
	
	fish.sound = new sf::Sound(*fish.sound_buffer);
	if(fish.sound == nullptr) {
		std::cerr << "Could not allocate memory for fish eating sound.\n";
		return false;
	}

	fish.sound->setVolume(50);

	from_0_to_width = std::uniform_real_distribution<float>(0.f, screen_dimension.x);
	from_0_to_height = std::uniform_real_distribution<float>(0.f, screen_dimension.y);

	worm.position = sf::Vector2f(from_0_to_width(random_generator), from_0_to_height(random_generator));
	
	worm.shape->setTexture(*&worm.texture);
	worm.shape->setOrigin(
		worm.dimension.x / 2,
		worm.dimension.y / 2
	);
	worm.shape->setPosition(worm.position);
	
	window->setFramerateLimit(60);

	fish.mov_speed = DEFAULT_FISH_MOV_SPEED;
		
	return true;
}

void Game::restart() {
	std::cout << "Restarting the game...\n";

	destroy();
	is_initialized = initialize();
	if(is_initialized == false) {
		std::cerr << "Could not initialize game.\n";
	}
}

void Game::handle_events() {
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

void Game::handle_keyboard() {
	if(window->hasFocus()) {
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) {
			fish.shape->setRotation(-90.f);
			fish.velocity.x += -fish.mov_speed * delta_time;
		} else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
			fish.shape->setRotation(90.f);
			fish.velocity.x += fish.mov_speed * delta_time;
		} else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) {
			fish.shape->setRotation(0.f);
			fish.velocity.y += -fish.mov_speed * delta_time;
		} else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) {
			fish.shape->setRotation(-180.f);
			fish.velocity.y += fish.mov_speed * delta_time;
		} else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::R)) {
			restart();
		} else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape)) {
			window->close();
		}
	}
}

void Game::check_collision() {
	if(fish.shape->getGlobalBounds().intersects(worm.shape->getGlobalBounds())) {
		fish.sound->play();
		worm.position = sf::Vector2f(
			from_0_to_width(random_generator),
			from_0_to_height(random_generator)
		);
		score_text->setString("Score: " + std::to_string(++score));
		worm.shape->setPosition(worm.position);
	}
	
	if(fish.position.x < 0) {
		fish.position.x = 0;
		fish.shape->setPosition(fish.position);
	}
	if(fish.position.x > screen_dimension.x) {
		fish.position.x = screen_dimension.x;
		fish.shape->setPosition(fish.position);
	}
	if(fish.position.y < 0) {
		fish.position.y = 0;
		fish.shape->setPosition(fish.position);
	}
	if(fish.position.y > screen_dimension.y) {
		fish.position.y = screen_dimension.y;
		fish.shape->setPosition(fish.position);
	}

	if(worm.position.x < 0) {
		worm.position.x = 0;
		worm.shape->setPosition(worm.position);
	}
	if(worm.position.x > screen_dimension.x) {
		worm.position.x = screen_dimension.x;
		worm.shape->setPosition(worm.position);
	}
	if(worm.position.y < 0) {
		worm.position.y = 0;
		worm.shape->setPosition(worm.position);
	}
	if(worm.position.y > screen_dimension.y) {
		worm.position.y = screen_dimension.y;
		worm.shape->setPosition(worm.position);
	}
}

void Game::render() {
	window->clear();
	window->draw(*background_sprite);
	window->draw(*score_text);
	window->draw(*fish.shape);
	window->draw(*worm.shape);
	window->display();
}

void Game::destroy() {
	delete window;
	delete fish.shape;
	delete worm.shape;
	delete font;
	delete score_text;
	delete background_texture;
	delete fish.texture;
	delete worm.texture;
	delete background_sprite;
	delete fish.sound_buffer;
	delete fish.sound;
}