#include "Game.hpp"

#include <cstring>
#include <sstream>

Game::Game() {
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

		handle_events();
		handle_keyword();

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
	if(image_background.loadFromFile(resource_background)) {
		std::cout << "Successfully loaded dimension of image \"" << resource_background << "\".\n";
	} else {
		std::cerr << "Could not load dimension of image \"" << resource_background << "\". Error: " << strerror(errno) << "\n";
		return false;
	}

	if(entities.entity_fish.image.loadFromFile(resource_fish_up)) {
		std::cout << "Successfully loaded image dimension of \"" << resource_fish_up << "\".\n";
	} else {
		std::cerr << "Could not load dimension of image \"" << resource_fish_up << "\". Error: " << strerror(errno) << "\n";
		return false;
	}

	if(entities.entity_worm.image.loadFromFile(resource_worm)) {
		std::cout << "Successfully loaded image dimension of \"" << resource_worm << "\".\n";
	} else {
		std::cerr << "Could not load dimension of image \"" << resource_worm << "\". Error: " << strerror(errno) << "\n";
		return false;
	}

	screen_dimension = image_background.getSize();
	entities.entity_fish.dimension = entities.entity_fish.image.getSize();
	entities.entity_worm.dimension = entities.entity_worm.image.getSize();

	video_mode = sf::VideoMode(screen_dimension.x, screen_dimension.y);
	window = new sf::RenderWindow(video_mode, "Overeater", sf::Style::Close);

	entities.entity_fish.shape = new sf::RectangleShape(sf::Vector2f(
		entities.entity_fish.dimension.x,
		entities.entity_fish.dimension.y
	));
	entities.entity_worm.shape = new sf::RectangleShape(sf::Vector2f(
		entities.entity_worm.dimension.x,
		entities.entity_worm.dimension.y
	));
	
	start_pos = sf::Vector2f(
		screen_dimension.x / 2 - entities.entity_fish.dimension.x / 2,
		screen_dimension.y / 2 - entities.entity_fish.dimension.y / 2
	);
	
	font = new sf::Font();
	if(font->loadFromFile(resource_font)) {
		std::cout << "Resource \"" << resource_font << "\" is loaded successfully.\n";
	} else {
		std::cerr << "Could not load resource \"" << resource_font << "\". Error: " << strerror(errno) << "\n";
		return false;
	}

	score = 0;

	score_text = new sf::Text("Score: " + std::to_string(score), *font, 50);
	score_text->setFillColor(sf::Color::Cyan);
	score_text->setStyle(sf::Text::Bold);
	score_text->setPosition(
		screen_dimension.x / 2 - score_text->getGlobalBounds().width / 2,
		screen_dimension.y / 2 - score_text->getGlobalBounds().height / 2
	);

	background_texture = new sf::Texture();
	entities.entity_fish.texture = new sf::Texture();
	entities.entity_worm.texture = new sf::Texture();

	if(background_texture->loadFromImage(image_background)) {
		std::cout << "Resource \"" << resource_background << "\" is loaded successfully.\n";
	} else {
		std::cerr << "Could not load resource \"" << resource_background << "\". Error: " << strerror(errno) << "\n";
		return false;
	}
		
	if(entities.entity_fish.texture->loadFromImage(entities.entity_fish.image)) {
		std::cout << "Resource \"" << resource_fish_up << "\" is loaded successfully.\n";
	} else {
		std::cerr << "Could not load resource \"" << resource_fish_up << "\". Error: " << strerror(errno) << "\n";
		return false;
	}

	if(entities.entity_worm.texture->loadFromImage(entities.entity_worm.image)) {
		std::cout << "Resource \"" << resource_worm << "\" is loaded successfully.\n";
	} else {
		std::cerr << "Could not load resource \"" << resource_worm << "\". Error: " << strerror(errno) << "\n";
		return false;
	}

	background_sprite = new sf::Sprite(*background_texture);
	entities.entity_fish.shape->setTexture(*&entities.entity_fish.texture);
	entities.entity_fish.shape->setOrigin(
		entities.entity_fish.dimension.x / 2,
		entities.entity_fish.dimension.y / 2
	);
	entities.entity_fish.shape->setPosition(start_pos);

	entities.entity_fish.sound_buffer = new sf::SoundBuffer();

	if(entities.entity_fish.sound_buffer->loadFromFile(resource_pou_eating)) {
		std::cout << "Resource \"" << resource_pou_eating << "\" is loaded successfully.\n";
	} else {
		std::cerr << "Could not load resource \"" << resource_pou_eating << "\". Error: " << strerror(errno) << "\n";
		return false;
	}
	
	entities.entity_fish.eating_sound = new sf::Sound(*entities.entity_fish.sound_buffer);
	entities.entity_fish.eating_sound->setVolume(50);

	from_0_to_width = std::uniform_real_distribution<float>(0.f, screen_dimension.x);
	from_0_to_height = std::uniform_real_distribution<float>(0.f, screen_dimension.y);

	worm_pos = sf::Vector2f(from_0_to_width(random_generator), from_0_to_height(random_generator));
	
	entities.entity_worm.shape->setTexture(*&entities.entity_worm.texture);
	entities.entity_worm.shape->setPosition(worm_pos);
	
	window->setFramerateLimit(60);

	entities.entity_fish.mov_speed = DEFAULT_FISH_MOV_SPEED;
		
	return true;
}

void Game::restart() {
	std::cout << "Restarting the game...\n";

	destroy();
	if(!initialize()) {
		std::cerr << "Could not initialize game.\n";
		exit(1);
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

void Game::handle_keyword() {
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
		worm_pos = sf::Vector2f(
			from_0_to_width(random_generator),
			from_0_to_height(random_generator)
		);
		score_text->setString("Score: " + std::to_string(++score));
		entities.entity_worm.shape->setPosition(worm_pos);
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