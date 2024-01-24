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
		entities.entity_fish.velocity = sf::Vector2f(0, 0);
		entities.entity_fish.fish_pos = entities.entity_fish.shape->getPosition();

		handle_events();
		handle_keyboard();

		entities.entity_fish.shape->move(entities.entity_fish.velocity);

		check_collision();
		render();
	}
}

Game::~Game() {
	if(is_initialized == true) {
		destroy();
	}
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

	if(entities.entity_fish.image.loadFromFile(resource_fish_up)) {
		std::cout << "Resource \"" << resource_fish_up << "\" is loaded successfully.\n";
	} else {
		err_msg << "Could not load resource \"" << resource_fish_up << "\". Error: " << strerror(errno);
		std::cerr << err_msg.str() << "\n";
		show_msg_box(err_msg.str());
		return false;
	}

	if(entities.entity_worm.image.loadFromFile(resource_worm)) {
		std::cout << "Resource \"" << resource_worm << "\" is loaded successfully.\n";
	} else {
		err_msg << "Could not load resource \"" << resource_worm << "\". Error: " << strerror(errno);
		std::cerr << err_msg.str() << "\n";
		show_msg_box(err_msg.str());
		return false;
	}

	screen_dimension = image_background.getSize();
	entities.entity_fish.dimension = entities.entity_fish.image.getSize();
	entities.entity_worm.dimension = entities.entity_worm.image.getSize();

	video_mode = sf::VideoMode(screen_dimension.x, screen_dimension.y);
	window = new sf::RenderWindow(video_mode, "Overeater", sf::Style::Close);
	if(window == nullptr) {
		std::cerr << "Could not allocate memory for window.\n";
		return false;
	}

	entities.entity_fish.shape = new sf::RectangleShape(sf::Vector2f(
		entities.entity_fish.dimension.x,
		entities.entity_fish.dimension.y
	));
	if(entities.entity_fish.shape == nullptr) {
		std::cerr << "Could not allocate memory for fish shape.\n";
		return false;
	}

	entities.entity_worm.shape = new sf::RectangleShape(sf::Vector2f(
		entities.entity_worm.dimension.x,
		entities.entity_worm.dimension.y
	));
	if(entities.entity_worm.shape == nullptr) {
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

	entities.entity_fish.texture = new sf::Texture();
	if(entities.entity_fish.texture == nullptr) {
		std::cerr << "Could not allocate memory for fish entity.\n";
		return false;
	}

	entities.entity_worm.texture = new sf::Texture();
	if(entities.entity_worm.texture == nullptr) {
		std::cerr << "Could not allocate memory for worm entity.\n";
		return false;
	}

	background_texture->loadFromImage(image_background);
	entities.entity_fish.texture->loadFromImage(entities.entity_fish.image);
	entities.entity_worm.texture->loadFromImage(entities.entity_worm.image);

	background_sprite = new sf::Sprite(*background_texture);
	if(background_sprite == nullptr) {
		std::cerr << "Could not allocate memory for background sprite.\n";
		return false;
	}

	entities.entity_fish.shape->setTexture(*&entities.entity_fish.texture);
	entities.entity_fish.shape->setOrigin(
		entities.entity_fish.dimension.x / 2,
		entities.entity_fish.dimension.y / 2
	);
	entities.entity_fish.shape->setPosition(start_pos);

	entities.entity_fish.sound_buffer = new sf::SoundBuffer();
	if(entities.entity_fish.sound_buffer == nullptr) {
		std::cerr << "Could not allocate memory for fish sound buffer.\n";
		return false;
	}

	if(entities.entity_fish.sound_buffer->loadFromFile(resource_pou_eating)) {
		std::cout << "Resource \"" << resource_pou_eating << "\" is loaded successfully.\n";
	} else {
		err_msg << "Could not load resource \"" << resource_pou_eating << "\". Error: " << strerror(errno);
		std::cerr << err_msg.str() << "\n";
		show_msg_box(err_msg.str());
		return false;
	}
	
	entities.entity_fish.eating_sound = new sf::Sound(*entities.entity_fish.sound_buffer);
	if(entities.entity_fish.eating_sound == nullptr) {
		std::cerr << "Could not allocate memory for fish eating sound.\n";
		return false;
	}

	entities.entity_fish.eating_sound->setVolume(50);

	from_0_to_width = std::uniform_real_distribution<float>(0.f, screen_dimension.x);
	from_0_to_height = std::uniform_real_distribution<float>(0.f, screen_dimension.y);

	entities.entity_worm.worm_pos = sf::Vector2f(from_0_to_width(random_generator), from_0_to_height(random_generator));
	
	entities.entity_worm.shape->setTexture(*&entities.entity_worm.texture);
	entities.entity_worm.shape->setOrigin(
		entities.entity_worm.dimension.x / 2,
		entities.entity_worm.dimension.y / 2
	);
	entities.entity_worm.shape->setPosition(entities.entity_worm.worm_pos);
	
	window->setFramerateLimit(60);

	entities.entity_fish.mov_speed = DEFAULT_FISH_MOV_SPEED;
		
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
	if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) {
		entities.entity_fish.shape->setRotation(-90.f);
		entities.entity_fish.velocity.x += -entities.entity_fish.mov_speed * delta_time;
	} else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
		entities.entity_fish.shape->setRotation(90.f);
		entities.entity_fish.velocity.x += entities.entity_fish.mov_speed * delta_time;
	} else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) {
		entities.entity_fish.shape->setRotation(0.f);
		entities.entity_fish.velocity.y += -entities.entity_fish.mov_speed * delta_time;
	} else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) {
		entities.entity_fish.shape->setRotation(-180.f);
		entities.entity_fish.velocity.y += entities.entity_fish.mov_speed * delta_time;
	} else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::R)) {
		restart();
	} else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape)) {
		window->close();
	}
}

