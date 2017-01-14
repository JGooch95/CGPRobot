#include <Robot.h>
#include <iostream>
#include <fstream>

Robot::Robot()
{
	//sets Limb Angles
	//0 & 1 Arms, 2 & 3 Legs
	for (int i = 0; i < 4; i++)
	{
		m_vfLimbAngles.push_back(0.0f);
	}

	//Arm Rotation Speeds
	m_vfLimbRotSpeeds.push_back(-1.0f);
	m_vfLimbRotSpeeds.push_back(1.0f);

	//Leg Rotation Speeds
	m_vfLimbRotSpeeds.push_back(1.0f);
	m_vfLimbRotSpeeds.push_back(-1.0f);
}

void Robot::update()
{
	//Head & Body
	for (int i = 0; i < 2; i++)
	{
		m_vParts.at(i)->start(); //Resets the model matrix
		m_vParts.at(i)->scale(m_vParts.at(i)->getScale());
		m_vParts.at(i)->translate(m_vParts.at(i)->getPosition());
	}

	//Arms and Legs
	for (int i = 0; i < 4; i++)
	{
		m_vParts.at(2 + i)->start(); //Resets the model matrix
		m_vParts.at(2 + i)->scale(m_vParts.at(2 + i)->getScale());

		//Rotate the limb around the joint
		m_vParts.at(2 + i)->translate(glm::vec3(0, -(m_vParts.at(2 + i)->getDimensions().y * m_vParts.at(2 + i)->getScale().y) / 2.0f, 0));
		m_vParts.at(2 + i)->rotate(glm::vec3(m_vfLimbAngles.at(i), 0, 0), LOCAL_COORDS);
		m_vParts.at(2 + i)->translate(glm::vec3(0, (m_vParts.at(2 + i)->getDimensions().y * m_vParts.at(2 + i)->getScale().y) / 2.0f, 0));

		//Moves the part to the joint
		m_vParts.at(2 + i)->translate(m_vParts.at(2 + i)->getPosition());
	}

	//All parts
	for (int i = 0; i < m_vParts.size(); i++)
	{
		m_vParts.at(i)->scale(m_Scale);
		m_vParts.at(i)->rotate(glm::vec3(m_Rotation), LOCAL_COORDS);
		m_vParts.at(i)->translate(m_Position);
	}
}

void Robot::animate()
{
	//0 & 1 Arms, 2 & 3 Legs
	for (int i = 0; i < 4; i++) //For every limb
	{
		m_vfLimbAngles.at(i) += m_vfLimbRotSpeeds.at(i); //Increment it's angle
		if (abs(m_vfLimbAngles.at(i)) > 45.0f) //If the angle goes over the boundary
		{
			m_vfLimbRotSpeeds.at(i) *= -1.0f;	//Flip the direction of the animation

			//Move the angle back to the boundary
			if (m_vfLimbAngles.at(i) > 45.0f) //Upper bound
			{
				m_vfLimbAngles.at(i) = 45.0f;
			}
			else if (m_vfLimbAngles.at(i) < -45.0f) //Lower bound
			{
				m_vfLimbAngles.at(i) = -45.0f;
			}
		}
	}
	
}

void Robot::move(float fDirection)
{
	float fSpeed = 2.0f; //The default speed of the movement

	float fTemp = (cosf(glm::radians(m_Rotation.y - 45.0f))) + (sinf(glm::radians(m_Rotation.y - 45.0f))); //Calculates the x of the unit direction vector
	m_Position.x += -fDirection * fSpeed * fTemp; //Increments position

	fTemp = (-sin(glm::radians(m_Rotation.y - 45.0f))) + (cos(glm::radians(m_Rotation.y - 45.0f))); //Calculates the y of the unit direction vector
	m_Position.z += -fDirection * fSpeed * fTemp; //Increments position
}

void Robot::turn(float fDirection)
{
	float fSpeed = 90.0f; //The default speed of the rotation
	m_Rotation.y += fDirection * fSpeed; //Increments rotation
}