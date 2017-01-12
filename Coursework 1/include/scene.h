#pragma once
#include "model.h"
#include "Robot.h"
#include "Collectable.h"
#include "GameObject.h"
class Scene
{
	private:
		GLuint m_uiProgramHandle; //Holds the current programHandle

		std::vector<GameObject*> m_vObjects; //Holds pointers to every object
		std::vector<Robot*> m_vRobots; //Holds pointers to every robot

		std::vector<Collectable*> m_vCollectables; //Holds pointers to every collectable
		int m_iCollectableCount; //Holds how many collectables have been collected

		std::vector<glm::mat3> m_vCameras; //Holds all of the camera positions
		std::vector<std::vector<bool>> m_vbUsePlayerPos; //Holds which camera coordinates will follow the player
		int m_iCurrentCamera; //Holds which the index for the camera being used

		//std::vector<Texture*> m_vTextures;

	public:
		Scene(); //Default constructor
		void init(); //Initializes the shaders
		void update(); //Updates every object in the scene
		void load(std::string dir); //Loads everything in the scene
		void linkMe(GLint vertShader, GLint fragShader); //Links the shaders
		void moveRobot(float Direction); //Moves the robot in the given direction
		void turnRobot(float Direction); //Turns the robot in the given direction
		void switchCamera(int Direction); //Switches to the next camera in the given direction
		~Scene();  //Deconstructor
};