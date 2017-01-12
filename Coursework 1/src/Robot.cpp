#include <Robot.h>
#include <iostream>
#include <fstream>

Robot::Robot()
{
	//Arms Angles
	m_vfLimbAngles.push_back(0);
	m_vfLimbAngles.push_back(0);

	//Leg Angles
	m_vfLimbAngles.push_back(0);
	m_vfLimbAngles.push_back(0);

	//Arm Rotation Speeds
	m_vfLimbRotSpeeds.push_back(-1);
	m_vfLimbRotSpeeds.push_back(1);

	//Leg Rotation Speeds
	m_vfLimbRotSpeeds.push_back(1);
	m_vfLimbRotSpeeds.push_back(-1);
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
		m_vParts.at(2 + i)->translate(glm::vec3(0, -(m_vParts.at(2 + i)->getDimensions().y * m_vParts.at(2 + i)->getScale().y) / 2, 0));
		m_vParts.at(2 + i)->rotate(glm::vec3(m_vfLimbAngles.at(i), 0, 0), LOCAL_COORDS);
		m_vParts.at(2 + i)->translate(glm::vec3(0, (m_vParts.at(2 + i)->getDimensions().y * m_vParts.at(2 + i)->getScale().y) / 2, 0));

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
		if (abs(m_vfLimbAngles.at(i)) > 45) //If the angle goes over the boundary
		{
			m_vfLimbRotSpeeds.at(i) *= -1;	//Flip the direction of the animation

			//Move the angle back to the boundary
			if (m_vfLimbAngles.at(i) > 45)
			{
				m_vfLimbAngles.at(i) = 45;
			}
			else if (m_vfLimbAngles.at(i) < -45)
			{
				m_vfLimbAngles.at(i) = -45;
			}
		}
	}
	
}

void Robot::Move(float Direction)
{
	float fSpeed = 0.1f; //The default speed of the movement

	float fTemp = (cosf(glm::radians(m_Rotation.y - 45))) + (sinf(glm::radians(m_Rotation.y - 45))); //Calculates the x of the unit direction vector
	m_Position.x += -Direction * fSpeed * fTemp; //Increments position

	fTemp = (-sin(glm::radians(m_Rotation.y - 45))) + (cos(glm::radians(m_Rotation.y - 45))); //Calculates the y of the unit direction vector
	m_Position.z += -Direction * fSpeed * fTemp; //Increments position
}

void Robot::Turn(float Direction)
{
	float fSpeed = 2.0f; //The default speed of the rotation
	m_Rotation.y += Direction * fSpeed; //Increments rotation
}