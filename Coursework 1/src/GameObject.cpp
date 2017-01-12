#include "GameObject.h"

GameObject::GameObject()
{

}

GameObject::GameObject(GLint programHandle, std::vector<Model*> NewModels)
{

}

void GameObject::update()
{
	for (int i = 0; i < m_vParts.size(); i++) //For every model in the scene
	{
		//Transform and render the model
		m_vParts.at(i)->start();
			m_vParts.at(i)->scale(m_vParts.at(i)->getScale().x, m_vParts.at(i)->getScale().y, m_vParts.at(i)->getScale().z);
			m_vParts.at(i)->translate(0, m_vParts.at(i)->getDimensions().y / 2, 0);
			m_vParts.at(i)->rotate(m_vParts.at(i)->getRotation().x, m_vParts.at(i)->getRotation().y, m_vParts.at(i)->getRotation().z, WORLD_COORDS);
			m_vParts.at(i)->translate(m_vParts.at(i)->getPosition().x, m_vParts.at(i)->getPosition().y, m_vParts.at(i)->getPosition().z);
			
			m_vParts.at(i)->scale(Scale.x, Scale.y, Scale.z);
			m_vParts.at(i)->rotate(Rotation.x, Rotation.y, Rotation.z, WORLD_COORDS);
			m_vParts.at(i)->translate(Position.x, Position.y, Position.z);
	}
}

void GameObject::render()
{
	for (int i = 0; i < m_vParts.size(); i++) //For every model in the scene
	{
		m_vParts.at(i)->render();
	}
}

void GameObject::setRotation(glm::vec3 newRotation)
{
	Rotation = newRotation;
}

void GameObject::setPosition(glm::vec3 newPosition)
{
	Position = newPosition;
}

void GameObject::setScale(glm::vec3 newScale)
{
	Scale = newScale;
}

glm::vec3 GameObject::getRotation()
{
	return Rotation;
}

glm::vec3 GameObject::getPosition()
{
	return Position;
}

glm::vec3 GameObject::getScale()
{
	return Scale;
}

GameObject::~GameObject()
{

}

