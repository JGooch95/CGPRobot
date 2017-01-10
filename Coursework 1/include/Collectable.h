#pragma once
#include "model.h"

class Collectable
{
	private: 
		Model* Model1;
		int Rot;
		glm::vec3 Position;
		float BounceSpeed;

	public:
		Collectable(Model* NewModel);
		void Move();
		int Colliding(glm::vec3 Position);
		bool Collected;
};