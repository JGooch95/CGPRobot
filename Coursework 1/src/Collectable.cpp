#include "Collectable.h"
#include <iostream>
#include <fstream>

Collectable::Collectable(Model* NewModel)
{
	Model1 = NewModel;
	Rot = 0;
	BounceSpeed = 0.001f;
};

void Collectable::Move()
{
	Position.y += BounceSpeed;

	if (Position.y > 0.05f || Position.y < 0.0f)
	{
		BounceSpeed *= -1.0f;
	}

	Rot += 1.0f;
	if (Rot >= 360.0f)
	{
		Rot = 0.0f;
	}
	if (Rot < 0.0f)
	{
		Rot = 359.0f;
	}

	Model1->start();
		Model1->scale(Model1->getCurrentScale().x, Model1->getCurrentScale().y, Model1->getCurrentScale().z);
		Model1->translate(0.0f, Model1->getCurrentScale().y / 2.0f, 0.0f);
		Model1->rotate(0.0f, Rot, 0.0f, LOCAL_COORDS);
		Model1->translate(Model1->getPosition().x, Model1->getPosition().y, Model1->getPosition().z);
		Model1->rotate(Model1->getRotation().x, Model1->getRotation().y, Model1->getRotation().z, WORLD_COORDS);
		Model1->translate(0.0f,Position.y,0.0f);
	Model1->end();
	Model1->render();
}

int Collectable::Colliding(glm::vec3 Position)
{
	glm::vec3 Dist = Position - Model1->getPosition();

	float d = sqrtf(pow(Dist.x, 2) + pow(Dist.y, 2) + pow(Dist.z, 2));

	if (d < 0.1 && d > -0.1)
	{
		Collected = true;
		return 1;
	}
	return 0;
}