#pragma once
#include "model.h"
#include "GameObject.h"
class Robot : public GameObject
{
	private:
		std::vector<float> m_vfLimbAngles; //Holds the angles of the limbs
		std::vector<float> m_vfLimbRotSpeeds; //Holds the rotation speeds of the limbs

	public:
		Robot(); //Default constructor
		void animate(); //Animates the arms and legs
		void update(); //Updates the model matrices for the parts
		void Move(float Direction); //Moves the robot in the direction given
		void Turn(float Direction); //Turns the robot in the direction given
};