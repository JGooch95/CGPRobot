#include "Collectable.h"
#include <iostream>
#include <fstream>

Collectable::Collectable()
{
	m_fBounceSpeed = 0.01f; //Sets the default bounce speed
	m_fBouncePos = 0.0f; //Sets the default bounce position
}

void Collectable::animate()
{
	if (!m_bCollected) //If the collectable hasn't been picked up
	{
		 m_fBouncePos += m_fBounceSpeed; //Move the collectable by the bounce speed

		//If the boundaries of the bounce are passed
		if (m_fBouncePos > 0.75f || m_fBouncePos < 0.0f)
		{
			m_fBounceSpeed *= -1.0f; //Flip the direction
		}

		m_Position.y += m_fBounceSpeed; //Adds the bounce speed to the position

		m_Rotation.y += 1.0f; //Increases the rotation angle for the spin

		//Limits the rotation to be between 0 and 360
		if (m_Rotation.y >= 360.0f) //Upper bound
		{
			m_Rotation.y -=360.0f;
		}
		else if (m_Rotation.y < 0.0f) //Lower bound
		{
			m_Rotation.y += 360.0f;
		}
	}
}

void Collectable::render()
{
	if (!m_bCollected) //If the collectable hasn't been picked up
	{
		//Render every part of the model
		for (int i = 0; i < m_vParts.size(); i++)
		{
			m_vParts.at(i)->render(); 
		}
	}
}

bool Collectable::colliding(glm::vec3 Position2)
{
	//Calculates the vector between the objects
	glm::vec3 dist = Position2 - m_Position;

	//Calculates the magnitude between them
	float fMag = sqrtf(pow(dist.x, 2.0f) + pow(dist.z, 2.0f));

	//If the magnitude is less than the collision range
	if (fMag < 1.0f)
	{
		m_bCollected = true; //The collectible has been collected
		return true;
	}
	return false;
}