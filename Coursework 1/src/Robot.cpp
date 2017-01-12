#include <Robot.h>
#include <iostream>
#include <fstream>

Robot::Robot()
{

}

Robot::Robot(std::string dir, GLint programHandle2)
{
	
}

void Robot::Setup()
{
	ArmAngles1 = 0;
	LegAngles1 = 0;

	ArmAngles1 = 0;
	LegAngles2 = 0;

	ArmRotSpeed1 = -1;
	ArmRotSpeed2 = 1;
	LegRotSpeed1 = 1;
	LegRotSpeed2 = -1;
	Position.y = 1.3f * Scale.y;
}

void Robot::update()
{
	ArmAngles1 += ArmRotSpeed1;
	ArmAngles2 += ArmRotSpeed2;
	LegAngles1 += LegRotSpeed1;
	LegAngles2 += LegRotSpeed2;

	if (LegAngles1 > 45 || LegAngles1 < -45)
	{
		LegRotSpeed1 *= -1;
		if (LegAngles1 > 45)
		{
			LegAngles1 = 45;
		}
		else if(LegAngles1 < -45)
		{
			LegAngles1 = -45;
		}
		
	}
	if (LegAngles2 > 45 || LegAngles2 < -45)
	{
		LegRotSpeed2 *= -1;
		if (LegAngles2 > 45)
		{
			LegAngles2 = 45;
		}
		else if (LegAngles2 < -45)
		{
			LegAngles2 = -45;
		}
	}
	if (ArmAngles1 > 45 || ArmAngles1 < -45)
	{
		ArmRotSpeed1 *= -1;
		if (ArmAngles1 > 45)
		{
			ArmAngles1 = 45;
		}
		else if (ArmAngles1 < -45)
		{
			ArmAngles1 = -45;
		}
	}
	if (ArmAngles2 > 45 || ArmAngles2 < -45)
	{
		ArmRotSpeed2 *= -1;
		if (ArmAngles2 > 45)
		{
			ArmAngles2 = 45;
		}
		else if (ArmAngles2 < -45)
		{
			ArmAngles2 = -45;
		}
	}

	m_vParts.at(0)->start();
		m_vParts.at(0)->scale(m_vParts.at(0)->getScale().x, m_vParts.at(0)->getScale().y, m_vParts.at(0)->getScale().z);
		m_vParts.at(0)->translate(m_vParts.at(0)->getPosition().x, m_vParts.at(0)->getPosition().y, m_vParts.at(0)->getPosition().z);

	m_vParts.at(1)->start();
		m_vParts.at(1)->scale(m_vParts.at(1)->getScale().x, m_vParts.at(1)->getScale().y, m_vParts.at(1)->getScale().z);
		m_vParts.at(1)->translate(m_vParts.at(1)->getPosition().x, m_vParts.at(1)->getPosition().y, m_vParts.at(1)->getPosition().z);

	for (int i = 0; i < 2; i++)
	{
			m_vParts.at(2 + i)->start();
			m_vParts.at(2 + i)->scale(m_vParts.at(2 + i)->getScale().x, m_vParts.at(2 + i)->getScale().y, m_vParts.at(2 + i)->getScale().z);
			m_vParts.at(2 + i)->translate(0, -m_vParts.at(2 + i)->getScale().y / 2, 0);
			if (i == 0)
			{
				m_vParts.at(2 + i)->rotate(ArmAngles1, 0, 0, LOCAL_COORDS);
			}
			else if (i == 1)
			{
				m_vParts.at(2 + i)->rotate(ArmAngles2, 0, 0, LOCAL_COORDS);
			}
			m_vParts.at(2 + i)->translate(0, m_vParts.at(2 + i)->getScale().y / 2, 0);
			m_vParts.at(2 + i)->translate(m_vParts.at(2 + i)->getPosition().x, m_vParts.at(2 + i)->getPosition().y, m_vParts.at(2 + i)->getPosition().z);
	}

	for (int i = 0; i < 2; i++)
	{
		m_vParts.at(4+i)->start();
			m_vParts.at(4 + i)->scale(m_vParts.at(3 + i)->getScale().x, m_vParts.at(4 + i)->getScale().y, m_vParts.at(4 + i)->getScale().z);
			m_vParts.at(4 + i)->translate(0, -m_vParts.at(4 + i)->getScale().y / 2, 0);
			if (i == 0)
			{
				m_vParts.at(4 + i)->rotate(LegAngles1, 0, 0, LOCAL_COORDS);
			}
			else if (i == 1)
			{
				m_vParts.at(4 + i)->rotate(LegAngles2, 0, 0, LOCAL_COORDS);
			}
			m_vParts.at(4+i)->translate(0, m_vParts.at(4 + i)->getScale().y / 2, 0);
			m_vParts.at(4+i)->translate(m_vParts.at(4 + i)->getPosition().x, m_vParts.at(4 + i)->getPosition().y, m_vParts.at(4 + i)->getPosition().z);
	}

	for (int i = 0; i < m_vParts.size(); i++)
	{
		m_vParts.at(i)->scale(Scale.x, Scale.y, Scale.z);
		m_vParts.at(i)->rotate(0, Rotation.y, 0, LOCAL_COORDS);
		m_vParts.at(i)->translate(Position.x, Position.y, Position.z);
	}

	
}

void Robot::Move(float Direction)
{
	float Speed = 0.02f;
	float temp1 = (cosf(glm::radians(Rotation.y - 45))) + (sinf(glm::radians(Rotation.y - 45)));
	float temp2 = (-sin(glm::radians(Rotation.y - 45))) + (cos(glm::radians(Rotation.y - 45)));

	Position.x += -Direction * Speed * temp1;
	Position.y = 1.3f * Scale.y;
	Position.z += -Direction * Speed * temp2;
}

void Robot::Turn(float Direction)
{
	float Speed = 2.0f;
	Rotation.y += Direction * Speed;
}