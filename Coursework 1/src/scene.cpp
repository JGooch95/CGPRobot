#include "scene.h"
#include <fstream>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Scene::Scene()
{
	init(); //Initialises the shaders

	//Sets up the pespective and passes it to the shader
	glm::mat4 perspMatrix = glm::perspective(60.0f, 16.0f / 9.0f, 0.1f, 1000.0f);
	gl::UniformMatrix4fv(gl::GetUniformLocation(m_uiProgramHandle, "P"), 1, gl::FALSE_, glm::value_ptr(perspMatrix));

	m_iCurrentCamera = 0; //Sets the current camera to the first camera
	m_iCollectableCount = 0; //Sets the collectable count to 0
}

void Scene::init()
{	
	//////////////////////////////////////////////////////
	/////////// Vertex shader //////////////////////////
	//////////////////////////////////////////////////////

	// Load contents of file
	std::ifstream inFile("Assets/shader/diffuse.vert");
	if (!inFile) {
		fprintf(stderr, "Error opening file: shader/modifiedBasic.vert\n");
		exit(1);
	}

	std::stringstream code;
	code << inFile.rdbuf();
	inFile.close();
	std::string codeStr(code.str());

	// Create the shader object
	GLuint vertShader = gl::CreateShader(gl::VERTEX_SHADER);
	if (0 == vertShader) {
		fprintf(stderr, "Error creating vertex shader.\n");
		exit(EXIT_FAILURE);
	}

	// Load the source code into the shader object
	const GLchar* codeArray[] = { codeStr.c_str() };
	gl::ShaderSource(vertShader, 1, codeArray, NULL);

	// Compile the shader
	gl::CompileShader(vertShader);

	// Check compilation status
	GLint result;
	gl::GetShaderiv(vertShader, gl::COMPILE_STATUS, &result);
	if (FALSE == result) {
		fprintf(stderr, "Vertex shader compilation failed!\n");

		GLint logLen;
		gl::GetShaderiv(vertShader, gl::INFO_LOG_LENGTH, &logLen);

		if (logLen > 0) {
			char * log = (char *)malloc(logLen);

			GLsizei written;
			gl::GetShaderInfoLog(vertShader, logLen, &written, log);

			fprintf(stderr, "Shader log: \n%s", log);

			free(log);
		}
	}

	//////////////////////////////////////////////////////
	/////////// Fragment shader //////////////////////////
	//////////////////////////////////////////////////////

	// Load contents of file into shaderCode here
	std::ifstream fragFile("Assets/shader/diffuse.frag");
	if (!fragFile) {
		fprintf(stderr, "Error opening file: shader/basic.frag\n");
		exit(1);
	}

	std::stringstream fragCode;
	fragCode << fragFile.rdbuf();
	fragFile.close();
	codeStr = fragCode.str();

	// Create the shader object
	GLuint fragShader = gl::CreateShader(gl::FRAGMENT_SHADER);
	if (0 == fragShader) {
		fprintf(stderr, "Error creating fragment shader.\n");
		exit(1);
	}

	// Load the source code into the shader object
	codeArray[0] = codeStr.c_str();
	gl::ShaderSource(fragShader, 1, codeArray, NULL);

	// Compile the shader
	gl::CompileShader(fragShader);

	// Check compilation status
	gl::GetShaderiv(fragShader, gl::COMPILE_STATUS, &result);
	if (FALSE == result) {
		fprintf(stderr, "Fragment shader compilation failed!\n");

		GLint logLen;
		gl::GetShaderiv(fragShader, gl::INFO_LOG_LENGTH, &logLen);

		if (logLen > 0) {
			char * log = (char *)malloc(logLen);

			GLsizei written;
			gl::GetShaderInfoLog(fragShader, logLen, &written, log);

			fprintf(stderr, "Shader log: \n%s", log);

			free(log);
		}
	}

	linkMe(vertShader, fragShader);
}

