#include <Robot.h>
#include <iostream>
#include <fstream>

Robot::Robot()
{

}

Robot::Robot(std::string dir, GLint programHandle2)
{
	Setup(dir, programHandle2);
}

void Robot::Setup(std::string dir, GLint programHandle2)
{
	Arms.resize(2);
	Legs.resize(2);

	ArmAngles1 = 0;
	LegAngles1 = 0;

	ArmAngles1 = 0;
	LegAngles2 = 0;

	ArmRotSpeed1 = -1;
	ArmRotSpeed2 = 1;
	LegRotSpeed1 = 1;
	LegRotSpeed2 = -1;
	Rot = 0;

	std::fstream Robotfile(dir, std::ios_base::in); //Attempt to open the file given

	if (!Robotfile.is_open()) //If the file didnt open
	{
		std::cerr << "File " << dir << " not found."; //Output an error that the file didnt open
		DebugBreak(); //Input a breakpoint.
		throw std::invalid_argument("File not found"); //Throw a file not found error
		return;	//ERROR!!!
	}

	std::string line; //Holds the line being read
	std::string token = ""; //Holds the part of the string that was last read
	glm::vec3 TempTranslationVect; //Holds the vector to rotate, translate or scale by
	glm::vec3 TempRotationVect; //Holds the vector to rotate, translate or scale by
	glm::vec3 TempScaleVect; //Holds the vector to rotate, translate or scale by
	std::vector<std::string> TextureDirs;
	bool TextureFound = false;
	std::string TextureDirectory;
	std::string ObjDirectory;
	int pos = 0;
	bool LoadingModel = false; //States whether a model is being loaded

	enum Parts { arm1, arm2, body, leg1, leg2, head };
	Parts P;

	//While there are lines to be read
	while (getline(Robotfile, line))
	{
		std::istringstream iss(line); //Holds the data that is read
		token = "";

		//While the token is not the "push" token
		while (token != "p") //(p = Push onto vector)
		{
			iss >> token; // read to first whitespace

			if (token == "o") //If its an OBJ directory
			{
				iss >> token; //Read the item
				ObjDirectory = token; //Load the model
				LoadingModel = true;
			}
			if (token == "t") //If its a translation transform
			{
				iss >> TempTranslationVect.x; //Read the x value
				iss >> TempTranslationVect.y; //Read the y value
				iss >> TempTranslationVect.z; //Read the z value
			}
			else if (token == "s") //If its a scale transform
			{
				iss >> TempScaleVect.x; //Read the x value
				iss >> TempScaleVect.y; //Read the y value
				iss >> TempScaleVect.z; //Read the z value
			}
			else if (token == "r") //If its a rotation transform
			{
				iss >> TempRotationVect.x; //Read the x value
				iss >> TempRotationVect.y; //Read the y value
				iss >> TempRotationVect.z; //Read the z value
			}
			else if (token == "tex") //If its a texture
			{
				iss >> token; //Read the item
				TextureDirectory = token;
			}

			else if (token == "prt") //If its an OBJ directory
			{
				iss >> token;
				if (token == "hd")
				{
					P = head;
				}
				else if (token == "bdy")
				{
					P = body;
				}
				else if (token == "l1")
				{
					P = leg1;
				}
				else if (token == "l2")
				{
					P = leg2;
				}
				else if (token == "a1")
				{
					P = arm1;
				}
				else if (token == "a2")
				{
					P = arm2;
				}
			}

			else if (token == "os")
			{
				iss >> overallScale.x;
				iss >> overallScale.y;
				iss >> overallScale.z;
			}

		}
		if (LoadingModel)
		{
			switch (P)
			{
			case head:
				Head = new Model(ObjDirectory, TempTranslationVect, TempRotationVect, TempScaleVect, TextureDirectory, programHandle2);
				break;
			case body:
				Body = new Model(ObjDirectory, TempTranslationVect, TempRotationVect, TempScaleVect, TextureDirectory, programHandle2);
				break;
			case leg1:
				Legs.at(0) = new Model(ObjDirectory, TempTranslationVect, TempRotationVect, TempScaleVect, TextureDirectory, programHandle2);
				break;
			case leg2:
				Legs.at(1) = new Model(ObjDirectory, TempTranslationVect, TempRotationVect, TempScaleVect, TextureDirectory, programHandle2);
				break;
			case arm1:
				Arms.at(0) = new Model(ObjDirectory, TempTranslationVect, TempRotationVect, TempScaleVect, TextureDirectory, programHandle2);
				break;
			case arm2:
				Arms.at(1) = new Model(ObjDirectory, TempTranslationVect, TempRotationVect, TempScaleVect, TextureDirectory, programHandle2);
				break;
			}
			LoadingModel = false;
		}
	}
	Robotfile.close(); //Closes the file

	Head->setPosition(Head->getPosition() * overallScale);
	Head->setCurrentScale(Head->getCurrentScale() * overallScale);
	Body->setPosition(Body->getPosition() * overallScale);
	Body->setCurrentScale(Body->getCurrentScale() * overallScale);
	for (int i = 0; i < Legs.size(); i++)
	{
		Legs.at(i)->setPosition(Legs.at(i)->getPosition() * overallScale);
		Legs.at(i)->setCurrentScale(Legs.at(i)->getCurrentScale() * overallScale);
	}
	for (int i = 0; i < Arms.size(); i++)
	{
		Arms.at(i)->setPosition(Arms.at(i)->getPosition() * overallScale);
		Arms.at(i)->setCurrentScale(Arms.at(i)->getCurrentScale() * overallScale);
	}
	Position = glm::vec3(9, 1.3f * overallScale.y, 10);
}

