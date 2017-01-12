#pragma once

#include "gl_core_4_3.hpp"
#include <fstream>	
#include <sstream>
#include <string>
#include <vector>
#include "glm\glm.hpp"
#include "Texture.h"

enum CoordinateType{ LOCAL_COORDS, WORLD_COORDS };

class Model
{
	private:
		GLuint m_uiVBOHandles[2];
		GLuint m_uiVAOHandle;
		GLint m_iProgramHandle;

		Texture* m_Texture;

		glm::mat4 m_ModelMatrix;

		std::vector<float> m_vfPositionData;
		std::vector<float> m_vfUvData;
		
		glm::vec3 m_Rotation;
		glm::vec3 m_Position;
		glm::vec3 m_Scale;
		glm::vec3 m_Dimensions;

public:
		Model(GLint programHandle2);
		Model(std::string Obj, glm::vec3 Translate2, glm::vec3 Rotate2, glm::vec3 Scale2, GLint programHandle2);
		void init();
		void render();
		void loadObj(std::string sDir);

		void start();

		void scale(float fX, float fY, float fZ);
		void rotate(float fX, float fY, float fZ, CoordinateType Coord);
		void translate(float fX, float fY, float fZ);

		glm::vec3 getRotation();
		glm::vec3 getPosition();
		glm::vec3 getScale();
		glm::vec3 getDimensions();

		void setPosition(glm::vec3 newVect);
		void setRotation(glm::vec3 newVect);
		void setScale(glm::vec3 newVect);
		void setTexture(std::string newTexture);
		
};