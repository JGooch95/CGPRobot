#pragma once
#include "model.h"
#include "Robot.h"
#include "Collectable.h"
#include "GameObject.h"
class Scene
{
	private:
		std::vector<GameObject*> m_vObjects;
		std::vector<Collectable*> m_vCollectables;
		std::vector<Texture*> m_vTextures;
		std::vector<Robot*> m_vRobots;
		GLuint m_uiProgramHandle;
		std::vector<glm::mat3> m_vCameras;
		glm::mat4 m_PerspMatrix;
		std::vector<std::vector<bool>> m_vbUsePlayerPos;
		int m_iCurrentCamera;
		int m_iCollectableCount;

	public:
		Scene();
		void init();
		void update();
		void load(std::string dir);
		void linkMe(GLint vertShader, GLint fragShader);
		void moveRobot(float Direction);
		void turnRobot(float Direction);
		void switchCamera(int Direction);
};