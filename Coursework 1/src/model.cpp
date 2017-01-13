#include "model.h"
#include "Bitmap.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

#define GLM_FORCE_RADIANS

Model::Model(GLint programHandle2)
{
	m_iProgramHandle = programHandle2;
}

void Model::start()
{
	//The model matrix
	m_ModelMatrix = glm::mat4(1.0f);
}

void Model::scale(glm::vec3 newScale)
{
	m_ModelMatrix = glm::scale(glm::mat4(1.0f), newScale)*  m_ModelMatrix; // Apply the scale to the model matrix
}

void Model::rotate(glm::vec3 newRotation, CoordinateType Coord)
{
	//X rotation 
	if (newRotation.x >= 360) //Upper bound
	{
		newRotation.x  -= 360;
	}
	else if (newRotation.x < 0) //Lower bound
	{
		newRotation.x += 360;
	}

	glm::mat4 rotMatrix = glm::rotate(glm::mat4(1.0f), newRotation.x, glm::vec3(1, 0, 0)); //Apply X transform to rotation matrix

	//Y rotation
	if (newRotation.y >= 360) //Upper bound
	{
		newRotation.y -= 360;
	}
	else if (newRotation.y < 0) //Lower bound
	{
		newRotation.y += 360;
	}

	rotMatrix *= glm::rotate(glm::mat4(1.0f), newRotation.y, glm::vec3(0, 1, 0)); //Apply Y transform to rotation matrix

	//Z Rotation
	if (newRotation.z >= 360) //Upper bound
	{
		newRotation.z -= 360;
	}
	else if (newRotation.z < 0) //Lower bound
	{
		newRotation.z += 360;
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
	gl::UniformMatrix4fv(gl::GetUniformLocation(m_iProgramHandle, "M"), 1, gl::FALSE_, glm::value_ptr(m_ModelMatrix));
	
	//Sends the material data to the shaders
	//gl::Uniform1i(gl::GetUniformLocation(m_iProgramHandle, "material.diffuse"), m_Texture->object());
	//gl::Uniform1i(gl::GetUniformLocation(m_iProgramHandle, "material.specular"), m_Texture->object());
	gl::Uniform1f(gl::GetUniformLocation(m_iProgramHandle, "material.shininess"), 32.0f);


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

	glm::vec3 dimHigh(0,0,0); //Holds the highest x, y and z for the model dimensions
	glm::vec3 dimLow(0,0,0);  //Holds the lowest x, y and z for the model dimensions

	std::string line; //Holds the line being read
	std::string token = ""; //Holds the part of the line being read

	m_Dimensions = glm::vec3(0, 0, 0); //Resetting the dimensions vector

	while (getline(modelfile, line))
	{
		std::istringstream iss(line);

		// process the line
		token = "";

		iss >> token; // read to first whitespace

		if (token == "v") //If the line is a vertex
		{
			//Vertex position
			glm::vec3 Vert;

			iss >> Vert.x;
			iss >> Vert.y;
			iss >> Vert.z;

			//Checks against the current dimensions and alters them if the vertices exceed the bounds
			if (Vert.x > dimHigh.x) //Upper Bound
			{
				dimHigh.x = Vert.x;
			}
			if (Vert.x < dimLow.x) //Lower Bound
			{
				dimLow.x = Vert.x;
			}

			//Checks against the current dimensions and alters them if the vertices exceed the bounds
			if (Vert.y > dimHigh.y) //Upper Bound
			{
				dimHigh.y = Vert.y;
			}
			if (Vert.y < dimLow.y) //Lower Bound
			{
				dimLow.y = Vert.y;
			}

			//Checks against the current dimensions and alters them if the vertices exceed the bounds
			if (Vert.z > dimHigh.z) //Upper Bound
			{
				dimHigh.z = Vert.y;
			}
			if (Vert.z < dimLow.z) //Lower Bound
			{
				dimLow.z = Vert.y;
			}

			vertices.push_back(Vert);
		}

		else if (token == "vt")  //If the line is a uv Coordinate 
		{
			//UV coordinates
			glm::vec2 UV;
			
			iss >> UV.x;
			iss >> UV.y;
			uvCoords.push_back(UV);
		}
		else if (token == "vn")  //If the line is a vertex normal
		{
			glm::vec3 norm;
			iss >> norm.x;
			iss >> norm.y;
			iss >> norm.z;

			vertNorms.push_back(norm);
		}
		else if (token == "f") //If the line is a face
		{
			//Triangles
			unsigned int value; //Holds the value to be stored
			static const int forwardSlash = 0x2F; // "/" character

												  //For every item
			for (int i = 0; i <3; i++)
			{
				iss >> value; //Reads the face index
				int lookAhead = iss.peek();  	// peek character

				m_vfPositionData.push_back(vertices.at(value - 1).x); //Adds the x for the vertex at that index to the position data
				m_vfPositionData.push_back(vertices.at(value - 1).y); //Adds the y for the vertex at that index to the position data
				m_vfPositionData.push_back(vertices.at(value - 1).z); //Adds the z for the vertex at that index to the position data

				if (lookAhead == forwardSlash)    //If the next character is a "/"
				{
					iss.ignore(1); //Ignore it
					iss >> value;
					m_vfUvData.push_back(uvCoords.at(value - 1).x);
					m_vfUvData.push_back(uvCoords.at(value - 1).y);

					if (lookAhead == forwardSlash) //If the next character is a "/"
					{
						iss.ignore(1); //Ignore it

						iss >> value; //Reads the face normal index
						m_vfNormalsData.push_back(vertNorms.at(value - 1).x); //Adds the x for the vertex at that index to the position data
						m_vfNormalsData.push_back(vertNorms.at(value - 1).y); //Adds the y for the vertex at that index to the position data
						m_vfNormalsData.push_back(vertNorms.at(value - 1).z); //Adds the z for the vertex at that index to the position data
					}
				}
			}
		}
	}
	modelfile.close();
	
	// Create and populate the buffer objects using separate buffers
	gl::GenBuffers(3, m_uiVBOHandles);

	GLuint positionBufferHandle = m_uiVBOHandles[0];
	GLuint uvBufferHandle = m_uiVBOHandles[1];
	GLuint normalsBufferHandle = m_uiVBOHandles[2];

	gl::BindBuffer(gl::ARRAY_BUFFER, positionBufferHandle);
	gl::BufferData(gl::ARRAY_BUFFER, m_vfPositionData.size() * sizeof(float), &m_vfPositionData[0], gl::STATIC_DRAW);

	gl::BindBuffer(gl::ARRAY_BUFFER, uvBufferHandle);
	gl::BufferData(gl::ARRAY_BUFFER, m_vfUvData.size() * sizeof(float), &m_vfUvData[0], gl::STATIC_DRAW);

	gl::BindBuffer(gl::ARRAY_BUFFER, normalsBufferHandle);
	gl::BufferData(gl::ARRAY_BUFFER, m_vfNormalsData.size() * sizeof(float), &m_vfNormalsData[0], gl::STATIC_DRAW);

	// Create and set-up the vertex array object
	gl::GenVertexArrays(1, &m_uiVAOHandle);
	gl::BindVertexArray(m_uiVAOHandle);

	gl::EnableVertexAttribArray(0);  // Vertex position
	gl::EnableVertexAttribArray(1);  // Vertex color
	gl::EnableVertexAttribArray(2);  // Vertex normals

	gl::BindBuffer(gl::ARRAY_BUFFER, positionBufferHandle);
	gl::VertexAttribPointer(0, 3, gl::FLOAT, FALSE, 0, (GLubyte *)NULL);

	gl::BindBuffer(gl::ARRAY_BUFFER, uvBufferHandle);
	gl::VertexAttribPointer(1, 2, gl::FLOAT, FALSE, 0, (GLubyte *)NULL);

	gl::BindBuffer(gl::ARRAY_BUFFER, normalsBufferHandle);
	gl::VertexAttribPointer(2, 3, gl::FLOAT, FALSE, 0, (GLubyte *)NULL);

	m_Dimensions = (dimHigh - dimLow); //Sets the dimensions to be the High - low
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
	GLint loc = gl::GetUniformLocation(m_iProgramHandle, "tex");
	gl::Uniform1f(loc, 0);
}


Model::~Model()
{	
	//Deletes the texture and clears the pointer
	delete(m_Texture);
	m_Texture = NULL;
}

