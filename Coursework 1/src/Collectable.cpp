#include "Collectable.h"
#include <iostream>
#include <fstream>

Collectable::Collectable(Model* NewModel)
{
	Model1 = NewModel;
	Rot = 0;
	BounceSpeed = 0.001;
};

void Collectable::Move()
{
	Position.y += BounceSpeed;

	if (Position.y > 0.05 || Position.y < 0)
	{
		BounceSpeed *= -1;
	}
	Rot += 1;
	if (Rot >= 360)
	{
		Rot = 0;
	}
	if (Rot < 0)
	{
		Rot = 359;
	}

	Model1->start();
	Model1->Scale(Model1->CurrentScale.x, Model1->CurrentScale.y, Model1->CurrentScale.z);
	Model1->Translate(0, Model1->CurrentScale.y / 2, 0);
	Model1->LRotate(0, Rot, 0);
	Model1->Translate(Model1->Position.x, Model1->Position.y, Model1->Position.z);
	Model1->WRotate(Model1->Rotation.x, Model1->Rotation.y, Model1->Rotation.z);
	Model1->Translate(0,Position.y,0);
	Model1->end();
	Model1->render();
}

int Collectable::Colliding(glm::vec3 Position)
{
	glm::vec3 Dist = Position - Model1->Position;

	float d = sqrtf(pow(Dist.x, 2) + pow(Dist.y, 2) + pow(Dist.z, 2));

	if (d < 0.1 && d > -0.1)
	{
		Collected = true;
		return 1;
	}
	return 0;
}