void Scene::linkMe(GLint vertShader, GLint fragShader) {
	// Create the program object
	m_uiProgramHandle = gl::CreateProgram();
	if (0 == m_uiProgramHandle) {
		fprintf(stderr, "Error creating program object.\n");
		exit(1);
	}

	// Attach the shaders to the program object
	gl::AttachShader(m_uiProgramHandle, vertShader);
	gl::AttachShader(m_uiProgramHandle, fragShader);

	// Link the program
	gl::LinkProgram(m_uiProgramHandle);

	// Check for successful linking
	GLint status;
	gl::GetProgramiv(m_uiProgramHandle, gl::LINK_STATUS, &status);
	if (FALSE == status) {

		fprintf(stderr, "Failed to link shader program!\n");

		GLint logLen;
		gl::GetProgramiv(m_uiProgramHandle, gl::INFO_LOG_LENGTH, &logLen);

		if (logLen > 0) {
			char * log = (char *)malloc(logLen);

			GLsizei written;
			gl::GetProgramInfoLog(m_uiProgramHandle, logLen, &written, log);

			fprintf(stderr, "Program log: \n%s", log);

			free(log);
		}
	}
	else {
		gl::UseProgram(m_uiProgramHandle);
	}
}

void Scene::update()
{
	//Clear the buffers ready for drawing
	gl::Clear(gl::COLOR_BUFFER_BIT | gl::DEPTH_BUFFER_BIT);

	if (m_vCameras.size() > 0)
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
	}

	configureLights();

	if (m_vObjects.size() > 0)
	{
		for (int i = 0; i < m_vObjects.size(); i++) //For every model in the scene
		{
			//Animate and update the objects
			m_vObjects.at(i)->animate();
			m_vObjects.at(i)->update();
		}
	}
	if (m_vHUD.size() > 0)
	{
		for (int i = 0; i < m_vHUD.size(); i++) //For every model in the scene
		{
			m_vHUD.at(i)->update();
		}
	}
	if (m_vCollectables.size() > 0)
	{
		for (int i = 0; i < m_vCollectables.size(); i++) //For every collectable
		{
			if (!m_vCollectables.at(i)->m_bCollected) //If the collectable hasn't been collected
			{
				if (m_vCollectables.at(i)->colliding(m_vRobots.at(0)->getPosition()))//If collision between robot and Collectables occurs
				{
					m_iCollectableCount++; //Increment collectable count
					std::cout << m_iCollectableCount << "\n"; //Output the amount of Collectables collected
				}
			}
		}
	}

	if (m_vObjects.size() > 0)
	{
		for (int i = 0; i < m_vObjects.size(); i++) //For every object in the scene
		{
			if (m_vObjects.at(i)->m_bUseLights)
			{
				configureLights();
				//Set ViewMatrix to be a lookat function for the new values
				glm::mat4 ViewMatrix = glm::lookAt(glm::vec3(m_vCameras.at(m_iCurrentCamera)[0].x, m_vCameras.at(m_iCurrentCamera)[0].y, m_vCameras.at(m_iCurrentCamera)[0].z),
					glm::vec3(m_vCameras.at(m_iCurrentCamera)[1].x, m_vCameras.at(m_iCurrentCamera)[1].y, m_vCameras.at(m_iCurrentCamera)[1].z),
					glm::vec3(m_vCameras.at(m_iCurrentCamera)[2].x, m_vCameras.at(m_iCurrentCamera)[2].y, m_vCameras.at(m_iCurrentCamera)[2].z));
				//Sends the view matrix to the shaders
				gl::UniformMatrix4fv(gl::GetUniformLocation(m_uiProgramHandle, "V"), 1, gl::FALSE_, glm::value_ptr(ViewMatrix));
			}
			else
			{
				configureHUDLights();
				gl::UniformMatrix4fv(gl::GetUniformLocation(m_uiProgramHandle, "V"), 1, gl::FALSE_, glm::value_ptr(glm::mat4(1)));
			}
			m_vObjects.at(i)->render(); //render the object
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

	//Set the camera and collectable counters
	m_iCameraCount = m_vCameras.size(); 
	m_iCollectableAmount = m_vCollectables.size();
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
			m_vObjects.at(m_vObjects.size() - 1)->m_bUseLights = true;
			m_vRobots.push_back(newBot); //Adds the robot to the robot list
			newBot = NULL; //Clears the pointer
			read(currentChild); //Reads into the robot
		}

		else if (strcmp(currentChild->Value(), "GAMEOBJECT") == 0) //If the current child is a game object
		{
			m_vObjects.push_back(new GameObject); //Creates a new game object
			m_vObjects.at(m_vObjects.size() - 1)->m_bUseLights = true;
			read(currentChild); //Reads into the game object
		}

		else if (strcmp(currentChild->Value(), "COLLECTABLE") == 0) //If the current child is a collectable
		{
			Collectable* newCollect = new Collectable; //Creates a new collectable
			m_vObjects.push_back(newCollect); //Adds the collectable to the objects list
			m_vObjects.at(m_vObjects.size() - 1)->m_bUseLights = true;
			m_vCollectables.push_back(newCollect); //Adds the collectable to the collectable list
			newCollect = NULL; //Clears the pointer
			read(currentChild); //Reads into the collectable
		}
		else if (strcmp(currentChild->Value(), "HUD") == 0) //If the current child is a collectable
		{
			GameObject* newHUD = new GameObject; //Creates a new collectable
			m_vObjects.push_back(newHUD); //Adds the collectable to the objects list
			m_vHUD.push_back(newHUD); //Adds the collectable to the collectable list
			newHUD = NULL; //Clears the pointer
			m_vObjects.at(m_vObjects.size()-1)->m_bUseLights = false; //Adds the collectable to the objects list
			read(currentChild); //Reads into the collectable
		}
		else if (strcmp(currentChild->Value(), "MODEL") == 0) //If the current child is a model
		{
			if (strcmp(currentNode->Value(), "ROBOT") == 0 || strcmp(currentNode->Value(), "GAMEOBJECT") == 0 || strcmp(currentNode->Value(), "COLLECTABLE") == 0 || strcmp(currentNode->Value(), "HUD") == 0)
			{
				m_vObjects.at(m_vObjects.size() - 1)->m_vParts.push_back(new Model(m_uiProgramHandle)); //Adds a model to the objects parts
			}
			read(currentChild); //Reads into the model
		}
		else if (strcmp(currentChild->Value(), "SCALE") == 0) //If the current child is scale
		{
			std::istringstream iss(currentChild->ToElement()->GetText());
			for (int i = 0; i < 3; i++)
			{
				iss >> tempTransform.x; //Read the x value
				iss >> tempTransform.y; //Read the y value
				iss >> tempTransform.z; //Read the z value
			}

			//If scaling some form of object
			if (strcmp(currentNode->Value(), "ROBOT") == 0 || strcmp(currentNode->Value(), "GAMEOBJECT") == 0 || strcmp(currentNode->Value(), "COLLECTABLE") == 0 || strcmp(currentNode->Value(), "HUD") == 0)
			{
				m_vObjects.at(m_vObjects.size() - 1)->setScale(tempTransform); //Sets the objects scale
			}
			//If scaling a model
			else if (strcmp(currentNode->Value(), "MODEL") == 0) 
			{
				m_vObjects.at(m_vObjects.size() - 1)->m_vParts.at(m_vObjects.at(m_vObjects.size() - 1)->m_vParts.size() - 1)->setScale(tempTransform); //Sets the models scale
			}
		}

		else if (strcmp(currentChild->Value(), "ROTATE") == 0) //If the current child is rotate
		{
			std::istringstream iss(currentChild->ToElement()->GetText());

			for (int i = 0; i < 3; i++)
			{
				iss >> tempTransform.x; //Read the x value
				iss >> tempTransform.y; //Read the y value
				iss >> tempTransform.z; //Read the z value
			}

			//If rotating some form of object
			if (strcmp(currentNode->Value(), "ROBOT") == 0 || strcmp(currentNode->Value(), "GAMEOBJECT") == 0 || strcmp(currentNode->Value(), "COLLECTABLE") == 0 || strcmp(currentNode->Value(), "HUD") == 0)
			{
				m_vObjects.at(m_vObjects.size() - 1)->setRotation(tempTransform);  //Sets the objects rotation
			}
			//If rotating a model
			else if (strcmp(currentNode->Value(), "MODEL") == 0)
			{
				m_vObjects.at(m_vObjects.size() - 1)->m_vParts.at(m_vObjects.at(m_vObjects.size() - 1)->m_vParts.size() - 1)->setRotation(tempTransform); //Sets the models rotation
			}

		}
		else if (strcmp(currentChild->Value(), "TRANSLATE") == 0) //If the current child is translate
		{
			std::istringstream iss(currentChild->ToElement()->GetText());

			for (int i = 0; i < 3; i++)
			{
				iss >> tempTransform.x; //Read the x value
				iss >> tempTransform.y; //Read the y value
				iss >> tempTransform.z; //Read the z value
			}

			//If translating some form of object
			if (strcmp(currentNode->Value(), "ROBOT") == 0 || strcmp(currentNode->Value(), "GAMEOBJECT") == 0 || strcmp(currentNode->Value(), "COLLECTABLE") == 0 || strcmp(currentNode->Value(), "HUD") == 0)
			{
				m_vObjects.at(m_vObjects.size() - 1)->setPosition(tempTransform); //Sets the objects translation
			}
			//If translating a model
			else if (strcmp(currentNode->Value(), "MODEL") == 0)
			{
				m_vObjects.at(m_vObjects.size() - 1)->m_vParts.at(m_vObjects.at(m_vObjects.size() - 1)->m_vParts.size() - 1)->setPosition(tempTransform); //Sets the models translation
			}
		}
		else if (strcmp(currentChild->Value(), "OBJECT") == 0) //If the current child is an object
		{
			if (strcmp(currentNode->Value(), "MODEL") == 0)
			{
				m_vObjects.at(m_vObjects.size() - 1)->m_vParts.at(m_vObjects.at(m_vObjects.size() - 1)->m_vParts.size() - 1)->loadObj(currentChild->ToElement()->GetText()); //Loads the model
			}
		}
		else if (strcmp(currentChild->Value(), "TEXTURE") == 0) //If the current child is a texture
		{
			if (strcmp(currentNode->Value(), "MODEL") == 0)
			{
				m_vObjects.at(m_vObjects.size() - 1)->m_vParts.at(m_vObjects.at(m_vObjects.size() - 1)->m_vParts.size() - 1)->setTexture(currentChild->ToElement()->GetText()); //Loads the texture
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
		m_iCurrentCamera = m_vCameras.size() - 1;
	}
	if (m_iCurrentCamera >= m_vCameras.size())
	{
		m_iCurrentCamera = 0;
	}
}

void Scene::configureLights()
{
	//Sets up the light and passes it to the shader
	gl::Uniform3f(gl::GetUniformLocation(m_uiProgramHandle, "light.position"), 0.0f, 1.0f, 0.0f);
	gl::Uniform3f(gl::GetUniformLocation(m_uiProgramHandle, "light.ambient"), 0.5f, 0.5f, 0.5f);
	gl::Uniform3f(gl::GetUniformLocation(m_uiProgramHandle, "light.diffuse"), 0.4f, 0.4f, 0.4f);
	gl::Uniform3f(gl::GetUniformLocation(m_uiProgramHandle, "light.specular"), 0.5f, 0.5f, 0.5f);
}

void Scene::configureHUDLights()
{
	//Sets up the light and passes it to the shader
	gl::Uniform3f(gl::GetUniformLocation(m_uiProgramHandle, "light.position"), 0.0f, 0.0f, 1.0f);
	gl::Uniform3f(gl::GetUniformLocation(m_uiProgramHandle, "light.ambient"), 1.0f, 1.0f, 1.0f);
	gl::Uniform3f(gl::GetUniformLocation(m_uiProgramHandle, "light.diffuse"), 0.0f, 0.0f, 0.0f);
	gl::Uniform3f(gl::GetUniformLocation(m_uiProgramHandle, "light.specular"), 0.0f, 0.0f, 0.0f);
}

Scene::~Scene()
{
	//Clears all of the data and pointers for all of the vectors.
	for (int i = 0; i < m_vObjects.size(); i++)
	{
		m_vObjects.at(i)->~GameObject();
		delete(m_vObjects.at(i));
		m_vObjects.at(i) = NULL;
	}
	for (int i = 0; i < m_vRobots.size(); i++)
	{
		m_vRobots.at(i) = NULL;
	}
	for (int i = 0; i < m_vCollectables.size(); i++)
	{
		m_vCollectables.at(i) = NULL;
	}
}