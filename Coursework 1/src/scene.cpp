#include "scene.h"
#include <fstream>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Scene::Scene(UIText* TextHolder2)
{
	//Loads the diffuse shader
	m_vShaders.push_back(new GLSLProgram);
	m_vShaders.at(0)->compileShader("Assets/shader/diffuse.vert");
	m_vShaders.at(0)->compileShader("Assets/shader/diffuse.frag");
	m_vShaders.at(0)->link();
	m_vShaders.at(0)->validate();
	m_vShaders.at(0)->use();

	//Loads the text shader
	m_vShaders.push_back(new GLSLProgram);
	m_vShaders.at(1)->compileShader("Assets/shader/text.vert");
	m_vShaders.at(1)->compileShader("Assets/shader/text.frag");
	m_vShaders.at(1)->link();
	m_vShaders.at(1)->validate();
	m_vShaders.at(1)->use();

	//Loads the 2D shader
	m_vShaders.push_back(new GLSLProgram);
	m_vShaders.at(2)->compileShader("Assets/shader/2D.vert");
	m_vShaders.at(2)->compileShader("Assets/shader/2D.frag");
	m_vShaders.at(2)->link();
	m_vShaders.at(2)->validate();
	m_vShaders.at(2)->use();

	m_iCurrentCamera = 0; //Sets the current camera to the first camera
	m_iCollectableCount = 0; //Sets the collectable count to 0
	textHolder = TextHolder2;
	m_bSceneOver = false;
	m_bNewCollectable = false;
	
	gl::Enable(gl::BLEND);
	gl::Enable(gl::CULL_FACE);
	gl::BlendFunc(gl::SRC_ALPHA, gl::ONE_MINUS_SRC_ALPHA);
}

