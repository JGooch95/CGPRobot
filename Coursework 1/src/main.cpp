#include <gl_core_4_3.hpp>
#include <iostream>
#include "scene.h"
#include "SFML/Graphics.hpp"

sf::RenderWindow *window;
Scene *currentScene = NULL;
enum Screens {MainMenu, Game};
Screens currentScreen = MainMenu;
UIText textHolder;

void loadScene()
{
	gl::ClearColor(0.5f, 0.5f, 0.5f, 1.0f); //Sets the screen clear colour
	//Loads a new scene 
	if (currentScene != NULL)
	{
		currentScene->~Scene();
		delete(currentScene);
		currentScene = NULL;
	}

	currentScene = new Scene(&textHolder);
	if (currentScreen == Game)
	{
		currentScene->load("Assets/scenes/Scene.xml");
	}
	else if (currentScreen == MainMenu)
	{
		currentScene->load("Assets/scenes/MenuScene.xml");
	}
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
				window->setView(sf::View(sf::FloatRect(0.0f, 0.0f,(float)window->getSize().x,(float)window->getSize().y))); //Resizes the window view to fit to the screen
				gl::Viewport(0, 0, window->getSize().x, window->getSize().y); //Resizes the viewport
			}
			if (event.type == sf::Event::MouseButtonPressed)
			{
				if (currentScreen == Game)
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
			if (event.type == sf::Event::KeyPressed)
			{
				if (event.key.code == sf::Keyboard::Return)
				{
					if (currentScreen == MainMenu)
					{
						currentScreen = Game;
						loadScene();
					}
					else if (currentScreen == Game && currentScene->getSceneOver())
					{
						currentScreen = MainMenu;
						loadScene();
					}
				}
			}
			if (event.type == sf::Event::KeyReleased)
			{
				if (currentScreen == Game)
				{
					if (event.key.code == sf::Keyboard::W || event.key.code == sf::Keyboard::Up)
					{
						currentScene->setRobotMoving(false);
					}
					if (event.key.code == sf::Keyboard::A || event.key.code == sf::Keyboard::Left)
					{
						currentScene->setRobotMoving(false);
					}
					if (event.key.code == sf::Keyboard::S || event.key.code == sf::Keyboard::Down)
					{
						currentScene->setRobotMoving(false);
					}
					if (event.key.code == sf::Keyboard::D || event.key.code == sf::Keyboard::Right)
					{
						currentScene->setRobotMoving(false);
					}
				}
			}
		}

		if (clock.getElapsedTime().asSeconds() > 1.0f / 60.0f) //Limits the update rate to be 60 frames per second
		{
			if (currentScreen == Game)
			{
				//Robot Controls
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up))
				{
					currentScene->moveRobot(1.0f * clock.getElapsedTime().asSeconds());
					currentScene->setRobotMoving(true);
				}
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))
				{
					currentScene->turnRobot(1.0f * clock.getElapsedTime().asSeconds());
					currentScene->setRobotMoving(true);
				}
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down))
				{
					currentScene->moveRobot(-1.0f * clock.getElapsedTime().asSeconds());
					currentScene->setRobotMoving(true);
				}
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right))
				{
					currentScene->turnRobot(-1.0f * clock.getElapsedTime().asSeconds());
					currentScene->setRobotMoving(true);
				}
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
	window = new sf::RenderWindow(sf::VideoMode(1280, 720), "Robot Game", sf::Style::Default, settings);

	//Loads openGL and checks if it has loaded correctly
	gl::exts::LoadTest GLLoaded = gl::sys::LoadFunctions();
	if (!GLLoaded)
	{
		std::cout << "openGL failed to load";
	}
	else //If openGL has loaded load the start the software
	{
		textHolder.loadCharacters();
		textHolder.setupBuffers();
		loadScene();
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