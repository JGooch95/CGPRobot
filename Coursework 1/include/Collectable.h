#pragma once
#include "model.h"
#include "GameObject.h"

class Collectable : public GameObject
{
	private: 
		float m_fBounceSpeed; //The speed the collectable bounces

	public:
		Collectable(); //Default constructor
		void animate(); //Animates the collectable
		bool colliding(glm::vec3 newPosition); //Checks for collisions at the position
		void render(); //Renders the object

		bool m_bCollected; //States whether the collectable has been picked up
};