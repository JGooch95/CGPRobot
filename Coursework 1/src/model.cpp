#include "model.h"
#include "Bitmap.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

#define GLM_FORCE_RADIANS

Model::Model(GLuint uiProgramHandle2)
{
	m_uiProgramHandle = uiProgramHandle2;
}

void Model::loadObj(std::string sDir)
{
	std::fstream modelfile(sDir, std::ios_base::in);

	//If the file doesn't load output an error
	if (!modelfile.is_open())
	{
		std::cerr << "File " << sDir << " not found.";
		DebugBreak();
		throw std::invalid_argument("File not found");
		return;	//ERROR!!!
	}

	std::vector<glm::vec3> vertices; // Holds each individual type of vertex in the file
	std::vector<glm::vec2> uvCoords; // Holds each individual uv coordinate in the file
	std::vector<glm::vec3> vertNorms; // Holds each individual vertex normal in the file

	glm::vec3 dimHigh(0.0f, 0.0f, 0.0f); //Holds the highest x, y and z for the model dimensions
	glm::vec3 dimLow(0.0f, 0.0f, 0.0f);  //Holds the lowest x, y and z for the model dimensions

	std::string sLine; //Holds the line being read
	std::string sToken = ""; //Holds the part of the line being read

	m_Dimensions = glm::vec3(0.0f, 0.0f, 0.0f); //Resetting the dimensions vector
	glm::vec3 tempVect; //A vector holding temporary data

	while (getline(modelfile, sLine)) //While new lines are being read
	{
		std::istringstream iss(sLine);

		//Process the line
		sToken = "";
		iss >> sToken; // read to first whitespace

		if (sToken == "v") //If the line is a vertex position
		{
			iss >> tempVect.x;
			iss >> tempVect.y;
			iss >> tempVect.z;

			//Checks against the current dimensions and alters them if the vertices exceed the bounds
			if (tempVect.x > dimHigh.x) //Upper Bound
			{
				dimHigh.x = tempVect.x;
			}
			if (tempVect.x < dimLow.x) //Lower Bound
			{
				dimLow.x = tempVect.x;
			}

			//Checks against the current dimensions and alters them if the vertices exceed the bounds
			if (tempVect.y > dimHigh.y) //Upper Bound
			{
				dimHigh.y = tempVect.y;
			}
			if (tempVect.y < dimLow.y) //Lower Bound
			{
				dimLow.y = tempVect.y;
			}

			//Checks against the current dimensions and alters them if the vertices exceed the bounds
			if (tempVect.z > dimHigh.z) //Upper Bound
			{
				dimHigh.z = tempVect.y;
			}
			if (tempVect.z < dimLow.z) //Lower Bound
			{
				dimLow.z = tempVect.y;
			}

			vertices.push_back(tempVect); //Adds the vertex to the vertices
		}

		else if (sToken == "vt")  //If the line is a uv Coordinate 
		{
			iss >> tempVect.x;
			iss >> tempVect.y;
			uvCoords.push_back(glm::vec2(tempVect.x,tempVect.y)); //Adds the uvCoordinate to the uvCoordinates
		}
		else if (sToken == "vn")  //If the line is a vertex normal
		{
			iss >> tempVect.x;
			iss >> tempVect.y;
			iss >> tempVect.z;

			vertNorms.push_back(tempVect); //Adds the vertex normal to teh vertex normals
		}
		else if (sToken == "f") //If the line is a face
		{
			unsigned int uiValue; //Holds the value to be stored
			static const int s_ciForwardSlash = 0x2F; // "/" character

			for (int i = 0; i <3; i++)  //For every item
			{
				iss >> uiValue; //Reads the face index
				int iLookAhead = iss.peek(); // peek character

				m_vfPositionData.push_back(vertices.at(uiValue - 1).x); //Adds the x for the vertex at that index to the position data
				m_vfPositionData.push_back(vertices.at(uiValue - 1).y); //Adds the y for the vertex at that index to the position data
				m_vfPositionData.push_back(vertices.at(uiValue - 1).z); //Adds the z for the vertex at that index to the position data

				if (iLookAhead == s_ciForwardSlash) //If the next character is a "/"
				{
					iss.ignore(1); //Ignore it
					iss >> uiValue;
					m_vfUvData.push_back(uvCoords.at(uiValue - 1).x); //Adds the UV x position to the uv coordinates
					m_vfUvData.push_back(uvCoords.at(uiValue - 1).y); //Adds the UV y position to the uv coordinates

					if (iLookAhead == s_ciForwardSlash) //If the next character is a "/"
					{
						iss.ignore(1); //Ignore it

						iss >> uiValue; //Reads the face normal index
						m_vfNormalsData.push_back(vertNorms.at(uiValue - 1).x); //Adds the x for the vertex normal at that index to vertNormals
						m_vfNormalsData.push_back(vertNorms.at(uiValue - 1).y); //Adds the y for the vertex normal at that index to vertNormals
						m_vfNormalsData.push_back(vertNorms.at(uiValue - 1).z); //Adds the z for the vertex normal at that index to vertNormals
					}
				}
			}
		}
	}
	modelfile.close(); //Closes the file

	// Create and populate the buffer objects using separate buffers
	gl::GenBuffers(3, m_uiVBOHandles);

	GLuint uiPositionBufferHandle = m_uiVBOHandles[0];
	GLuint uiUvBufferHandle = m_uiVBOHandles[1];
	GLuint uiNormalsBufferHandle = m_uiVBOHandles[2];

	//Fills the vertex position buffers
	gl::BindBuffer(gl::ARRAY_BUFFER, uiPositionBufferHandle);
	gl::BufferData(gl::ARRAY_BUFFER, m_vfPositionData.size() * sizeof(float), &m_vfPositionData[0], gl::STATIC_DRAW);
	
	//Fills the UV Coordinate buffers
	gl::BindBuffer(gl::ARRAY_BUFFER, uiUvBufferHandle);
	gl::BufferData(gl::ARRAY_BUFFER, m_vfUvData.size() * sizeof(float), &m_vfUvData[0], gl::STATIC_DRAW);

	//Fills the vertex normal buffers
	gl::BindBuffer(gl::ARRAY_BUFFER, uiNormalsBufferHandle);
	gl::BufferData(gl::ARRAY_BUFFER, m_vfNormalsData.size() * sizeof(float), &m_vfNormalsData[0], gl::STATIC_DRAW);

	// Create and set-up the vertex array object
	gl::GenVertexArrays(1, &m_uiVAOHandle);
	gl::BindVertexArray(m_uiVAOHandle);

	gl::EnableVertexAttribArray(0);  // Vertex position
	gl::EnableVertexAttribArray(1);  // Vertex color
	gl::EnableVertexAttribArray(2);  // Vertex normals

	gl::BindBuffer(gl::ARRAY_BUFFER, uiPositionBufferHandle);
	gl::VertexAttribPointer(0, 3, gl::FLOAT, FALSE, 0, (GLubyte *)NULL);

	gl::BindBuffer(gl::ARRAY_BUFFER, uiUvBufferHandle);
	gl::VertexAttribPointer(1, 2, gl::FLOAT, FALSE, 0, (GLubyte *)NULL);

	gl::BindBuffer(gl::ARRAY_BUFFER, uiNormalsBufferHandle);
	gl::VertexAttribPointer(2, 3, gl::FLOAT, FALSE, 0, (GLubyte *)NULL);

	m_Dimensions = (dimHigh - dimLow); //Sets the dimensions to be the High - low
}

