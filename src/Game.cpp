#include "Game.hpp"
#include "resources.hpp"

#include <iostream>

Game::Game() {
	sf::err().rdbuf(nullptr);
	initialize();
}

void Game::game_loop() {
	while(window.isOpen()) {
		delta_time = delta_clock.restart().asSeconds();
		fish.velocity = sf::Vector2f(0, 0);
		fish.position = fish.sprite.getPosition();

		handle_events();
		handle_keyboard();

		fish.sprite.move(fish.velocity);

		check_collision();
		render();
	}
}

void Game::initialize() {
	background_texture.loadFromMemory(bg_png, bg_png_size);
	fish.texture.loadFromMemory(fish_up_png, fish_up_png_size);
	worm.texture.loadFromMemory(worm_png, worm_png_size);

	screen_dimension = background_texture.getSize();
	fish.dimension = fish.texture.getSize();
	worm.dimension = worm.texture.getSize();

	video_mode = sf::VideoMode(screen_dimension.x, screen_dimension.y);
	window.create(video_mode, "Overeater", sf::Style::Close);
	
	start_pos = sf::Vector2f(screen_dimension.x / 2, screen_dimension.y / 2);
	
	font.loadFromMemory(Roboto_Bold_ttf, Roboto_Bold_ttf_size);

	score = 0;

	score_text.setString("Score: 0");
	score_text.setFont(font);
	score_text.setCharacterSize(50);
	score_text.setFillColor(sf::Color::Cyan);
	score_text.setStyle(sf::Text::Bold);
	score_text.setPosition(10, 10);

	background_sprite.setTexture(background_texture);
	fish.sprite.setTexture(fish.texture);
	fish.sprite.setOrigin(fish.dimension.x / 2, fish.dimension.y / 2);
	fish.sprite.setPosition(start_pos);

	fish.sound_buffer.loadFromMemory(pou_eating_wav, pou_eating_wav_size);
	fish.sound.setBuffer(fish.sound_buffer);
	fish.sound.setVolume(50);

	random_generator.seed(random_device());

	from_0_to_width = std::uniform_real_distribution<float>(0.f, screen_dimension.x);
	from_0_to_height = std::uniform_real_distribution<float>(0.f, screen_dimension.y);

	worm.position = sf::Vector2f(from_0_to_width(random_generator), from_0_to_height(random_generator));
	
	worm.sprite.setTexture(worm.texture);
	worm.sprite.setOrigin(
		worm.dimension.x / 2,
		worm.dimension.y / 2
	);
	worm.sprite.setPosition(worm.position);
	
	window.setFramerateLimit(60);

	fish.mov_speed = 700;
}

void Game::restart() {
	std::cout << "Restarting the game...\n";
	
	score = 0;
	score_text.setString("Score: 0");

	fish.sprite.setPosition(start_pos);
	fish.sprite.setRotation(0);

	worm.sprite.setPosition(
		from_0_to_width(random_generator), 
		from_0_to_height(random_generator)
	);
}

void Game::handle_events() {
	sf::Event event;
	while(window.pollEvent(event)) {
		switch(event.type) {
			case sf::Event::Closed: {
				window.close();
				break;
			}
			case sf::Event::KeyReleased: {
				if(event.key.code == sf::Keyboard::R) {
					restart();
				} else if(event.key.code == sf::Keyboard::Escape) {
					window.close();
				}
				break;
			}
		}
	}
}

void Game::handle_keyboard() {
	if(window.hasFocus()) {
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) {
			fish.sprite.setRotation(-90.f);
			fish.velocity.x += -fish.mov_speed * delta_time;
		} else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
			fish.sprite.setRotation(90.f);
			fish.velocity.x += fish.mov_speed * delta_time;
		} else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) {
			fish.sprite.setRotation(0.f);
			fish.velocity.y += -fish.mov_speed * delta_time;
		} else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) {
			fish.sprite.setRotation(-180.f);
			fish.velocity.y += fish.mov_speed * delta_time;
		}
	}
}

void Game::check_collision() {
	if(fish.sprite.getGlobalBounds().intersects(worm.sprite.getGlobalBounds())) {
		fish.sound.play();
		worm.position = sf::Vector2f(
			from_0_to_width(random_generator),
			from_0_to_height(random_generator)
		);
		score_text.setString("Score: " + std::to_string(++score));
		worm.sprite.setPosition(worm.position);
	}
	
	if(fish.position.x < 0) {
		fish.position.x = 0;
		fish.sprite.setPosition(fish.position);
	}
	if(fish.position.x > screen_dimension.x) {
		fish.position.x = screen_dimension.x;
		fish.sprite.setPosition(fish.position);
	}
	if(fish.position.y < 0) {
		fish.position.y = 0;
		fish.sprite.setPosition(fish.position);
	}
	if(fish.position.y > screen_dimension.y) {
		fish.position.y = screen_dimension.y;
		fish.sprite.setPosition(fish.position);
	}

	if(worm.position.x < 0) {
		worm.position.x = 0;
		worm.sprite.setPosition(worm.position);
	}
	if(worm.position.x > screen_dimension.x) {
		worm.position.x = screen_dimension.x;
		worm.sprite.setPosition(worm.position);
	}
	if(worm.position.y < 0) {
		worm.position.y = 0;
		worm.sprite.setPosition(worm.position);
	}
	if(worm.position.y > screen_dimension.y) {
		worm.position.y = screen_dimension.y;
		worm.sprite.setPosition(worm.position);
	}
}

void Game::render() {
	window.clear();
	window.draw(background_sprite);
	window.draw(score_text);
	window.draw(fish.sprite);
	window.draw(worm.sprite);
	window.display();
}