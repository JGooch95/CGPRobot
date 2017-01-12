#include "scene.h"
#include <fstream>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Scene::Scene()
{

}

void Scene::init()
{	
	//////////////////////////////////////////////////////
	/////////// Vertex shader //////////////////////////
	//////////////////////////////////////////////////////

	// Load contents of file
	std::ifstream inFile("Assets/shader/modifiedBasic.vert");
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
	std::ifstream fragFile("Assets/shader/basic.frag");
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

	//Sets the view distance and angle of view.
	m_PerspMatrix = glm::perspective(glm::radians(60.0f), 1280.0f/720.0f, 0.1f, 1000.0f);
	m_iCurrentCamera = 0;
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

	m_iCollectableCount = 0;
}

void Scene::update()
{
	//Clear the buffers ready for drawing
	gl::Clear(gl::COLOR_BUFFER_BIT | gl::DEPTH_BUFFER_BIT);
	
	//Gets the location of the view and projection variables for the camera.
	GLint viewMatrixID = gl::GetUniformLocation(m_uiProgramHandle, "mView");
	GLint projectionMatrixID = gl::GetUniformLocation(m_uiProgramHandle, "mProjection");

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

	//Set ViewMatrix to be a lookat function for the new values
	glm::mat4 ViewMatrix = glm::lookAt(glm::vec3(m_vCameras.at(m_iCurrentCamera)[0].x, m_vCameras.at(m_iCurrentCamera)[0].y, m_vCameras.at(m_iCurrentCamera)[0].z),
							  glm::vec3(m_vCameras.at(m_iCurrentCamera)[1].x, m_vCameras.at(m_iCurrentCamera)[1].y, m_vCameras.at(m_iCurrentCamera)[1].z),
							  glm::vec3(m_vCameras.at(m_iCurrentCamera)[2].x, m_vCameras.at(m_iCurrentCamera)[2].y, m_vCameras.at(m_iCurrentCamera)[2].z));

	//Sets the matrix for the camera
	gl::UniformMatrix4fv(viewMatrixID, 1, gl::FALSE_, glm::value_ptr(ViewMatrix));
	gl::UniformMatrix4fv(projectionMatrixID, 1, gl::FALSE_, glm::value_ptr(m_PerspMatrix));

	for (int i = 0; i < m_vObjects.size(); i++) //For every model in the scene
	{
		//Transform and render the model
		m_vObjects.at(i)->update();
	}
	
	for (int i = 0; i < m_vCollectables.size(); i++) //For every collectable
	{
		if (!m_vCollectables.at(i)->Collected) //If the collectable has been collected
		{
			if (m_vCollectables.at(i)->Colliding(m_vRobots.at(0)->getPosition()))//If collision between robot and Collectables occurs
			{
				m_iCollectableCount++;					   //Increment collectable count
				std::cout << m_iCollectableCount << "\n"; //Output the amount of Collectables collected
			}
		}
	}
	
	for (int i = 0; i < m_vObjects.size(); i++) //For every model in the scene
	{
		m_vObjects.at(i)->render();
	}
}

