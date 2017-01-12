#pragma once
#include "model.h"

class GameObject
{
	protected:
		glm::vec3 Rotation;
		glm::vec3 Position;
		glm::vec3 Scale;
	public:
		GameObject();
		GameObject(GLint programHandle, std::vector<Model*> NewModels);
		virtual void update();
		virtual void render();
		std::vector<Model*> m_vParts;

		void setRotation(glm::vec3 newRotation);
		void setPosition(glm::vec3 newPosition);
		void setScale(glm::vec3 newScale);

		glm::vec3 getRotation();
		glm::vec3 getPosition();
		glm::vec3 getScale();
		~GameObject();
};