#include "Collectable.h"
#include <iostream>
#include <fstream>

Collectable::Collectable(Model* newModel)
{
	model1 = newModel;
	Rot = 0;
	BounceSpeed = 0.001f;
};

void Collectable::Move()
{
	Position.y += BounceSpeed;

	if (Position.y > 0.5f * model1->getCurrentScale().y || Position.y < 0.0f)
	{
		BounceSpeed *= -1.0f;
	}

	Rot += 1.0f;
	if (Rot >= 360.0f)
	{
		Rot = 0.0f;
	}
	else if (Rot < 0.0f)
	{
		Rot = 359.0f;
	}

	model1->start();
		model1->scale(model1->getCurrentScale().x, model1->getCurrentScale().y, model1->getCurrentScale().z);
		model1->translate(0.0f, model1->getCurrentScale().y / 2.0f, 0.0f);
		model1->rotate(0.0f, Rot, 0.0f, LOCAL_COORDS);
		model1->translate(model1->getPosition().x, model1->getPosition().y, model1->getPosition().z);
		model1->rotate(model1->getRotation().x, model1->getRotation().y, model1->getRotation().z, WORLD_COORDS);
		model1->translate(0.0f,Position.y,0.0f);
	model1->end();
	model1->render();
}

bool Collectable::Colliding(glm::vec3 Position)
{
	//Calculates the vector between the models
	glm::vec3 dist = Position - model1->getPosition();

	//Calculates the magnitude between them
	float fMag = sqrtf(pow(dist.x, 2) + pow(dist.z, 2));

	//If the magnitude is less than the collision range
	if (fMag < 0.1f)
	{
		Collected = true; //The collectible has been collected
		return true; 
	}
	return false;
}