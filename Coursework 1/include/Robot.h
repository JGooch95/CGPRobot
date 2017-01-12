#pragma once
#include "model.h"
#include "GameObject.h"
class Robot : public GameObject
{
	private:
		float ArmRotSpeed1, ArmRotSpeed2;
		float LegRotSpeed1, LegRotSpeed2;
		float ArmAngles1, ArmAngles2;
		float LegAngles1, LegAngles2;

	public:
		Robot();
		Robot(std::string dir, GLint programHandle2);
		void Setup();
		void update();
		void Move(float Direction);
		void Turn(float Direction);
};