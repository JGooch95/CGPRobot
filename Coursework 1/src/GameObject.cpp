#include "GameObject.h"

GameObject::GameObject()
{
}

GameObject::GameObject(GLuint uiProgramHandle2)
{
	m_uiProgramHandle = uiProgramHandle2;
}
void GameObject::update()
{
	for (int i = 0; i < m_vParts.size(); i++) //For every model in the scene
	{
		//Transform and render the model
		m_vParts.at(i)->start(); //Resets the model to default
			m_vParts.at(i)->scale(m_vParts.at(i)->getScale());  //Scales the part to its size
			m_vParts.at(i)->translate(m_vParts.at(i)->getPosition()); // Moves the part to its position
			m_vParts.at(i)->rotate(m_vParts.at(i)->getRotation(), WORLD_COORDS);//Rotates the part to its rotation
			
			m_vParts.at(i)->scale(m_Scale); //Scales the pdddart to the model scale
			m_vParts.at(i)->rotate(m_Rotation, LOCAL_COORDS); // Rotates the part to the model scale
			m_vParts.at(i)->translate(m_Position); //Translates the part to the model position
		m_vParts.at(i)->end();
	}
}

void GameObject::render()
{
	for (int i = 0; i < m_vParts.size(); i++) //For every part of the object
	{
		m_vParts.at(i)->render(); //Render it
	}
}

void GameObject::animate()
{
}

void GameObject::setRotation(glm::vec3 newRotation)
{
	m_Rotation = newRotation;
}

void GameObject::setPosition(glm::vec3 newPosition)
{
	m_Position = newPosition; 
}

void GameObject::setScale(glm::vec3 newScale)
{
	m_Scale = newScale; 
}

glm::vec3 GameObject::getRotation()
{
	return m_Rotation;
}

glm::vec3 GameObject::getPosition()
{
	return m_Position;
}

glm::vec3 GameObject::getScale()
{
	return m_Scale;
}


GameObject::~GameObject()
{
	for (int i = 0; i < m_vParts.size(); i++) //For every part of the model
	{
		delete(m_vParts.at(i)); //Delete the data at the pointer location
		m_vParts.at(i) = NULL; //Remove the pointer
	}
}

