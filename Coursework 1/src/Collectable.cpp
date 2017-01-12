#include "Collectable.h"
#include <iostream>
#include <fstream>

Collectable::Collectable()
{
	m_fBounceSpeed = 0.001f; //Sets the default bounce speed
}

void Collectable::animate()
{
	if (!m_bCollected) //If the collectable hasn't been picked up
	{
		m_Position.y += m_fBounceSpeed; //Move the collectable by the bounce speed

		//If the boundaries of the bounce are passed
		if (m_Position.y > 0.5f * m_vParts.at(0)->getScale().y || m_Position.y < 0.0f)
		{
			m_fBounceSpeed *= -1.0f; //Flip the direction
		}

		m_Rotation.y += 1.0f; //Increases the rotation

		//Limits the rotation to be between 0 and 360
		if (m_Rotation.y >= 360.0f)
		{
			m_Rotation.y -=360;
		}
		else if (m_Rotation.y < 0.0f)
		{
			m_Rotation.y += 360.0f;
		}
	}
}

void Collectable::render()
{
	if (!m_bCollected) //If the collectable hasn't been picked up
	{
		//Render every part
		for (int i = 0; i < m_vParts.size(); i++)
		{
			m_vParts.at(i)->render(); 
		}
	}
}

bool Collectable::colliding(glm::vec3 newPosition)
{
	//Calculates the vector between the models
	glm::vec3 dist = newPosition - m_Position;

	//Calculates the magnitude between them
	float fMag = sqrtf(pow(dist.x, 2) + pow(dist.z, 2));

	//If the magnitude is less than the collision range
	if (fMag < 0.1f)
	{
		m_bCollected = true; //The collectible has been collected
		return true; 
	}
	return false;
}