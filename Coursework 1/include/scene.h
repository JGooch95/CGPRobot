#pragma once
#include "model.h"
#include "Robot.h"
#include "Collectable.h"
#include "GameObject.h"
#include <tinyxml2.h>
#include <Array>
#include "UIText.h"
#include "glslprogram.h"
#include "SFML\Graphics.hpp"

class Scene
{
	private:
		std::vector<GLSLProgram*> m_vShaders; //Holds the pointers to the shaders being used
		std::vector<GameObject*> m_vObjects; //Holds pointers to every object
		std::vector<Robot*> m_vRobots; //Holds pointers to every robot
		std::vector<Collectable*> m_vCollectables; //Holds pointers to every collectable
		std::vector<GameObject*> m_vHUD; //Holds the pointers to the 2d Objects used for the HUD
		std::vector<glm::mat3> m_vCameras; //Holds all of the camera positions
		std::vector<std::vector<bool>> m_vbUsePlayerPos; //Holds which camera coordinates will follow the player
		int m_iCurrentCamera; //Holds which the index for the camera being used
		int m_iCollectableCount; //Holds how many collectables have been collected
		glm::mat3 tempCamera; //Holds a temporary camera for building 
		UIText* textHolder; //Holds the text to be drawn
		bool m_bSceneOver; //States whether the scene should close
		sf::Clock m_Clock; //Used for timing events
		bool m_bNewCollectable; //States whether a new collectable has been acquired

	public:
		Scene(UIText* TextHolder); //Default constructor
		void update(); //Updates every object in the scene
		void load(std::string sDir); //Loads everything in the scene
		void read(tinyxml2::XMLNode* currentChild); //Reads the next line of an xml file
		void moveRobot(float fDirection); //Moves the robot in the given direction
		void turnRobot(float fDirection); //Turns the robot in the given direction
		void switchCamera(int iDirection); //Switches to the next camera in the given direction
		void setRobotMoving(bool bValue); //Sets whether the robot is moving or not
		bool getSceneOver(); //Returns whether the scene should close
		~Scene();  //Deconstructor
};