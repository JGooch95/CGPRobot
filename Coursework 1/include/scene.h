#pragma once
#include "model.h"
#include "Robot.h"
#include "Collectable.h"
#include "GameObject.h"
#include <tinyxml2.h>
#include <Array>
#include "HUD.h"


class Scene
{
	private:
		GLuint m_uiProgramHandle; //Holds the current programHandle

		std::vector<GameObject*> m_vObjects; //Holds pointers to every object
		std::vector<Robot*> m_vRobots; //Holds pointers to every robot
		std::vector<Collectable*> m_vCollectables; //Holds pointers to every collectable
		std::vector<GameObject*> m_vHUD;
		std::vector<glm::mat3> m_vCameras; //Holds all of the camera positions
		std::vector<std::vector<bool>> m_vbUsePlayerPos; //Holds which camera coordinates will follow the player

		int m_iCurrentCamera; //Holds which the index for the camera being used
		int m_iCameraCount; //Holds the amount of cameras in the scene
		int m_iCollectableCount; //Holds how many collectables have been collected
		int m_iCollectableAmount; //Holds the amount of collectables in the scene

		glm::mat3 tempCamera; //Holds a temporary camera for building 

	public:
		Scene(); //Default constructor
		void init(); //Initializes the shaders
		void update(); //Updates every object in the scene
		void load(std::string sDir); //Loads everything in the scene
		void read(tinyxml2::XMLNode* currentChild); //Reads the next line of an xml file
		void linkMe(GLint vertShader, GLint fragShader); //Links the shaders
		void moveRobot(float fDirection); //Moves the robot in the given direction
		void turnRobot(float fDirection); //Turns the robot in the given direction
		void switchCamera(int iDirection); //Switches to the next camera in the given direction
		void configureLights(); //Configures the lighting
		void configureHUDLights(); //Configures the lighting
		void setRobotMoving(bool bValue); //Sets whether the robot is moving or not
		~Scene();  //Deconstructor
};