void Robot::Render()
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

	Head->start();
		Head->scale(Head->getCurrentScale().x, Head->getCurrentScale().y, Head->getCurrentScale().z);
		Head->translate(Head->getPosition().x, Head->getPosition().y, Head->getPosition().z);
		Head->rotate(0, Rot, 0, WORLD_COORDS);
		Head->translate(Position.x, Position.y, Position.z);
	Head->end();
	Head->render();

	Body->start();
		Body->scale(Body->getCurrentScale().x, Body->getCurrentScale().y, Body->getCurrentScale().z);
		Body->translate(Body->getPosition().x, Body->getPosition().y, Body->getPosition().z);
		Body->rotate(0, Rot, 0, WORLD_COORDS);
		Body->translate(Position.x, Position.y, Position.z);
	Body->end();
	Body->render();

	for (int i = 0; i < Legs.size(); i++)
	{
		Legs.at(i)->start();
			Legs.at(i)->scale(Legs.at(i)->getCurrentScale().x, Legs.at(i)->getCurrentScale().y, Legs.at(i)->getCurrentScale().z);
			Legs.at(i)->translate(0, -Legs.at(i)->getCurrentScale().y / 2, 0);
			if (i == 0)
			{
				Legs.at(i)->rotate(LegAngles1, 0, 0, LOCAL_COORDS);
			}
			else if (i == 1)
			{
				Legs.at(i)->rotate(LegAngles2, 0, 0, LOCAL_COORDS);
			}
			Legs.at(i)->translate(0, Legs.at(i)->getCurrentScale().y / 2, 0);
			Legs.at(i)->translate(Legs.at(i)->getPosition().x, Legs.at(i)->getPosition().y, Legs.at(i)->getPosition().z);
			Legs.at(i)->rotate(0, Rot, 0, LOCAL_COORDS);
			Legs.at(i)->translate(Position.x, Position.y, Position.z);

		Legs.at(i)->end();
		Legs.at(i)->render();
	}

	for (int i = 0; i < Arms.size(); i++)
	{
		Arms.at(i)->start();
			Arms.at(i)->scale(Arms.at(i)->getCurrentScale().x, Arms.at(i)->getCurrentScale().y, Arms.at(i)->getCurrentScale().z);
			Arms.at(i)->translate(0, -Arms.at(i)->getCurrentScale().y/2, 0);
			if (i == 0)
			{
				Arms.at(i)->rotate(ArmAngles1, 0, 0, LOCAL_COORDS);
			}
			else if (i == 1)
			{
				Arms.at(i)->rotate(ArmAngles2, 0, 0, LOCAL_COORDS);
			}
			Arms.at(i)->translate(0, Arms.at(i)->getCurrentScale().y / 2, 0);
			Arms.at(i)->translate(Arms.at(i)->getPosition().x, Arms.at(i)->getPosition().y, Arms.at(i)->getPosition().z);
			Arms.at(i)->rotate(0, Rot, 0, LOCAL_COORDS);
			Arms.at(i)->translate(Position.x, Position.y, Position.z);

		Arms.at(i)->end();
		Arms.at(i)->render();
	}
	
}

void Robot::Move(float Direction)
{
	float Speed = 0.02f;
	float temp1 = (cosf(glm::radians(Rot - 45))) + (sinf(glm::radians(Rot - 45)));
	float temp2 = (-sin(glm::radians(Rot - 45))) + (cos(glm::radians(Rot - 45)));

	Position.x += -Direction * Speed * temp1;
	Position.y = 1.3f * overallScale.y;
	Position.z += -Direction * Speed * temp2;
}

void Robot::Turn(float Direction)
{
	float Speed = 2.0f;
	Rot += Direction * Speed;
}