void Scene::update()
{
	//Clear the buffers ready for drawing
	gl::Clear(gl::COLOR_BUFFER_BIT | gl::DEPTH_BUFFER_BIT);

	glm::mat4 ViewMatrix = glm::mat4(1.0f);
	if (!m_vCameras.empty()) 
	{
		//Check every coordinate in the camera to see if it's locked to the player
		for (int i = 0; i < m_vbUsePlayerPos.at(m_iCurrentCamera).size(); i++)
		{
			//If it is set it's variable to the according player position
			if (m_vbUsePlayerPos.at(m_iCurrentCamera).at(i))
			{
				//ViewX
				if (i == 0)
				{
					m_vCameras.at(m_iCurrentCamera)[0].x = m_vRobots.at(0)->getPosition().x;
				}
				//ViewY
				else if (i == 1)
				{
					m_vCameras.at(m_iCurrentCamera)[0].y = m_vRobots.at(0)->getPosition().y;
				}
				//ViewZ
				else if (i == 2)
				{
					m_vCameras.at(m_iCurrentCamera)[0].z = m_vRobots.at(0)->getPosition().z;
				}

				//PositionX
				else if (i == 3)
				{
					m_vCameras.at(m_iCurrentCamera)[1].x = m_vRobots.at(0)->getPosition().x;
				}
				//PositionY
				else if (i == 4)
				{
					m_vCameras.at(m_iCurrentCamera)[1].y = m_vRobots.at(0)->getPosition().y;
				}
				//PositionZ
				else if (i == 5)
				{
					m_vCameras.at(m_iCurrentCamera)[1].z = m_vRobots.at(0)->getPosition().z;
				}
			}
		}

		//Set ViewMatrix to be a lookat function for the new values
		ViewMatrix = glm::lookAt(glm::vec3(m_vCameras.at(m_iCurrentCamera)[0].x, m_vCameras.at(m_iCurrentCamera)[0].y, m_vCameras.at(m_iCurrentCamera)[0].z),
			glm::vec3(m_vCameras.at(m_iCurrentCamera)[1].x, m_vCameras.at(m_iCurrentCamera)[1].y, m_vCameras.at(m_iCurrentCamera)[1].z),
			glm::vec3(m_vCameras.at(m_iCurrentCamera)[2].x, m_vCameras.at(m_iCurrentCamera)[2].y, m_vCameras.at(m_iCurrentCamera)[2].z));
	}

	m_vShaders.at(0)->use(); //Uses the diffuse 3D shader
	m_vShaders.at(0)->setUniform("P", glm::perspective(60.0f, 16.0f / 9.0f, 0.1f, 1000.0f));
	m_vShaders.at(0)->setUniform("light.position", 0.0f, 1.0f, 0.0f);
	m_vShaders.at(0)->setUniform("light.ambient", 0.5f, 0.5f, 0.5f);
	m_vShaders.at(0)->setUniform("light.diffuse", 0.4f, 0.4f, 0.4f);
	m_vShaders.at(0)->setUniform("light.specular", 0.5f, 0.5f, 0.5f);

	//Sends the view matrix to the shaders
	m_vShaders.at(0)->setUniform("V", ViewMatrix);

	if (!m_vObjects.empty())
	{
		for (int i = 0; i < m_vObjects.size(); i++) //For every model in the scene
		{
			//Animate and update the objects
			m_vObjects.at(i)->animate();
			m_vObjects.at(i)->update();
		}
	}
	if (!m_vCollectables.empty() > 0)
	{
		for (int i = 0; i < m_vCollectables.size(); i++) //For every collectable
		{
			if (!m_vCollectables.at(i)->m_bCollected) //If the collectable hasn't been collected
			{
				if (m_vCollectables.at(i)->colliding(m_vRobots.at(0)->getPosition()))//If collision between robot and Collectables occurs
				{
					m_iCollectableCount++; //Increment collectable count
					std::cout << m_iCollectableCount << "\n"; //Output the amount of Collectables collected
					m_bNewCollectable = true;
					m_Clock.restart(); //Used for timing events
					
				}
			}
		}
	}

	if (!m_vObjects.empty())
	{
		for (int i = 0; i < m_vObjects.size(); i++) //For every object in the scene
		{
			m_vObjects.at(i)->render(); //render the object
		}
	}

	m_vShaders.at(2)->use();//Uses the 2D shader
	//Sets the projection matrix and draws the HUD images
	m_vShaders.at(2)->setUniform("P", glm::perspective(60.0f, 16.0f / 9.0f, 0.1f, 1000.0f));
	if (!m_vHUD.empty())
	{
		for (int i = 0; i < m_vHUD.size(); i++) //For every HUD model in the scene
		{
			m_vHUD.at(i)->update();
			m_vHUD.at(i)->render();
		}
	}

	//Draws the Heads up display text
	m_vShaders.at(1)->use(); //Uses the 2D HUD Shader
	m_vShaders.at(1)->setUniform("P", glm::ortho(0.0f, 1280.0f, 0.0f, 720.0f));

	//Renders the UIText
	if (!m_vCameras.empty()) //Cameras
	{
		textHolder->render(m_vShaders.at(1)->getHandle(), "Current Camera: " + std::to_string(m_iCurrentCamera + 1) + " / " + std::to_string(m_vCameras.size()), 5.0f, 10.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
	}
	if (!m_vCollectables.empty()) //Collectables
	{
		textHolder->render(m_vShaders.at(1)->getHandle(), "Collectables collected: " + std::to_string(m_iCollectableCount) + " / " + std::to_string(m_vCollectables.size()), 5.0f, 40.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
		if (m_iCollectableCount >= m_vCollectables.size())
		{
			m_bSceneOver = true;
		}
	}

	if (m_bSceneOver)
	{
		textHolder->render(m_vShaders.at(1)->getHandle(), "Congratulations!", 450.0f, 380.0f, 2.0f, glm::vec3(1.0f, 1.0f, 1.0f));
		textHolder->render(m_vShaders.at(1)->getHandle(), "Press 'enter' to return to the menu", 450.0f, 320.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
	}

	//Outputs a message to the screen if a collectable has been collected
	if (m_bNewCollectable)
	{
		textHolder->render(m_vShaders.at(1)->getHandle(), "Collectable Collected", 500.0f, 550.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
		if (m_Clock.getElapsedTime().asSeconds() > 1.0f)
		{
			m_bNewCollectable = false;
		}
	}
	
}

void Scene::load(std::string sDir)
{
	tinyxml2::XMLDocument doc;//Holds the document being read
	doc.LoadFile(sDir.c_str()); //Loads in the file from the given directory

	//For every XML node in the file
	for (tinyxml2::XMLNode* currentChild = doc.FirstChild(); currentChild != NULL; currentChild = currentChild->NextSibling())
	{
		//If there is a scene tag
		if (strcmp(currentChild->Value(), "SCENE") ==0)
		{
			read(currentChild); //Go to the tags child
		}
	}
}

void Scene::read(tinyxml2::XMLNode* currentNode)
{
	glm::vec3 tempTransform; //Holds a temporary transform variable

	//For every child of the current node
	for (tinyxml2::XMLNode* currentChild = currentNode->FirstChild(); currentChild != NULL; currentChild = currentChild->NextSibling())
	{
		if (strcmp(currentChild->Value(), "ROBOT") == 0) //If the current child is a robot
		{
			Robot* newBot = new Robot;  //Creates a new robot
			m_vObjects.push_back(newBot); //Adds the robot to the objects list
			m_vRobots.push_back(newBot); //Adds the robot to the robot list
			newBot = NULL; //Clears the pointer
			read(currentChild); //Reads into the robot
		}

		else if (strcmp(currentChild->Value(), "GAMEOBJECT") == 0) //If the current child is a game object
		{
			m_vObjects.push_back(new GameObject); //Creates a new game object
			read(currentChild); //Reads into the game object
		}

		else if (strcmp(currentChild->Value(), "COLLECTABLE") == 0) //If the current child is a collectable
		{
			Collectable* newCollect = new Collectable; //Creates a new collectable
			m_vObjects.push_back(newCollect); //Adds the collectable to the objects list
			m_vCollectables.push_back(newCollect); //Adds the collectable to the collectable list
			newCollect = NULL; //Clears the pointer
			read(currentChild); //Reads into the collectable
		}
		else if (strcmp(currentChild->Value(), "HUD") == 0) //If the current child is a collectable
		{
			GameObject* newHUD = new GameObject; //Creates a new collectable
			m_vHUD.push_back(newHUD); //Adds the collectable to the collectable list
			newHUD = NULL; //Clears the pointer
			read(currentChild); //Reads into the collectable
		}
		else if (strcmp(currentChild->Value(), "MODEL") == 0) //If the current child is a model
		{
			if (strcmp(currentNode->Value(), "ROBOT") == 0 || strcmp(currentNode->Value(), "GAMEOBJECT") == 0 || strcmp(currentNode->Value(), "COLLECTABLE") == 0 )
			{
				m_vObjects.at(m_vObjects.size() - 1)->m_vParts.push_back(new Model(m_vShaders.at(0)->getHandle())); //Adds a model to the objects parts
			}
			else if (strcmp(currentNode->Value(), "HUD") == 0) //If loading a HUD
			{
				m_vHUD.at(m_vHUD.size() - 1)->m_vParts.push_back(new Model(m_vShaders.at(2)->getHandle()));
			}
			read(currentChild); //Reads into the model
		}
		else if (strcmp(currentChild->Value(), "SCALE") == 0) //If the current child is scale
		{
			std::istringstream iss(currentChild->ToElement()->GetText());
			
			iss >> tempTransform.x; //Read the x value
			iss >> tempTransform.y; //Read the y value
			iss >> tempTransform.z; //Read the z value
			
			//If scaling some form of object
			if (strcmp(currentNode->Value(), "ROBOT") == 0 || strcmp(currentNode->Value(), "GAMEOBJECT") == 0 || strcmp(currentNode->Value(), "COLLECTABLE") == 0)
			{
				m_vObjects.at(m_vObjects.size() - 1)->setScale(tempTransform); //Sets the objects scale
			}
			//If scaling a model
			else if (strcmp(currentNode->Value(), "MODEL") == 0) 
			{
				if (strcmp(currentNode->Parent()->Value(), "HUD") == 0) //If loading a HUD
				{
					m_vHUD.at(m_vHUD.size() - 1)->m_vParts.at(m_vHUD.at(m_vHUD.size() - 1)->m_vParts.size() - 1)->setScale(tempTransform);
				}
				else
				{
					m_vObjects.at(m_vObjects.size() - 1)->m_vParts.at(m_vObjects.at(m_vObjects.size() - 1)->m_vParts.size() - 1)->setScale(tempTransform); //Sets the models scale
				}
			}
			else if (strcmp(currentNode->Value(), "HUD") == 0) //If loading a HUD
			{
				m_vHUD.at(m_vHUD.size() - 1)->setScale(tempTransform);
			}
		}

		else if (strcmp(currentChild->Value(), "ROTATE") == 0) //If the current child is rotate
		{
			std::istringstream iss(currentChild->ToElement()->GetText());

			iss >> tempTransform.x; //Read the x value
			iss >> tempTransform.y; //Read the y value
			iss >> tempTransform.z; //Read the z value

			//If rotating some form of object
			if (strcmp(currentNode->Value(), "ROBOT") == 0 || strcmp(currentNode->Value(), "GAMEOBJECT") == 0 || strcmp(currentNode->Value(), "COLLECTABLE") == 0)
			{
				m_vObjects.at(m_vObjects.size() - 1)->setRotation(tempTransform);  //Sets the objects rotation
			}
			//If rotating a model
			else if (strcmp(currentNode->Value(), "MODEL") == 0)
			{
				if (strcmp(currentNode->Parent()->Value(), "HUD") == 0) //If loading a HUD
				{
					m_vHUD.at(m_vHUD.size() - 1)->m_vParts.at(m_vHUD.at(m_vHUD.size() - 1)->m_vParts.size() - 1)->setRotation(tempTransform);
				}
				else
				{
					m_vObjects.at(m_vObjects.size() - 1)->m_vParts.at(m_vObjects.at(m_vObjects.size() - 1)->m_vParts.size() - 1)->setRotation(tempTransform); //Sets the models rotation
				}
			}
			else if (strcmp(currentNode->Value(), "HUD") == 0) //If loading a HUD
			{
				m_vHUD.at(m_vHUD.size() - 1)->setRotation(tempTransform);
			}

		}
		else if (strcmp(currentChild->Value(), "TRANSLATE") == 0) //If the current child is translate
		{
			std::istringstream iss(currentChild->ToElement()->GetText());

			
			iss >> tempTransform.x; //Read the x value
			iss >> tempTransform.y; //Read the y value
			iss >> tempTransform.z; //Read the z value
			

			//If translating some form of object
			if (strcmp(currentNode->Value(), "ROBOT") == 0 || strcmp(currentNode->Value(), "GAMEOBJECT") == 0 || strcmp(currentNode->Value(), "COLLECTABLE") == 0)
			{
				m_vObjects.at(m_vObjects.size() - 1)->setPosition(tempTransform); //Sets the objects translation
			}
			//If translating a model
			else if (strcmp(currentNode->Value(), "MODEL") == 0)
			{ 
				if (strcmp(currentNode->Parent()->Value(), "HUD") == 0) //If loading a HUD
				{
					m_vHUD.at(m_vHUD.size() - 1)->m_vParts.at(m_vHUD.at(m_vHUD.size() - 1)->m_vParts.size() - 1)->setPosition(tempTransform);
				}
				else
				{
					m_vObjects.at(m_vObjects.size() - 1)->m_vParts.at(m_vObjects.at(m_vObjects.size() - 1)->m_vParts.size() - 1)->setPosition(tempTransform); //Sets the models translation
				}
			} 
			else if (strcmp(currentNode->Value(), "HUD") == 0) //If loading a HUD
			{
				m_vHUD.at(m_vHUD.size() - 1)->setPosition(tempTransform);
			}
		}
		else if (strcmp(currentChild->Value(), "OBJECT") == 0) //If the current child is an object
		{
			if (strcmp(currentNode->Value(), "MODEL") == 0)
			{
				if (strcmp(currentNode->Parent()->Value(), "HUD") == 0) //If loading a HUD
				{
					m_vHUD.at(m_vHUD.size() - 1)->m_vParts.at(m_vHUD.at(m_vHUD.size() - 1)->m_vParts.size() - 1)->loadObj(currentChild->ToElement()->GetText());
				}
				else
				{
					m_vObjects.at(m_vObjects.size() - 1)->m_vParts.at(m_vObjects.at(m_vObjects.size() - 1)->m_vParts.size() - 1)->loadObj(currentChild->ToElement()->GetText()); //Loads the model
				}
			}
		}
		else if (strcmp(currentChild->Value(), "TEXTURE") == 0) //If the current child is a texture
		{
			if (strcmp(currentNode->Value(), "MODEL") == 0)
			{
				if (strcmp(currentNode->Parent()->Value(), "HUD") == 0) //If loading a HUD
				{
					m_vHUD.at(m_vHUD.size() - 1)->m_vParts.at(m_vHUD.at(m_vHUD.size() - 1)->m_vParts.size() - 1)->setTexture(currentChild->ToElement()->GetText()); //Loads the texture
				}
				else
				{
					m_vObjects.at(m_vObjects.size() - 1)->m_vParts.at(m_vObjects.at(m_vObjects.size() - 1)->m_vParts.size() - 1)->setTexture(currentChild->ToElement()->GetText()); //Loads the texture
				}
			}
		}
		else if (strcmp(currentChild->Value(), "CAMERA") == 0) //If the current child is a camera
		{
			//Makes a new set of camera following variables
			m_vbUsePlayerPos.push_back(std::vector<bool>{false, false, false,
				false, false, false});
			read(currentChild);
			m_vCameras.push_back(tempCamera); //Adds a new camera to the cameras list
		}
		else if (strcmp(currentChild->Value(), "POSITION") == 0) //If the current child is a position
		{
			if (strcmp(currentNode->Value(), "CAMERA") == 0) //If the childs parent node is a camera
			{
				std::istringstream iss(currentChild->ToElement()->GetText());
				std::string token;
				for (int i = 0; i < 3; i++)
				{
					iss >> token;
					if (token == "p") //If the value is a p
					{
						m_vbUsePlayerPos[m_vbUsePlayerPos.size() - 1][i] = true; //Set the camera to follow that coordinate
					}
					else
					{
						if (i == 0)
							tempCamera[0].x = stof(token); //Read the item
						if (i == 1)
							tempCamera[0].y = stof(token); //Read the item
						if (i == 2)
							tempCamera[0].z = stof(token); //Read the item
					}
				}
			}
		}
		else if (strcmp(currentChild->Value(), "VIEW") == 0) //If the current child is a view
		{
			std::istringstream iss(currentChild->ToElement()->GetText());
			std::string token;
			for (int i = 0; i < 3; i++)
			{
				iss >> token;
				if (token == "p") //If the value is a p
				{
					m_vbUsePlayerPos[m_vbUsePlayerPos.size() - 1][i + 3] = true; //Set the camera to follow that coordinate
				}
				else
				{
					if (i == 0)
						tempCamera[1].x = stof(token); //Read the item
					if (i == 1)
						tempCamera[1].y = stof(token); //Read the item
					if (i == 2)
						tempCamera[1].z = stof(token); //Read the item
				}
			}
		}
		else if (strcmp(currentChild->Value(), "UP") == 0)  //If the current child is up
		{
			std::istringstream iss(currentChild->ToElement()->GetText());

			//Sets the up variables
			iss >> tempCamera[2].x; 
			iss >> tempCamera[2].y;
			iss >> tempCamera[2].z;
		}
	}
}

void Scene::moveRobot(float fDirection)
{
	if (!m_vRobots.empty())
	{
		m_vRobots.at(0)->move(fDirection);
	}
}

void Scene::turnRobot(float fDirection)
{
	if (!m_vRobots.empty())
	{
		m_vRobots.at(0)->turn(fDirection);
	}
}

void Scene::setRobotMoving(bool bValue)
{
	if (!m_vRobots.empty())
	{
		m_vRobots.at(0)->setMoving(bValue);
	}
}

void Scene::switchCamera(int iDirection)
{
	m_iCurrentCamera += iDirection; //Changes the current camera variable

	//Sets a loop if the boundaries are passed
	if (m_iCurrentCamera < 0)
	{
		m_iCurrentCamera = (int)m_vCameras.size() - 1;
	}
	if (m_iCurrentCamera >= m_vCameras.size())
	{
		m_iCurrentCamera = 0;
	}
}

bool Scene::getSceneOver()
{
	return m_bSceneOver;
}

Scene::~Scene()
{
	//Clears all of the data and pointers for all of the vectors.
	if (!m_vShaders.empty())
	{
		for (int i = 0; i < m_vShaders.size(); i++)
		{
			delete(m_vShaders.at(i));
			m_vShaders.at(i) = NULL;
		}
	}
	if (m_vObjects.size() > 0)
	{
		for (int i = 0; i < m_vObjects.size(); i++)
		{
			m_vObjects.at(i)->~GameObject();
			delete(m_vObjects.at(i));
			m_vObjects.at(i) = NULL;
		}
	}
	if (m_vRobots.size() > 0)
	{
		for (int i = 0; i < m_vRobots.size(); i++)
		{
			m_vRobots.at(i) = NULL;
		}
	}
	if (m_vCollectables.size() > 0)
	{
		for (int i = 0; i < m_vCollectables.size(); i++)
		{
			m_vCollectables.at(i) = NULL;
		}
	}
	if (m_vHUD.size() > 0)
	{
		for (int i = 0; i < m_vHUD.size(); i++)
		{
			m_vHUD.at(i)->~GameObject();
			delete(m_vHUD.at(i));
		}
	}
}