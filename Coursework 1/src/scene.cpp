#include "scene.h"
#include <fstream>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <tinyxml2.h>

Scene::Scene()
{
	init();

	//Sets up the pespective and passes it to the shader
	glm::mat4 PerspMatrix = glm::perspective(60.0f, 1280.0f / 720.0f, 0.1f, 1000.0f);

	gl::UniformMatrix4fv(gl::GetUniformLocation(m_uiProgramHandle, "P"), 1, gl::FALSE_, glm::value_ptr(PerspMatrix));

	m_iCurrentCamera = 0; //Sets the current camera to the first camera
	m_iCollectableCount - 0; //Sets the collectable count to 0
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

	//Check every coordinate in the camera to see if it's locked to the player
	for (int i = 0; i <  m_vbUsePlayerPos.at(m_iCurrentCamera).size(); i++) 
	{
		//If it is set it's variable to the according player position
		if ( m_vbUsePlayerPos.at(m_iCurrentCamera).at(i))
		{
			//ViewX
			if (i == 0)
			{
				m_vCameras.at(m_iCurrentCamera)[0].x = m_vRobots.at(0)->getPosition().x;
			}
			//ViewY
			else if (i == 1)
			{
				m_vCameras.at(m_iCurrentCamera)[0].y =  m_vRobots.at(0)->getPosition().y;
			}
			//ViewZ
			else if (i == 2)
			{
				m_vCameras.at(m_iCurrentCamera)[0].z =  m_vRobots.at(0)->getPosition().z;
			}

			//PositionX
			else if (i == 3)
			{
				m_vCameras.at(m_iCurrentCamera)[1].x =  m_vRobots.at(0)->getPosition().x;
			}
			//PositionY
			else if (i == 4)
			{
				m_vCameras.at(m_iCurrentCamera)[1].y =  m_vRobots.at(0)->getPosition().y;
			}
			//PositionZ
			else if (i == 5)
			{
				m_vCameras.at(m_iCurrentCamera)[1].z = m_vRobots.at(0)->getPosition().z;
			}
		}
	}

	gl::Uniform3f(gl::GetUniformLocation(m_uiProgramHandle, "light.position"), 0.0f, 1.0f, 0.0f);
	gl::Uniform3f(gl::GetUniformLocation(m_uiProgramHandle, "light.ambient") , 0.5f, 0.5f, 0.5f);
	gl::Uniform3f(gl::GetUniformLocation(m_uiProgramHandle, "light.diffuse") , 0.4f, 0.4f, 0.4f);
	gl::Uniform3f(gl::GetUniformLocation(m_uiProgramHandle, "light.specular"), 0.5f, 0.5f, 0.5f);

	//Set ViewMatrix to be a lookat function for the new values
	glm::mat4 ViewMatrix = glm::lookAt(glm::vec3(m_vCameras.at(m_iCurrentCamera)[0].x, m_vCameras.at(m_iCurrentCamera)[0].y, m_vCameras.at(m_iCurrentCamera)[0].z),
							  glm::vec3(m_vCameras.at(m_iCurrentCamera)[1].x, m_vCameras.at(m_iCurrentCamera)[1].y, m_vCameras.at(m_iCurrentCamera)[1].z),
							  glm::vec3(m_vCameras.at(m_iCurrentCamera)[2].x, m_vCameras.at(m_iCurrentCamera)[2].y, m_vCameras.at(m_iCurrentCamera)[2].z));

	//Sets the matrix for the camera and shaders
	gl::UniformMatrix4fv(gl::GetUniformLocation(m_uiProgramHandle, "V"), 1, gl::FALSE_, glm::value_ptr(ViewMatrix));

	for (int i = 0; i < m_vObjects.size(); i++) //For every model in the scene
	{
		//Animate and update the objects
		m_vObjects.at(i)->animate();
		m_vObjects.at(i)->update();
	}
	
	for (int i = 0; i < m_vCollectables.size(); i++) //For every collectable
	{
		if (!m_vCollectables.at(i)->m_bCollected) //If the collectable hasn't been collected
		{
			if (m_vCollectables.at(i)->colliding(m_vRobots.at(0)->getPosition()))//If collision between robot and Collectables occurs
			{
				m_iCollectableCount++;					   //Increment collectable count
				std::cout << m_iCollectableCount << "\n"; //Output the amount of Collectables collected
			}
		}
	}
	
	for (int i = 0; i < m_vObjects.size(); i++) //For every object in the scene
	{
		m_vObjects.at(i)->render(); //render the object
	}
}

