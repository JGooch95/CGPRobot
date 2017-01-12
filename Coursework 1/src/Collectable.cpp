#include "Collectable.h"
#include <iostream>
#include <fstream>

Collectable::Collectable()
{
	BounceSpeed = 0.001f;
}
Collectable::Collectable(Model* newModel)
{
	BounceSpeed = 0.001f;
};

void Collectable::update()
{
	if (!Collected)
	{
		Position.y += BounceSpeed;

		if (Position.y > 0.5f * m_vParts.at(0)->getScale().y || Position.y < 0.0f)
		{
			BounceSpeed *= -1.0f;
		}

		Rotation.y += 1.0f;
		if (Rotation.y >= 360.0f)
		{
			Rotation.y = 0.0f;
		}
		else if (Rotation.y < 0.0f)
		{
			Rotation.y = 359.0f;
		}

		m_vParts.at(0)->start();
		m_vParts.at(0)->scale(m_vParts.at(0)->getScale().x, m_vParts.at(0)->getScale().y, m_vParts.at(0)->getScale().z);
		m_vParts.at(0)->translate(0.0f, m_vParts.at(0)->getScale().y / 2.0f, 0.0f);
		m_vParts.at(0)->rotate(0.0f, Rotation.y, 0.0f, LOCAL_COORDS);
		m_vParts.at(0)->translate(m_vParts.at(0)->getPosition().x, m_vParts.at(0)->getPosition().y, m_vParts.at(0)->getPosition().z);
		m_vParts.at(0)->rotate(m_vParts.at(0)->getRotation().x, m_vParts.at(0)->getRotation().y, m_vParts.at(0)->getRotation().z, WORLD_COORDS);
		m_vParts.at(0)->translate(0.0f, Position.y, 0.0f);

		m_vParts.at(0)->scale(Scale.x, Scale.y, Scale.z);
		m_vParts.at(0)->rotate(Rotation.x, Rotation.y, Rotation.z, WORLD_COORDS);
		m_vParts.at(0)->translate(Position.x, Position.y, Position.z);
	}
}

void Collectable::render()
{
	if (!Collected)
	{
		for (int i = 0; i < m_vParts.size(); i++) //For every model in the scene
		{
			m_vParts.at(i)->render();
		}
	}
}

bool Collectable::Colliding(glm::vec3 newPosition)
{
	//Calculates the vector between the models
	glm::vec3 dist = newPosition - Position;

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