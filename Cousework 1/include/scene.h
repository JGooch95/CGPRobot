#pragma once
#include "model.h"
#include "Robot.h"
#include "Collectable.h"
class Scene
{
	private:
		std::vector<Model*> Models;
		std::vector<Collectable*> Collectables;
		std::vector<Texture*> gTextures;
		Robot Bot1;
		GLuint programHandle;
		std::vector<glm::mat3> Cameras;
		glm::mat4 P;
		std::vector<std::vector<bool>> usePlayerPos;
		int CurrentCamera;
		int CollectableCount;

	public:
		Scene();
		void Init();
		void Update();
		void Load(std::string dir);
		void linkMe(GLint vertShader, GLint fragShader);
		void MoveRobot(float Direction);
		void TurnRobot(float Direction);
		void SwitchCamera(int Direction);
};