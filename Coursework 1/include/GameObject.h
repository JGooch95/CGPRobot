#pragma once
#include "model.h"

class GameObject
{
	protected:
		glm::vec3 m_Rotation; //The overall rotation of the object
		glm::vec3 m_Position; //The overall position of the object
		glm::vec3 m_Scale; //The overall scale of the object
		GLuint m_uiProgramHandle;

	public:
		GameObject(); //Default Constructor
		GameObject(GLuint uiProgramHandle2);
		virtual void update(); //Updates the transformations
		virtual void render(); //Renders the parts of the model
		virtual void animate(); //Changes the data for animating

		void setRotation(glm::vec3 newRotation); //Sets the overall rotation of the model
		void setPosition(glm::vec3 newPosition); //Sets the overall position of the model
		void setScale(glm::vec3 newScale); //Sets the overall scale of the model

		glm::vec3 getRotation(); //Gets the overall rotation of the model
		glm::vec3 getPosition(); //Gets the overall position of the model
		glm::vec3 getScale(); //Gets the overall scale of the model
		~GameObject(); //Destructor

		std::vector<Model*> m_vParts; //Holds the models for the object
};