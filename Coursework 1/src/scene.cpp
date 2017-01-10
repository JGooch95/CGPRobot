#include "scene.h"
#include <fstream>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Scene::Scene()
{

}

void Scene::Init()
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
	P = glm::perspective(glm::radians(60.0f), 1280.0f/720.0f, 0.1f, 1000.0f);
	CurrentCamera = 0;
}

void Scene::linkMe(GLint vertShader, GLint fragShader) {
	// Create the program object
	programHandle = gl::CreateProgram();
	if (0 == programHandle) {
		fprintf(stderr, "Error creating program object.\n");
		exit(1);
	}

	// Attach the shaders to the program object
	gl::AttachShader(programHandle, vertShader);
	gl::AttachShader(programHandle, fragShader);

	// Link the program
	gl::LinkProgram(programHandle);

	// Check for successful linking
	GLint status;
	gl::GetProgramiv(programHandle, gl::LINK_STATUS, &status);
	if (FALSE == status) {

		fprintf(stderr, "Failed to link shader program!\n");

		GLint logLen;
		gl::GetProgramiv(programHandle, gl::INFO_LOG_LENGTH, &logLen);

		if (logLen > 0) {
			char * log = (char *)malloc(logLen);

			GLsizei written;
			gl::GetProgramInfoLog(programHandle, logLen, &written, log);

			fprintf(stderr, "Program log: \n%s", log);

			free(log);
		}
	}
	else {
		gl::UseProgram(programHandle);
	}
	Bot1.Setup("Assets/Robot1.txt", programHandle);
	CollectableCount = 0;
}

void Scene::Update()
{
	gl::Clear(gl::COLOR_BUFFER_BIT | gl::DEPTH_BUFFER_BIT);

	GLint viewMatrixID = gl::GetUniformLocation(programHandle, "mView");
	GLint projectionMatrixID = gl::GetUniformLocation(programHandle, "mProjection");

	for (int i = 0; i < usePlayerPos.at(CurrentCamera).size(); i++)
	{
		if (usePlayerPos.at(CurrentCamera).at(i))
		{
			if (i == 0)
			{
				Cameras.at(CurrentCamera)[0].x = Bot1.Position.x;
			}
			else if (i == 1)
			{
				Cameras.at(CurrentCamera)[0].y = Bot1.Position.y;
			}
			else if (i == 2)
			{
				Cameras.at(CurrentCamera)[0].z = Bot1.Position.z;
			}

			else if (i == 3)
			{
				Cameras.at(CurrentCamera)[1].x = Bot1.Position.x;
			}
			else if (i == 4)
			{
				Cameras.at(CurrentCamera)[1].y = Bot1.Position.y;
			}
			else if (i == 5)
			{
				Cameras.at(CurrentCamera)[1].z = Bot1.Position.z;
			}
		}
	}
	glm::mat4 V = glm::lookAt(glm::vec3(Cameras.at(CurrentCamera)[0].x, Cameras.at(CurrentCamera)[0].y, Cameras.at(CurrentCamera)[0].z),
							  glm::vec3(Cameras.at(CurrentCamera)[1].x, Cameras.at(CurrentCamera)[1].y, Cameras.at(CurrentCamera)[1].z),
							  glm::vec3(Cameras.at(CurrentCamera)[2].x, Cameras.at(CurrentCamera)[2].y, Cameras.at(CurrentCamera)[2].z));

	//Sets the matrix for the camera
	gl::UniformMatrix4fv(viewMatrixID, 1, gl::FALSE_, glm::value_ptr(V));
	gl::UniformMatrix4fv(projectionMatrixID, 1, gl::FALSE_, glm::value_ptr(P));

	for (int i = 0; i < Models.size(); i++)
	{
		Models.at(i)->update();

		Models.at(i)->start();
			Models.at(i)->scale(Models.at(i)->getCurrentScale().x, Models.at(i)->getCurrentScale().y, Models.at(i)->getCurrentScale().z);
			Models.at(i)->translate(0, Models.at(i)->getDimensions().y / 2, 0);
			Models.at(i)->rotate(Models.at(i)->getRotation().x, Models.at(i)->getRotation().y, Models.at(i)->getRotation().z, WORLD_COORDS);
			Models.at(i)->translate(Models.at(i)->getPosition().x, Models.at(i)->getPosition().y, Models.at(i)->getPosition().z);
		Models.at(i)->end();

		Models.at(i)->render();
	}

	for (int i = 0; i < Collectables.size(); i++)
	{
		if (!Collectables.at(i)->Collected)
		{
			Collectables.at(i)->Move();
			if (Collectables.at(i)->Colliding(Bot1.Position))
			{
				CollectableCount++;
				std::cout << CollectableCount;
			}
		}
	}

	Bot1.Render();
}

void Scene::Load(std::string dir)
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
	bool isCollectable = false;
	int pos = 0;
	bool LoadingModel = false; //States whether a model is being loaded

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

			if (token == "o") //If its an OBJ directory
			{
				iss >> ObjDirectory; //Read the item
				LoadingModel = true;
			}
			if (token == "t") //If its a translation transform
			{
				iss >> TempTranslationVect.x; //Read the x value
				iss >> TempTranslationVect.y; //Read the y value
				iss >> TempTranslationVect.z; //Read the z value
			}
			else if (token == "s") //If its a scale transform
			{
				iss >> TempScaleVect.x; //Read the x value
				iss >> TempScaleVect.y; //Read the y value
				iss >> TempScaleVect.z; //Read the z value
			}
			else if (token == "r") //If its a rotation transform
			{
				iss >> TempRotationVect.x; //Read the x value
				iss >> TempRotationVect.y; //Read the y value
				iss >> TempRotationVect.z; //Read the z value
			}
			else if (token == "tex") //If its a texture
			{
				iss >> TextureDirectory; //Read the item
			}

			else if (token == "c") //If its a texture
			{
				usePlayerPos.push_back(std::vector<bool>{false, false, false, 
														 false, false, false});
				for (int i = 0; i < 6; i++)
				{
					iss >> token;
					if (token == "p")
					{
						usePlayerPos[usePlayerPos.size() - 1][i] = true;
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
				

				Cameras.push_back(glm::mat3(TempCamera[0].x, TempCamera[0].y, TempCamera[0].z,
											TempCamera[1].x, TempCamera[1].y, TempCamera[1].z,
											TempCamera[2].x, TempCamera[2].y, TempCamera[2].z));
			}

			if (token == "coll") //If its an OBJ directory
			{
				iss >> token; //Read the item
				if (token == "true")
				{
					isCollectable = true;
				}
				if (token == "false")
				{
					isCollectable = false;
				}
			}
		}

		if (LoadingModel)
		{
			if (isCollectable)
				Collectables.push_back(new Collectable(new Model(ObjDirectory, TempTranslationVect, TempRotationVect, TempScaleVect, TextureDirectory, programHandle)));
			else
				Models.push_back(new Model(ObjDirectory, TempTranslationVect, TempRotationVect, TempScaleVect, TextureDirectory, programHandle)); //Pushes the model onto the vector

			LoadingModel = false;
		}
	}
	Scenefile.close(); //Closes the file
}

void Scene::MoveRobot(float Direction)
{
	Bot1.Move(Direction);
}
void Scene::TurnRobot(float Direction)
{
	Bot1.Turn(Direction);
}
void Scene::SwitchCamera(int Direction)
{
	CurrentCamera += Direction;
	if (CurrentCamera < 0)
	{
		CurrentCamera = Cameras.size() - 1;
	}
	if (CurrentCamera >= Cameras.size())
	{
		CurrentCamera = 0;
	}
}