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
		GLuint m_uiVBOHandles[3];
		GLuint m_uiVAOHandle;
		GLuint m_uiProgramHandle;

		Texture* m_Texture; // Holds a pointer to the texture being used

		std::vector<float> m_vfPositionData; //Holds the vertex data to be rendered
		std::vector<float> m_vfUvData; //Holds the UV's to map the texture to the vertices
		std::vector<float> m_vfNormalsData; //Holds the normals used for lighting the model

		glm::mat4 m_ModelMatrix; //Holds the matrix to transform the model by

		glm::vec3 m_Rotation; //Holds the default rotation
		glm::vec3 m_Position; //Holds the default position
		glm::vec3 m_Scale;  //Holds the default scale
		glm::vec3 m_Dimensions;  //Holds the dimensions of the model

public:
		Model(GLuint uiProgramHandle2); //Constructor
		void loadObj(std::string sDir); //Loads the obj file and fills the buffers
		void render(); //Renders the model

		void start(); //Resets the model matrix ready for transforms

		void scale(glm::vec3 newScale); //Scales the current model matrix
		void rotate(glm::vec3 newRotation, CoordinateType Coord); //Rotates the current model matrix
		void translate(glm::vec3 newPosition); //Translates the current model matrix

		glm::vec3 getRotation(); //Returns the models default rotation
		glm::vec3 getPosition(); //Returns the models default position
		glm::vec3 getScale();    //Returns the models default scale
		glm::vec3 getDimensions(); //Returns the dimensions of the model

		void setPosition(glm::vec3 newVect); //Sets the models default rotation
		void setRotation(glm::vec3 newVect); //Sets the models default position
		void setScale(glm::vec3 newVect);	 //Sets the models default scale
		void setTexture(std::string newTexture); //Sets the texture of the model
		~Model(); //Deconstructor
		
};