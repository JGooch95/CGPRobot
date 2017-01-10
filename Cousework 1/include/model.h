#pragma once

#include "gl_core_4_3.hpp"
#include <fstream>	
#include <sstream>
#include <string>
#include <vector>
#include "glm\glm.hpp"
#include "Texture.h"

class Model
{
	private:
		GLuint vboHandles[2];
		GLuint vaoHandle;
		GLint programHandle;

		Texture* gTexture;

		glm::mat4 M;

		std::string TextureDir;
		std::vector<float> positionData;
		std::vector<float> uvData;

	public:
		Model(std::string Obj, glm::vec3 Translate2, glm::vec3 Rotate2, glm::vec3 Scale2, std::string Tex, GLint programHandle2);
		void init();
		void update();
		void render();
		void loadObj(std::string dir);

		void end();
		void start();
		void Scale(float x, float y, float z);
		void WRotate(float x, float y, float z);
		void LRotate(float x, float y, float z);
		void Translate(float x, float y, float z);
		void LTranslate(float x, float y, float z);
		glm::vec3 Rotation;
		glm::vec3 CurrentScale;
		glm::vec3 Position;
		glm::vec3 Dimensions;
		
};