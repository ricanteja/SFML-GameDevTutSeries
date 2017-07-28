#include <iostream>
#include <string>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#define UPDATE_RATE 1.0f / 60.0f
#define UPDATE_LIMIT 10


// Better than system("pause");
int systemPause(int flag = 0)
{
	std::cout << "Press the [Return] key to continue...\n";

	while(!sf::Keyboard::isKeyPressed(sf::Keyboard::Return))
	{
		// Delay so we don't hog CPU cycles
		sf::sleep(sf::milliseconds(20));
	}

	// Return the flag passed through
	return flag;
}

int main()
{
	// Setup the resolutions for windowed and fullscreen mode
	sf::VideoMode mode_windowed(640, 420);
	// The getFullscreenModes() function returns an array of
	// compatible fullscreen video modes supported by the
	// system, the first element being the best one by default
	sf::VideoMode mode_fullscreen = sf::VideoMode::getFullscreenModes()[0];

	// Flag for toggling fullscreen mode
	bool is_fullscreen = false;

	sf::RenderWindow window(mode_windowed, "Basic SFML Game", sf::Style::Close);

	// The view will allow us to maintain our 'zoom' when
	// we fullscreen the window to scale up
	sf::View view = window.getView();

	sf::Clock clock;
	float total_time = 0.0f;
	float frame_time = 0.0f;
	int update_count = 0;

	// How we will be keeping track of our score
	unsigned int score = 0;

	// Seed the random number generator
	srand(clock.restart().asSeconds());

	// Load our graphics, font and sound resources into buffers

	sf::Texture tex_bird;
	// If the texture failed to load for whatever reason, quit!
	if(!tex_bird.loadFromFile("./resources/bird.png"))
		return systemPause(EXIT_FAILURE);

	sf::Texture tex_stick;
	if(!tex_stick.loadFromFile("./resources/stick.png"))
		return systemPause(EXIT_FAILURE);

	sf::Texture tex_grass;
	// Setting the repeat on the texture causes it to tile auto-
	// magically when it is applied to a sprite too large for it
	tex_grass.setRepeated(true);
	if(!tex_grass.loadFromFile("./resources/grass.png"))
		return systemPause(EXIT_FAILURE);

	sf::Font font_message;
	if(!font_message.loadFromFile("./resources/press_start.ttf"))
		return systemPause(EXIT_FAILURE);

	sf::SoundBuffer sndbuf_pickup;
	if(!sndbuf_pickup.loadFromFile("./resources/pickup_stick.wav"))
		return systemPause(EXIT_FAILURE);

	// Create our sprites, which act as billboards on to which
	// we apply the textures we loaded earlier

	// The bird which is our player and center its position on screen
	sf::Sprite bird(tex_bird);
	bird.setPosition(mode_windowed.width / 2, mode_windowed.height / 2);

	// The stick sprite our bird player will pick up
	sf::Sprite stick(tex_stick);
	stick.setPosition(mode_windowed.width / 2 + 200, mode_windowed.height / 2);

	// The grass sprite that will be used as our background
	sf::Sprite grass(tex_grass);
	// By setting the texture rect to the size of the window we
	// will cause the texture to repeat, making a nice easy background
	grass.setTextureRect(sf::IntRect(0, 0, 640, 480));

	sf::Text ui_score("Score: 0", font_message, 24);
	ui_score.setOutlineThickness(2);
	ui_score.setOutlineColor(sf::Color::Black);
	ui_score.setPosition(5.0f, 5.0f);
	// This will be our limit for how high up we can go, we don't
	// want our player to go past our score gui, it will look weird!
	float top_border = ui_score.getPosition().y + ui_score.getLocalBounds().height;

	// The sound object that will play what was loaded into the
	// sound buffer from earlier
	sf::Sound sound_pickup(sndbuf_pickup);

	while(window.isOpen())
	{
		frame_time = clock.restart().asSeconds();
		total_time += frame_time;
		update_count = 0;

		while(total_time >= UPDATE_RATE && update_count < UPDATE_LIMIT)
		{
			sf::Event evt;

			while(window.pollEvent(evt))
			{
				if(evt.type == sf::Event::Closed)
					window.close();
			}

			// Move the player
			if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
				bird.move(0.0f, -5.0f);
			if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
				bird.move(0.0f, 5.0f);
			if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
				bird.move(-5.0f, 0.0f);
			if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
				bird.move(5.0f, 0.0f);
			// Set the window to fullscreen when F11 is hit
			if(sf::Keyboard::isKeyPressed(sf::Keyboard::F11))
			{
				if(is_fullscreen)
					window.create(mode_windowed, "SFML Picking Sticks", sf::Style::Close);
				else
					window.create(mode_fullscreen, "", sf::Style::Fullscreen);

				window.setView(view);
				is_fullscreen = !is_fullscreen;
			}

			// Keep player in bounds
			if(bird.getPosition().x < 0)
				bird.move(5.0f, 0.0f);
			if(bird.getPosition().y < top_border)
				bird.move(0.0f, 5.0f);
			if(bird.getPosition().x + bird.getGlobalBounds().width > mode_windowed.width)
				bird.move(-5.0f, 0.0f);
			if(bird.getPosition().y + bird.getLocalBounds().height > mode_windowed.height)
				bird.move(0.0f, -5.0f);

			// Check if the player is colliding with the stick
			if(bird.getGlobalBounds().intersects(stick.getGlobalBounds()))
			{
				// If the player is colliding with the stick play
				// a sound and increase the score as well as update
				// the score text so the player can see it
				sound_pickup.play();
				score++;
				ui_score.setString("Score: " + std::to_string(score));
				// Finally pick a new random position for the stick
				// while making sure it isn't partially offscreen
				int limit_x = mode_windowed.width - stick.getLocalBounds().width;
				int limit_y = mode_windowed.height - stick.getGlobalBounds().height - top_border;
				stick.setPosition(rand() % limit_x, top_border + rand() % limit_y);
			}

			total_time -= UPDATE_RATE;
			update_count++;
		}

		window.clear();

		// Draw the grass first so it is in the background
		window.draw(grass);
		// Then draw everything else on top
		window.draw(bird);
		window.draw(stick);
		window.draw(ui_score);

		window.display();

		std::cout << "\rfps: " << 1.0f / frame_time;
	}

	return 0;
}
