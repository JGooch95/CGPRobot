#pragma once
#include "model.h"
#include "GameObject.h"

class Collectable : public GameObject
{
	private: 
		float BounceSpeed;

	public:
		Collectable();
		Collectable(Model* NewModel);
		void update();
		bool Colliding(glm::vec3 newPosition);
		void render();
		bool Collected;

};