void Model::start()
{
	m_ModelMatrix = glm::mat4(1.0f); //Resets the model matrix
}

void Model::scale(glm::vec3 newScale)
{
	m_ModelMatrix = glm::scale(glm::mat4(1.0f), newScale) *  m_ModelMatrix; // Apply the scale to the model matrix
}

void Model::rotate(glm::vec3 newRotation, CoordinateType Coord)
{
	//Limits the X rotation to be between 0 and 360 
	if (newRotation.x >= 360.0f) //Upper bound
	{
		newRotation.x  -= 360;
	}
	else if (newRotation.x < 0.0f) //Lower bound
	{
		newRotation.x += 360.0f;
	}

	glm::mat4 rotMatrix = glm::rotate(glm::mat4(1.0f), newRotation.x, glm::vec3(1, 0, 0)); //Apply X transform to rotation matrix

	//Limits the Y rotation to be between 0 and 360 
	if (newRotation.y >= 360.0f) //Upper bound
	{
		newRotation.y -= 360.0f;
	}
	else if (newRotation.y < 0.0f) //Lower bound
	{
		newRotation.y += 360.0f;
	}

	rotMatrix *= glm::rotate(glm::mat4(1.0f), newRotation.y, glm::vec3(0, 1, 0)); //Apply Y transform to rotation matrix

	//Limits the Z rotation to be between 0 and 360 
	if (newRotation.z >= 360.0f) //Upper bound
	{
		newRotation.z -= 360.0f;
	}
	else if (newRotation.z < 0.0f) //Lower bound
	{
		newRotation.z += 360.0f;
	}

	rotMatrix *= glm::rotate(glm::mat4(1.0f), newRotation.z, glm::vec3(0, 0, 1)); //Apply Z transform to rotation matrix

	//If rotating around world coordinates
	if (Coord == WORLD_COORDS)
	{
		m_ModelMatrix *= rotMatrix; //Multiply model matrix by rotation
	}
	//If rotating around local coordinates
	else
	{
		m_ModelMatrix = rotMatrix *  m_ModelMatrix; //Multiply rotation by model matrix
	}
}