void Game::check_collision() {
	if(entities.entity_fish.shape->getGlobalBounds().intersects(entities.entity_worm.shape->getGlobalBounds())) {
		entities.entity_fish.eating_sound->play();
		entities.entity_worm.worm_pos = sf::Vector2f(
			from_0_to_width(random_generator),
			from_0_to_height(random_generator)
		);
		score_text->setString("Score: " + std::to_string(++score));
		entities.entity_worm.shape->setPosition(entities.entity_worm.worm_pos);
	}

	if(entities.entity_fish.fish_pos.x < 0) {
		entities.entity_fish.fish_pos.x = 0;
		entities.entity_fish.shape->setPosition(entities.entity_fish.fish_pos);
	}
	if(entities.entity_fish.fish_pos.x > screen_dimension.x) {
		entities.entity_fish.fish_pos.x = screen_dimension.x;
		entities.entity_fish.shape->setPosition(entities.entity_fish.fish_pos);
	}
	if(entities.entity_fish.fish_pos.y < 0) {
		entities.entity_fish.fish_pos.y = 0;
		entities.entity_fish.shape->setPosition(entities.entity_fish.fish_pos);
	}
	if(entities.entity_fish.fish_pos.y > screen_dimension.y) {
		entities.entity_fish.fish_pos.y = screen_dimension.y;
		entities.entity_fish.shape->setPosition(entities.entity_fish.fish_pos);
	}

	if(entities.entity_worm.worm_pos.x < 0) {
		entities.entity_worm.worm_pos.x = 0;
		entities.entity_worm.shape->setPosition(entities.entity_worm.worm_pos);
	}
	if(entities.entity_worm.worm_pos.x > screen_dimension.x) {
		entities.entity_worm.worm_pos.x = screen_dimension.x;
		entities.entity_worm.shape->setPosition(entities.entity_worm.worm_pos);
	}
	if(entities.entity_worm.worm_pos.y < 0) {
		entities.entity_worm.worm_pos.y = 0;
		entities.entity_worm.shape->setPosition(entities.entity_worm.worm_pos);
	}
	if(entities.entity_worm.worm_pos.y > screen_dimension.y) {
		entities.entity_worm.worm_pos.y = screen_dimension.y;
		entities.entity_worm.shape->setPosition(entities.entity_worm.worm_pos);
	}
}

void Game::render() {
	window->clear();
	window->draw(*background_sprite);
	window->draw(*score_text);
	window->draw(*entities.entity_fish.shape);
	window->draw(*entities.entity_worm.shape);
	window->display();
}

void Game::destroy() {
	delete window;
	delete entities.entity_fish.shape;
	delete entities.entity_worm.shape;
	delete font;
	delete score_text;
	delete background_texture;
	delete entities.entity_fish.texture;
	delete entities.entity_worm.texture;
	delete background_sprite;
	delete entities.entity_fish.sound_buffer;
	delete entities.entity_fish.eating_sound;
}