#pragma once
#include "model.h"
class Robot
{
	private:
		std::vector<Model*> Arms;
		std::vector<Model*> Legs;
		Model* Body;
		Model* Head;

		float ArmRotSpeed1, ArmRotSpeed2;
		float LegRotSpeed1, LegRotSpeed2;
		float ArmAngles1, ArmAngles2;
		float LegAngles1, LegAngles2;

		float Rot;
		glm::vec3 overallScale;

	public:
		Robot();
		Robot(std::string dir, GLint programHandle2);
		void Setup(std::string dir, GLint programHandle2);
		void Render();
		void Move(float Direction);
		void Turn(float Direction);
		glm::vec3 Position;
};