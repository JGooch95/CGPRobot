#pragma once
#include "model.h"

class Collectable
{
	private: 
		Model* model1;
		int Rot;
		glm::vec3 Position;
		float BounceSpeed;

	public:
		Collectable(Model* NewModel);
		void Move();
		bool Colliding(glm::vec3 Position);
		bool Collected;
};