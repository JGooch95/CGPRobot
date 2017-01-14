#include <gl_core_4_3.hpp>
#include <iostream>
#include "scene.h"
#include "SFML/Graphics.hpp"

sf::RenderWindow *window;
Scene *currentScene;

void init()
{
	gl::ClearColor(0.5f, 0.5f, 0.5f, 1.0f); //Sets the screen clear colour

	//Loads a new scene 
	currentScene = new Scene;
	currentScene->load("Assets/scenes/Scene.xml");
}

void gameLoop()
{
	sf::Clock clock; //Makes a clock to control the frame rate

	while (window->isOpen())
	{
		sf::Event event;
		while (window->pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				window->close();
			}
			else if (event.type == sf::Event::Resized)
			{
			}
			if (event.type == sf::Event::MouseButtonPressed)
			{
				if (event.key.code == sf::Mouse::Left)
				{
					currentScene->switchCamera(+1);
				}
				if (event.key.code == sf::Mouse::Right)
				{
					currentScene->switchCamera(-1);
				}
			}
		}

		if (clock.getElapsedTime().asSeconds() > 1.0f / 60.0f) //Limits the update rate to be 60 frames per second
		{
			//Robot Controls
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
			{
				currentScene->moveRobot(1.0f * clock.getElapsedTime().asSeconds());
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
			{
				currentScene->turnRobot(1.0f * clock.getElapsedTime().asSeconds());
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
			{
				currentScene->moveRobot(-1.0f * clock.getElapsedTime().asSeconds());
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
			{
				currentScene->turnRobot(-1.0f * clock.getElapsedTime().asSeconds());
			}

			currentScene->update(); //Update the scene
			window->display();
			clock.restart();
		}
	}
}

void main()
{
	//Sets up the settings used for the window
	sf::ContextSettings settings;
	settings.depthBits = 24;
	settings.stencilBits = 8;
	settings.antialiasingLevel = 4;
	settings.majorVersion = 4;
	settings.minorVersion = 3;

	//Adds a new window
	window = new sf::RenderWindow(sf::VideoMode(1280, 720), "Open GL", sf::Style::Default, settings);

	//Loads openGL and checks if it has loaded correctly
	gl::exts::LoadTest GLLoaded = gl::sys::LoadFunctions();
	if (!GLLoaded)
	{
		std::cout << "openGL failed to load";
	}
	else //If openGL has loaded load the start the software
	{
		init();
		gameLoop();
	}

	//Clears all pointers
	delete(window);
	window = NULL;
	currentScene->~Scene();
	delete(currentScene);
	currentScene = NULL;

	exit(EXIT_FAILURE);
}