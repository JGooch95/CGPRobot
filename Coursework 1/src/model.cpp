#include "model.h"
#include "Bitmap.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

#define GLM_FORCE_RADIANS

Model::Model(std::string Obj, glm::vec3 Translate2, glm::vec3 Rotate2, glm::vec3 Scale2, std::string sTex, GLint programHandle2) {
	m_iProgramHandle = programHandle2;
	Position = Translate2;
	Rotation = Rotate2;
	CurrentScale = Scale2;
	loadObj(Obj);
	init(sTex);
}

void Model::init(std::string sTex){
	// Create and populate the buffer objects using separate buffers
	gl::GenBuffers(2, m_uiVBOHandles);

	GLuint positionBufferHandle = m_uiVBOHandles[0];
	GLuint uvBufferHandle = m_uiVBOHandles[1];

	gl::BindBuffer(gl::ARRAY_BUFFER, positionBufferHandle);
	gl::BufferData(gl::ARRAY_BUFFER, m_vfPositionData.size() * sizeof(float), &m_vfPositionData[0], gl::STATIC_DRAW);

	gl::BindBuffer(gl::ARRAY_BUFFER, uvBufferHandle);
	gl::BufferData(gl::ARRAY_BUFFER, m_vfUvData.size() * sizeof(float), &m_vfUvData[0], gl::STATIC_DRAW);

	// Create and set-up the vertex array object
	gl::GenVertexArrays(1, &m_uiVAOHandle);
	gl::BindVertexArray(m_uiVAOHandle);

	gl::EnableVertexAttribArray(0);  // Vertex position
	gl::EnableVertexAttribArray(1);  // Vertex color

	gl::BindBuffer(gl::ARRAY_BUFFER, positionBufferHandle);
	gl::VertexAttribPointer(0, 3, gl::FLOAT, FALSE, 0, (GLubyte *)NULL);

	gl::BindBuffer(gl::ARRAY_BUFFER, uvBufferHandle);
	gl::VertexAttribPointer(1, 2, gl::FLOAT, FALSE, 0, (GLubyte *)NULL);

	//Load the texture
	Bitmap bmp = Bitmap::bitmapFromFile(sTex);
	bmp.flipVertically();
	m_Texture = new Texture(bmp);

	//Set texture
	gl::ActiveTexture(gl::TEXTURE0);
	gl::BindTexture(gl::TEXTURE_2D, m_Texture->object());
	GLint loc = gl::GetUniformLocation(m_iProgramHandle, "tex");
	gl::Uniform1f(loc, 0);

}

void Model::update(){
}

void Model::start()
{
	//The model matrix
	m_ModelMatrix = glm::mat4(1.0f);
}

void Model::end()
{
	//Links each variable to the matrixID value.
	GLint modelMatrixID = gl::GetUniformLocation(m_iProgramHandle, "mModel");

	//The matrixId is then passed the values to be applied to the variables. 
	gl::UniformMatrix4fv(modelMatrixID, 1, gl::FALSE_, glm::value_ptr(m_ModelMatrix));
}

void Model::scale(float fX, float fY, float fZ)
{
	m_ModelMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(fX, fY, fZ))*  m_ModelMatrix; // Apply the scale to the model matrix
}

void Model::rotate(float fX, float fY, float fZ, CoordinateType Coord)
{
	//X rotation 
	if (fX >= 360) //Upper bound
	{
		fX -= 360;
	}
	else if (fX < 0) //Lower bound
	{
		fX += 360;
	}

	glm::mat4 rotMatrix = glm::rotate(glm::mat4(1.0f), fX, glm::vec3(1, 0, 0));

	//Y rotation
	if (fY >= 360) //Upper bound
	{
		fY -= 360;
	}
	else if (fY < 0) //Lower bound
	{
		fY += 360;
	}

	rotMatrix *= glm::rotate(glm::mat4(1.0f), fY, glm::vec3(0, 1, 0));

	//Z Rotation
	if (fZ >= 360) //Upper bound
	{
		fZ -= 360;
	}
	else if (fZ < 0) //Lower bound
	{
		fZ += 360;
	}

	rotMatrix *= glm::rotate(glm::mat4(1.0f), fZ, glm::vec3(0, 0, 1));

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



void Model::translate(float fX, float fY, float fZ)
{
	m_ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(fX, fY, fZ)) *  m_ModelMatrix; // Apply the translation to the model matrix
}


void Model::render(){
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

	std::vector<glm::vec3> vertices; //Holds each individual type of vertex in the file
	std::vector<glm::vec2> uvCoords; // Holds each individual uv coordinate in the file

	glm::vec3 dimHigh(0,0,0); //Holds the highest x, y and z for the model dimensions
	glm::vec3 dimLow(0,0,0);  //Holds the lowest x, y and z for the model dimensions

	std::string line; //Holds the line being read
	std::string token = ""; //Holds the part of the line being read

	Dimensions = glm::vec3(0, 0, 0); //Resetting the dimensions vector

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

				//Checks against the current dimensions and alters them if the vertices exceed the bounds
				if (m_vfPositionData.at(m_vfPositionData.size()-1) > dimHigh.x) //Upper Bound
				{
					dimHigh.x = m_vfPositionData.at(m_vfPositionData.size()-1);
				}
				if (m_vfPositionData.at(m_vfPositionData.size() - 1) < dimLow.x) //Lower Bound
				{
					dimLow.x = m_vfPositionData.at(m_vfPositionData.size() - 1);
				}

				m_vfPositionData.push_back(vertices.at(value - 1).y); //Adds the y for the vertex at that index to the position data

				//Checks against the current dimensions and alters them if the vertices exceed the bounds
				if (m_vfPositionData.at(m_vfPositionData.size()-1) > dimHigh.y) //Upper Bound
				{
					dimHigh.y = m_vfPositionData.at(m_vfPositionData.size()-1);
				}
				if (m_vfPositionData.at(m_vfPositionData.size() - 1) < dimLow.y) //Lower Bound
				{
					dimLow.y = m_vfPositionData.at(m_vfPositionData.size() - 1);
				}

				m_vfPositionData.push_back(vertices.at(value - 1).z); //Adds the z for the vertex at that index to the position data

				//Checks against the current dimensions and alters them if the vertices exceed the bounds
				if (m_vfPositionData.at(m_vfPositionData.size()-1) > dimHigh.z) //Upper Bound
				{
					dimHigh.z = m_vfPositionData.at(m_vfPositionData.size()-1);
				}
				if (m_vfPositionData.at(m_vfPositionData.size() - 1) < dimLow.z) //Lower Bound
				{
					dimLow.z = m_vfPositionData.at(m_vfPositionData.size() - 1);
				}

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
					}

					else //If the next character is not a "/"
					{
						iss >> value; //Reads the face texture index

						iss.ignore(1); //Ignore the slash

						iss >> value; //Reads the face normal index
					}
				}
			}
		}
	}
	modelfile.close();


	Dimensions = (dimHigh - dimLow) * CurrentScale;
}

glm::vec3 Model::getRotation()
{
	return Rotation;
}

glm::vec3 Model::getCurrentScale()
{
	return CurrentScale;
}

glm::vec3 Model::getPosition()
{
	return Position;
}

glm::vec3 Model::getDimensions()
{
	return Dimensions;
}

void Model::setCurrentScale(glm::vec3 newVect)
{
	CurrentScale = newVect;
}

void Model::setPosition(glm::vec3 newVect)
{
	Position = newVect;
}

void Model::setRotation(glm::vec3 newVect)
{
	Rotation = newVect;
}