void Scene::load(std::string dir)
{
	tinyxml2::XMLDocument doc;
	doc.LoadFile(dir.c_str());
	tinyxml2::XMLNode* node = doc.FirstChild();
	std::string Reading;
	glm::vec3 tempTransform; //Holds a temporary transform variable
	for (tinyxml2::XMLNode* currentChild = node; currentChild != NULL; currentChild = currentChild->NextSibling())
	{
		const char* val = currentChild->Value();
		printf("Name of play (1): %s\n", val);
		
		if (strcmp(currentChild->Value(), "SCENE") ==0)
		{
			for (tinyxml2::XMLNode* currentChild2 = currentChild->FirstChild(); currentChild2 != NULL; currentChild2 = currentChild2->NextSibling())
			{
				val = currentChild2->Value();
				printf("Name of play (1): %s\n", val);
				if (strcmp(currentChild2->Value(), "ROBOT") == 0)
				{
					Robot* newBot = new Robot;
					m_vObjects.push_back(newBot);
					m_vRobots.push_back(newBot);
					newBot = NULL;

					for (tinyxml2::XMLNode* currentChild3 = currentChild2->FirstChild(); currentChild3 != NULL; currentChild3 = currentChild3->NextSibling())
					{
						val = currentChild3->Value();
						printf("Name of play (1): %s\n", val);
						if (strcmp(currentChild3->Value(), "SCALE") == 0)
						{
							std::istringstream iss(currentChild3->ToElement()->GetText());
							for (int i = 0; i < 3; i++)
							{
								iss >> tempTransform.x; //Read the x value
								iss >> tempTransform.y; //Read the y value
								iss >> tempTransform.z; //Read the z value
							}

							m_vObjects.at(m_vObjects.size() - 1)->setScale(tempTransform);
							printf("Name of play (1): %s\n", currentChild3->ToElement()->GetText());
						}
						if (strcmp(currentChild3->Value(), "ROTATE") == 0)
						{
							std::istringstream iss(currentChild3->ToElement()->GetText());
							for (int i = 0; i < 3; i++)
							{
								iss >> tempTransform.x; //Read the x value
								iss >> tempTransform.y; //Read the y value
								iss >> tempTransform.z; //Read the z value
							}

							m_vObjects.at(m_vObjects.size() - 1)->setRotation(tempTransform);
							printf("Name of play (1): %s\n", currentChild3->ToElement()->GetText());
						}
						if (strcmp(currentChild3->Value(), "TRANSLATE") == 0)
						{
							std::istringstream iss(currentChild3->ToElement()->GetText());
							for (int i = 0; i < 3; i++)
							{
								iss >> tempTransform.x; //Read the x value
								iss >> tempTransform.y; //Read the y value
								iss >> tempTransform.z; //Read the z value
							}

							m_vObjects.at(m_vObjects.size() - 1)->setPosition(tempTransform);
							printf("Name of play (1): %s\n", currentChild3->ToElement()->GetText());
						}
						if (strcmp(currentChild3->Value(), "MODEL") == 0)
						{
							m_vObjects.at(m_vObjects.size()-1)->m_vParts.push_back(new Model(m_uiProgramHandle));
							for (tinyxml2::XMLNode* currentChild4 = currentChild3->FirstChild(); currentChild4 != NULL; currentChild4 = currentChild4->NextSibling())
							{
								val = currentChild4->Value();
								printf("Name of play (1): %s\n", val);
								if (strcmp(currentChild4->Value(), "OBJECT") == 0)
								{
									m_vObjects.at(m_vObjects.size()-1)->m_vParts.at(m_vObjects.at(m_vObjects.size() - 1)->m_vParts.size() - 1)->loadObj(currentChild4->ToElement()->GetText());
									printf("Name of play (1): %s\n", currentChild4->ToElement()->GetText());
								}
								if (strcmp(currentChild4->Value(), "TEXTURE") == 0)
								{
									m_vObjects.at(m_vObjects.size()-1)->m_vParts.at(m_vObjects.at(m_vObjects.size() - 1)->m_vParts.size() - 1)->setTexture(currentChild4->ToElement()->GetText());
									printf("Name of play (1): %s\n", currentChild4->ToElement()->GetText());
								}
								if (strcmp(currentChild4->Value(), "SCALE") == 0)
								{
									std::istringstream iss(currentChild4->ToElement()->GetText());
									for (int i = 0; i < 3; i++)
									{
										iss >> tempTransform.x; //Read the x value
										iss >> tempTransform.y; //Read the y value
										iss >> tempTransform.z; //Read the z value
									}

									m_vObjects.at(m_vObjects.size() - 1)->m_vParts.at(m_vObjects.at(m_vObjects.size() - 1)->m_vParts.size() - 1)->setScale(tempTransform);
									printf("Name of play (1): %s\n", currentChild4->ToElement()->GetText());
								}
								if (strcmp(currentChild4->Value(), "ROTATE") == 0)
								{
									std::istringstream iss(currentChild4->ToElement()->GetText());
									for (int i = 0; i < 3; i++)
									{
										iss >> tempTransform.x; //Read the x value
										iss >> tempTransform.y; //Read the y value
										iss >> tempTransform.z; //Read the z value
									}

									m_vObjects.at(m_vObjects.size() - 1)->m_vParts.at(m_vObjects.at(m_vObjects.size() - 1)->m_vParts.size() - 1)->setRotation(tempTransform);
									printf("Name of play (1): %s\n", currentChild4->ToElement()->GetText());
								}
								if (strcmp(currentChild4->Value(), "TRANSLATE") == 0)
								{
									std::istringstream iss(currentChild4->ToElement()->GetText());
									for (int i = 0; i < 3; i++)
									{
										iss >> tempTransform.x; //Read the x value
										iss >> tempTransform.y; //Read the y value
										iss >> tempTransform.z; //Read the z value
									}

									m_vObjects.at(m_vObjects.size() - 1)->m_vParts.at(m_vObjects.at(m_vObjects.size() - 1)->m_vParts.size() - 1)->setPosition(tempTransform);
									printf("Name of play (1): %s\n", currentChild4->ToElement()->GetText());
								}
							}
						}
					}
				}
				if (strcmp(currentChild2->Value(), "GAMEOBJECT") == 0) 
				{
					m_vObjects.push_back(new GameObject);
					for (tinyxml2::XMLNode* currentChild3 = currentChild2->FirstChild(); currentChild3 != NULL; currentChild3 = currentChild3->NextSibling())
					{
						val = currentChild3->Value();
						printf("Name of play (1): %s\n", val);
						if (strcmp(currentChild3->Value(), "SCALE") == 0)
						{
							std::istringstream iss(currentChild3->ToElement()->GetText());
							for (int i = 0; i < 3; i++)
							{
								iss >> tempTransform.x; //Read the x value
								iss >> tempTransform.y; //Read the y value
								iss >> tempTransform.z; //Read the z value
							}

							m_vObjects.at(m_vObjects.size() - 1)->setScale(tempTransform);
							printf("Name of play (1): %s\n", currentChild3->ToElement()->GetText());
						}
						if (strcmp(currentChild3->Value(), "ROTATE") == 0)
						{
							std::istringstream iss(currentChild3->ToElement()->GetText());
							for (int i = 0; i < 3; i++)
							{
								iss >> tempTransform.x; //Read the x value
								iss >> tempTransform.y; //Read the y value
								iss >> tempTransform.z; //Read the z value
							}

							m_vObjects.at(m_vObjects.size() - 1)->setRotation(tempTransform);
							printf("Name of play (1): %s\n", currentChild3->ToElement()->GetText());
						}
						if (strcmp(currentChild3->Value(), "TRANSLATE") == 0)
						{
							std::istringstream iss(currentChild3->ToElement()->GetText());
							for (int i = 0; i < 3; i++)
							{
								iss >> tempTransform.x; //Read the x value
								iss >> tempTransform.y; //Read the y value
								iss >> tempTransform.z; //Read the z value
							}

							m_vObjects.at(m_vObjects.size() - 1)->setPosition(tempTransform);
							printf("Name of play (1): %s\n", currentChild3->ToElement()->GetText());
						}
						if (strcmp(currentChild3->Value(), "MODEL") == 0)
						{
							m_vObjects.at(m_vObjects.size() - 1)->m_vParts.push_back(new Model(m_uiProgramHandle));
							for (tinyxml2::XMLNode* currentChild4 = currentChild3->FirstChild(); currentChild4 != NULL; currentChild4 = currentChild4->NextSibling())
							{
								val = currentChild4->Value();
								printf("Name of play (1): %s\n", val);
								if (strcmp(currentChild4->Value(), "OBJECT") == 0)
								{
									m_vObjects.at(m_vObjects.size()-1)->m_vParts.at(m_vObjects.at(m_vObjects.size() - 1)->m_vParts.size() - 1)->loadObj(currentChild4->ToElement()->GetText());
									printf("Name of play (1): %s\n", currentChild4->ToElement()->GetText());
								}
								if (strcmp(currentChild4->Value(), "TEXTURE") == 0)
								{
									m_vObjects.at(m_vObjects.size()-1)->m_vParts.at(m_vObjects.at(m_vObjects.size() - 1)->m_vParts.size() - 1)->setTexture(currentChild4->ToElement()->GetText());
									printf("Name of play (1): %s\n", currentChild4->ToElement()->GetText());
								}
								if (strcmp(currentChild4->Value(), "SCALE") == 0)
								{
									std::istringstream iss(currentChild4->ToElement()->GetText());
									for (int i = 0; i < 3; i++)
									{
										iss >> tempTransform.x; //Read the x value
										iss >> tempTransform.y; //Read the y value
										iss >> tempTransform.z; //Read the z value
									}

									m_vObjects.at(m_vObjects.size() - 1)->m_vParts.at(m_vObjects.at(m_vObjects.size() - 1)->m_vParts.size() - 1)->setScale(tempTransform);
									printf("Name of play (1): %s\n", currentChild4->ToElement()->GetText());
								}
								if (strcmp(currentChild4->Value(), "ROTATE") == 0)
								{
									std::istringstream iss(currentChild4->ToElement()->GetText());
									for (int i = 0; i < 3; i++)
									{
										iss >> tempTransform.x; //Read the x value
										iss >> tempTransform.y; //Read the y value
										iss >> tempTransform.z; //Read the z value
									}

									m_vObjects.at(m_vObjects.size() - 1)->m_vParts.at(m_vObjects.at(m_vObjects.size() - 1)->m_vParts.size() - 1)->setRotation(tempTransform);
									printf("Name of play (1): %s\n", currentChild4->ToElement()->GetText());
								}
								if (strcmp(currentChild4->Value(), "TRANSLATE") == 0)
								{
									std::istringstream iss(currentChild4->ToElement()->GetText());
									for (int i = 0; i < 3; i++)
									{
										iss >> tempTransform.x; //Read the x value
										iss >> tempTransform.y; //Read the y value
										iss >> tempTransform.z; //Read the z value
									}

									m_vObjects.at(m_vObjects.size() - 1)->m_vParts.at(m_vObjects.at(m_vObjects.size() - 1)->m_vParts.size() - 1)->setPosition(tempTransform);
									printf("Name of play (1): %s\n", currentChild4->ToElement()->GetText());
								}
							}
						}
					}
				}
				if (strcmp(currentChild2->Value(), "COLLECTABLE") == 0) 
				{
					Collectable* newCollect = new Collectable;
					m_vObjects.push_back(newCollect);
					m_vCollectables.push_back(newCollect);
					newCollect = NULL;
					for (tinyxml2::XMLNode* currentChild3 = currentChild2->FirstChild(); currentChild3 != NULL; currentChild3 = currentChild3->NextSibling())
					{
						val = currentChild3->Value();
						printf("Name of play (1): %s\n", val);
						if (strcmp(currentChild3->Value(), "SCALE") == 0)
						{
							std::istringstream iss(currentChild3->ToElement()->GetText());
							for (int i = 0; i < 3; i++)
							{
								iss >> tempTransform.x; //Read the x value
								iss >> tempTransform.y; //Read the y value
								iss >> tempTransform.z; //Read the z value
							}

							m_vObjects.at(m_vObjects.size() - 1)->setScale(tempTransform);
							printf("Name of play (1): %s\n", currentChild3->ToElement()->GetText());
						}
						if (strcmp(currentChild3->Value(), "ROTATE") == 0)
						{
							std::istringstream iss(currentChild3->ToElement()->GetText());
							for (int i = 0; i < 3; i++)
							{
								iss >> tempTransform.x; //Read the x value
								iss >> tempTransform.y; //Read the y value
								iss >> tempTransform.z; //Read the z value
							}

							m_vObjects.at(m_vObjects.size() - 1)->setRotation(tempTransform);
							printf("Name of play (1): %s\n", currentChild3->ToElement()->GetText());
						}
						if (strcmp(currentChild3->Value(), "TRANSLATE") == 0)
						{
							std::istringstream iss(currentChild3->ToElement()->GetText());
							for (int i = 0; i < 3; i++)
							{
								iss >> tempTransform.x; //Read the x value
								iss >> tempTransform.y; //Read the y value
								iss >> tempTransform.z; //Read the z value
							}

							m_vObjects.at(m_vObjects.size() - 1)->setPosition(tempTransform);
							printf("Name of play (1): %s\n", currentChild3->ToElement()->GetText());
						}
						if (strcmp(currentChild3->Value(), "MODEL") == 0)
						{
							m_vObjects.at(m_vObjects.size() - 1)->m_vParts.push_back(new Model(m_uiProgramHandle));
						
							for (tinyxml2::XMLNode* currentChild4 = currentChild3->FirstChild(); currentChild4 != NULL; currentChild4 = currentChild4->NextSibling())
							{
								val = currentChild4->Value();
								printf("Name of play (1): %s\n", val);
								if (strcmp(currentChild4->Value(), "OBJECT") == 0)
								{
									m_vObjects.at(m_vObjects.size()-1)->m_vParts.at(m_vObjects.at(m_vObjects.size() - 1)->m_vParts.size() - 1)->loadObj(currentChild4->ToElement()->GetText());
									printf("Name of play (1): %s\n", currentChild4->ToElement()->GetText());
								}
								if (strcmp(currentChild4->Value(), "TEXTURE") == 0)
								{	
									m_vObjects.at(m_vObjects.size()-1)->m_vParts.at(m_vObjects.at(m_vObjects.size() - 1)->m_vParts.size() - 1)->setTexture(currentChild4->ToElement()->GetText());
									printf("Name of play (1): %s\n", currentChild4->ToElement()->GetText());
								}
								if (strcmp(currentChild4->Value(), "SCALE") == 0)
								{
									std::istringstream iss(currentChild4->ToElement()->GetText());
									for (int i = 0; i < 3; i++)
									{
										iss >> tempTransform.x; //Read the x value
										iss >> tempTransform.y; //Read the y value
										iss >> tempTransform.z; //Read the z value
									}

									m_vObjects.at(m_vObjects.size() - 1)->m_vParts.at(m_vObjects.at(m_vObjects.size() - 1)->m_vParts.size() - 1)->setScale(tempTransform);
									printf("Name of play (1): %s\n", currentChild4->ToElement()->GetText());
								}
								if (strcmp(currentChild4->Value(), "ROTATE") == 0)
								{
									std::istringstream iss(currentChild4->ToElement()->GetText());
									for (int i = 0; i < 3; i++)
									{
										iss >> tempTransform.x; //Read the x value
										iss >> tempTransform.y; //Read the y value
										iss >> tempTransform.z; //Read the z value
									}

									m_vObjects.at(m_vObjects.size() - 1)->m_vParts.at(m_vObjects.at(m_vObjects.size() - 1)->m_vParts.size() - 1)->setRotation(tempTransform);
									printf("Name of play (1): %s\n", currentChild4->ToElement()->GetText());
								}
								if (strcmp(currentChild4->Value(), "TRANSLATE") == 0)
								{
									std::istringstream iss(currentChild4->ToElement()->GetText());
									for (int i = 0; i < 3; i++)
									{
										iss >> tempTransform.x; //Read the x value
										iss >> tempTransform.y; //Read the y value
										iss >> tempTransform.z; //Read the z value
									}
									
									m_vObjects.at(m_vObjects.size()-1)->m_vParts.at(m_vObjects.at(m_vObjects.size()-1)->m_vParts.size()-1)->setPosition(tempTransform);

								
									printf("Name of play (1): %s\n", currentChild4->ToElement()->GetText());
								}
							}
						}
					}
				}
				if (strcmp(currentChild2->Value(), "CAMERA") == 0) 
				{
					glm::mat3 tempCamera; //Holds a temporary camera for building 
					m_vbUsePlayerPos.push_back(std::vector<bool>{false, false, false,
						false, false, false});

					for (tinyxml2::XMLNode* currentChild3 = currentChild2->FirstChild(); currentChild3 != NULL; currentChild3 = currentChild3->NextSibling())
					{
						val = currentChild3->Value();
						printf("Name of play (1): %s\n", val);
						if (strcmp(currentChild3->Value(), "POSITION") == 0)
						{
							std::istringstream iss(currentChild3->ToElement()->GetText());
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
									if(i == 0)
										tempCamera[0].x = stof(token); //Read the item
									if(i == 1)
										tempCamera[0].y = stof(token); //Read the item
									if(i==2)
										tempCamera[0].z = stof(token); //Read the item
								}
							}
							printf("Name of play (1): %s\n", currentChild3->ToElement()->GetText());
						}
						if (strcmp(currentChild3->Value(), "VIEW") == 0)
						{
							std::istringstream iss(currentChild3->ToElement()->GetText());
							std::string token;
							for (int i = 0; i < 3; i++)
							{
								iss >> token;
								if (token == "p") //If the value is a p
								{
									m_vbUsePlayerPos[m_vbUsePlayerPos.size() - 1][i+3] = true; //Set the camera to follow that coordinate
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
							printf("Name of play (1): %s\n", currentChild3->ToElement()->GetText());
						}
						if (strcmp(currentChild3->Value(), "UP") == 0)
						{
							std::istringstream iss(currentChild3->ToElement()->GetText());
							iss >> tempCamera[2].x;
							iss >> tempCamera[2].y;
							iss >> tempCamera[2].z;
							printf("Name of play (1): %s\n", currentChild3->ToElement()->GetText());
						}
					}
					m_vCameras.push_back(tempCamera);
				}
			}
		}
	}
}

void Scene::moveRobot(float Direction)
{
	m_vRobots.at(0)->Move(Direction); 
}

void Scene::turnRobot(float Direction)
{
	m_vRobots.at(0)->Turn(Direction);
}

void Scene::switchCamera(int Direction)
{
	m_iCurrentCamera += Direction; //Changes the current camera variable

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