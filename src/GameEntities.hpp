#if defined(__linux__)
#	include "../include/SFML_Linux/Graphics.hpp"
#	include "../include/SFML_Linux/Audio.hpp"
#elif defined(_MSC_VER)
#	include "../include/SFML_MSC_Windows/Graphics.hpp"
#	include "../include/SFML_MSC_Windows/Audio.hpp"
#endif

struct GameEntityFish {
	float mov_speed;
	sf::Vector2f velocity;
	sf::Image image;
	sf::Vector2u dimension;
	sf::Texture* texture;
	sf::SoundBuffer* sound_buffer;
	sf::Sound* eating_sound;
	sf::RectangleShape* shape;
};

struct GameEntityWorm {
	sf::Image image;
	sf::Vector2u dimension;
	sf::Texture* texture;
	sf::RectangleShape* shape;
};

struct GameEntities {
	GameEntityFish entity_fish;
	GameEntityWorm entity_worm;
};