void Scene::load(std::string dir)
{
	std::fstream Scenefile(dir, std::ios_base::in); //Attempt to open the file given

	if (!Scenefile.is_open()) //If the file didnt open
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
	glm::mat3 TempCamera;
	bool TextureFound = false;
	std::string TextureDirectory;
	std::string ObjDirectory;
	glm::vec3 tempRotate;
	glm::vec3 tempScale;
	glm::vec3 tempTranslate;
	//std::vector<GameObject*> NewObjects;
	std::vector<GameObject*> newObjects;
	std::vector<Model*> NewModels;

	int iCurrentObject;
	int iCurrentPart;
	bool isCollectable = false;
	int pos = 0;
	bool LoadingModel = false; //States whether a model is being loaded
	bool LoadingObject = false;
	bool loadingRobot = false;

	//While there are lines to be read
	while (getline(Scenefile, line))
	{
		std::istringstream iss(line); //Holds the data that is read
		token = "";

		isCollectable = false;

		//While the token is not the "push" token
		while (token != "p") //(p = Push onto vector)
		{
			iss >> token; // read to first whitespace

			if (token == "m") //If its an OBJ directory
			{
				iss >> iCurrentObject;
				
				if (iCurrentObject >= m_vObjects.size())
				{
					m_vObjects.push_back(new GameObject);
				}

				LoadingObject = true;
			}

			else if (token == "coll") //If its an OBJ directory
			{
				iss >> iCurrentObject;
				if (iCurrentObject >= m_vObjects.size())
				{
					Collectable* newCollect = new Collectable;
					m_vObjects.push_back(newCollect);
					m_vCollectables.push_back(newCollect);
					newCollect = NULL;
				}
				LoadingObject = true;
			}

			else if (token == "b") //If its an OBJ directory
			{
				iss >> iCurrentObject;
				if (iCurrentObject >= m_vObjects.size())
				{
					Robot* newBot = new Robot;
					m_vObjects.push_back(newBot);
					m_vRobots.push_back(newBot);
					newBot = NULL;
				}
				LoadingObject = true;
			}

			else if (token == "prt") //If its an OBJ directory
			{
				iss >> iCurrentPart;
				if (LoadingObject)
				{
					
					if (m_vObjects.at(iCurrentObject)->m_vParts.size() >= iCurrentPart)
					{
						m_vObjects.at(iCurrentObject)->m_vParts.push_back(new Model(m_uiProgramHandle));
					}
					
					LoadingModel = true;
				}
			}

			else if (token == "obj") //If its an OBJ directory
			{
				iss >> ObjDirectory; //Read the item
				if (LoadingModel)
				{
					m_vObjects.at(iCurrentObject)->m_vParts.at(iCurrentPart)->loadObj(ObjDirectory);
				}
				
			}
			else if (token == "tex") //If its a texture
			{
				iss >> TextureDirectory; //Read the item
				if (LoadingModel)
				{
					m_vObjects.at(iCurrentObject)->m_vParts.at(iCurrentPart)->setTexture(TextureDirectory);
				}
			}

			else if (token == "t") //If its a translation transform
			{
				iss >> TempTranslationVect.x; //Read the x value
				iss >> TempTranslationVect.y; //Read the y value
				iss >> TempTranslationVect.z; //Read the z value
				if (LoadingModel)
				{
					m_vObjects.at(iCurrentObject)->m_vParts.at(iCurrentPart)->setPosition(TempTranslationVect);
				}
			}
			else if (token == "s") //If its a scale transform
			{
				iss >> TempScaleVect.x; //Read the x value
				iss >> TempScaleVect.y; //Read the y value
				iss >> TempScaleVect.z; //Read the z value
				if (LoadingModel)
				{
					m_vObjects.at(iCurrentObject)->m_vParts.at(iCurrentPart)->setScale(TempScaleVect);
				}
			}
			else if (token == "r") //If its a rotation transform
			{
				iss >> TempRotationVect.x; //Read the x value
				iss >> TempRotationVect.y; //Read the y value
				iss >> TempRotationVect.z; //Read the z value
				if (LoadingModel)
				{
					m_vObjects.at(iCurrentObject)->m_vParts.at(iCurrentPart)->setRotation(TempRotationVect);
				}
			}

			else if (token == "c") //If its a texture
			{
				 m_vbUsePlayerPos.push_back(std::vector<bool>{false, false, false, 
														 false, false, false});
				for (int i = 0; i < 6; i++)
				{
					iss >> token;
					if (token == "p")
					{
						 m_vbUsePlayerPos[ m_vbUsePlayerPos.size() - 1][i] = true;
					}
					else
					{
						if (i <3)
							TempCamera[0][i] = stoi(token); //Read the item
						else
							TempCamera[1][i - 3] = stoi(token);  //Read the item
					}
				}
				
				iss >> TempCamera[2].x; //Read the item
				iss >> TempCamera[2].y; //Read the item
				iss >> TempCamera[2].z; //Read the item
				

				m_vCameras.push_back(glm::mat3(TempCamera[0].x, TempCamera[0].y, TempCamera[0].z,
											TempCamera[1].x, TempCamera[1].y, TempCamera[1].z,
											TempCamera[2].x, TempCamera[2].y, TempCamera[2].z));
			}

			else if (token == "os") //If its an OBJ directory
			{
				if (LoadingObject)
				{
					iss >> tempScale.x; //Read the x value
					iss >> tempScale.y; //Read the y value
					iss >> tempScale.z; //Read the z value

					m_vObjects.at(iCurrentObject)->setScale(tempScale);
					
				}
			}

			else if (token == "or") //If its an OBJ directory
			{
				if (LoadingObject)
				{
					iss >> tempRotate.x; //Read the x value
					iss >> tempRotate.y; //Read the y value
					iss >> tempRotate.z; //Read the z value

					m_vObjects.at(iCurrentObject)->setRotation(tempRotate);
					
				}
			}

			else if (token == "ot") //If its an OBJ directory
			{
				if (LoadingObject)
				{
					iss >> tempTranslate.x; //Read the x value
					iss >> tempTranslate.y; //Read the y value
					iss >> tempTranslate.z; //Read the z value

					m_vObjects.at(iCurrentObject)->setPosition(tempTranslate);
					
				}
			}
		}
		

		if (LoadingModel)
		{
			LoadingModel = false;
		}
		if (LoadingObject)
		{
			LoadingObject = false;
		}
	}
	Scenefile.close(); //Closes the file

	for (int i = 0; i < m_vRobots.size(); i++)
	{
		m_vRobots.at(i)->Setup();
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
	m_iCurrentCamera += Direction;
	if (m_iCurrentCamera < 0)
	{
		m_iCurrentCamera = m_vCameras.size() - 1;
	}
	if (m_iCurrentCamera >= m_vCameras.size())
	{
		m_iCurrentCamera = 0;
	}
}