void Model::translate(glm::vec3 newPosition)
{
	m_ModelMatrix = glm::translate(glm::mat4(1.0f), newPosition) *  m_ModelMatrix; // Apply the translation to the model matrix
}


void Model::render(){
	//Links each variable to the shaders
	gl::UniformMatrix4fv(gl::GetUniformLocation(m_uiProgramHandle, "M"), 1, gl::FALSE_, glm::value_ptr(m_ModelMatrix));

	//Sends the material data to the shaders
	gl::Uniform1f(gl::GetUniformLocation(m_uiProgramHandle, "material.shininess"), 32.0f);
	//gl::Uniform1i(gl::GetUniformLocation(m_iProgramHandle, "material.diffuse"), m_Texture->object());
	//gl::Uniform1i(gl::GetUniformLocation(m_iProgramHandle, "material.specular"), m_Texture->object());

	gl::Enable(gl::DEPTH_TEST);
	
	//Set texture0 to the model's texture
	gl::ActiveTexture(gl::TEXTURE0);
	gl::BindTexture(gl::TEXTURE_2D, m_Texture->object());

	//Use this model's VAO and draw the model
	gl::BindVertexArray(m_uiVAOHandle);
	gl::DrawArrays(gl::TRIANGLES, 0, m_vfPositionData.size() / 3);

	gl::BindTexture(gl::TEXTURE_2D, NULL); //Unbind the Texture
	gl::BindVertexArray(NULL); //Unbind the VAO
}

glm::vec3 Model::getRotation()
{
	return m_Rotation;
}

glm::vec3 Model::getPosition()
{
	return m_Position;
}
glm::vec3 Model::getScale()
{
	return m_Scale;
}

glm::vec3 Model::getDimensions()
{
	return m_Dimensions;
}

void Model::setPosition(glm::vec3 newVect)
{
	m_Position = newVect;
}

void Model::setRotation(glm::vec3 newVect)
{
	m_Rotation = newVect;
}
void Model::setScale(glm::vec3 newVect)
{
	m_Scale = newVect;
}

void Model::setTexture(std::string newTexture)
{
	if (m_Texture != NULL) //If a texture is already in the location
	{
		delete(m_Texture); //Delete the texture
		m_Texture = NULL;  //Remove the pointer
	}

	//Load the texture
	Bitmap bmp = Bitmap::bitmapFromFile(newTexture);
	bmp.flipVertically();
	m_Texture = new Texture(bmp);

	//Set texture
	gl::ActiveTexture(gl::TEXTURE0);
	gl::BindTexture(gl::TEXTURE_2D, m_Texture->object());
	GLint loc = gl::GetUniformLocation(m_uiProgramHandle, "tex");
	gl::Uniform1f(loc, 0);
}


Model::~Model()
{	
	//Deletes the texture and clears the pointer
	delete(m_Texture);
	m_Texture = NULL;
}

