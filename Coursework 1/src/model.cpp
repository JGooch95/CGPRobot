#include "model.h"
#include "Bitmap.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

#define GLM_FORCE_RADIANS

Model::Model(std::string Obj, glm::vec3 Translate2, glm::vec3 Rotate2, glm::vec3 Scale2, std::string Tex, GLint programHandle2) {
	programHandle = programHandle2;
	TextureDir = Tex;
	Position = Translate2;
	Rotation = Rotate2;
	CurrentScale = Scale2;
	loadObj(Obj);
	init();
}

void Model::init(){
	// Create and populate the buffer objects using separate buffers
	gl::GenBuffers(2, vboHandles);

	GLuint positionBufferHandle = vboHandles[0];
	GLuint uvBufferHandle = vboHandles[1];

	gl::BindBuffer(gl::ARRAY_BUFFER, positionBufferHandle);
	gl::BufferData(gl::ARRAY_BUFFER, positionData.size() * sizeof(float), &positionData[0], gl::STATIC_DRAW);

	gl::BindBuffer(gl::ARRAY_BUFFER, uvBufferHandle);
	gl::BufferData(gl::ARRAY_BUFFER, uvData.size() * sizeof(float), &uvData[0], gl::STATIC_DRAW);

	// Create and set-up the vertex array object
	gl::GenVertexArrays(1, &vaoHandle);
	gl::BindVertexArray(vaoHandle);

	gl::EnableVertexAttribArray(0);  // Vertex position
	gl::EnableVertexAttribArray(1);  // Vertex color

	gl::BindBuffer(gl::ARRAY_BUFFER, positionBufferHandle);
	gl::VertexAttribPointer(0, 3, gl::FLOAT, FALSE, 0, (GLubyte *)NULL);

	gl::BindBuffer(gl::ARRAY_BUFFER, uvBufferHandle);
	gl::VertexAttribPointer(1, 2, gl::FLOAT, FALSE, 0, (GLubyte *)NULL);

	//Load the texture
	Bitmap bmp = Bitmap::bitmapFromFile(TextureDir);
	bmp.flipVertically();
	gTexture = new Texture(bmp);

	//Set texture
	gl::ActiveTexture(gl::TEXTURE0);
	gl::BindTexture(gl::TEXTURE_2D, gTexture->object());
	GLint loc = gl::GetUniformLocation(programHandle, "tex");
	gl::Uniform1f(loc, 0);

}

void Model::update(){
}

void Model::start()
{
	//The model matrix
	M = glm::mat4(1.0f);
}

void Model::end()
{
	//Links each variable to the matrixID value.
	GLint modelMatrixID = gl::GetUniformLocation(programHandle, "mModel");

	//The matrixId is then passed the values to be applied to the variables. 
	gl::UniformMatrix4fv(modelMatrixID, 1, gl::FALSE_, glm::value_ptr(M));
}
void Model::Scale(float x, float y, float z)
{
	M = glm::scale(glm::mat4(1.0f), glm::vec3(x, y, z))* M;
}

void Model::WRotate(float x, float y, float z)
{
	//The rotation matrix applied to the points
	//X rotation
	if (x >= 360)
	{
		x -= 360;
	}
	else if (x < 0)
	{
		x += 360;
	}

	//Y rotation
	if (y >= 360)
	{
		y -= 360;
	}
	else if (y < 0)
	{
		y += 360;
	}


	//Z Rotation
	if (z >= 360)
	{
		z -= 360;
	}
	else if (z < 0)
	{
		z += 360;
	}


	glm::mat4 Rotate = glm::rotate(glm::mat4(1.0f), x, glm::vec3(1, 0, 0));
	Rotate *= glm::rotate(glm::mat4(1.0f), y, glm::vec3(0, 1, 0));
	Rotate *= glm::rotate(glm::mat4(1.0f), z, glm::vec3(0, 0, 1));
	M = M*Rotate;
}

void Model::LRotate(float x, float y, float z)
{
	//The rotation matrix applied to the points
	//X rotation
	if (x >= 360)
	{
		x -= 360;
	}
	else if (x < 0)
	{
		x += 360;
	}

	//Y rotation
	if (y >= 360)
	{
		y -= 360;
	}
	else if (y < 0)
	{
		y += 360;
	}


	//Z Rotation
	if (z >= 360)
	{
		z -= 360;
	}
	else if (z < 0)
	{
		z += 360;
	}


	glm::mat4 Rotate = glm::rotate(glm::mat4(1.0f), x, glm::vec3(1, 0, 0));
	Rotate *= glm::rotate(glm::mat4(1.0f), y, glm::vec3(0, 1, 0));
	Rotate *= glm::rotate(glm::mat4(1.0f), z, glm::vec3(0, 0, 1));
	M = Rotate*M;
}

void Model::Translate(float x, float y, float z)
{
	//The translation matrix applied to the points
	M = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z)) * M;
	//M *= glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z));
}
void Model::LTranslate(float x, float y, float z)
{
	//The translation matrix applied to the points
	M = M * glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z));
	//M *= glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z));
}

void Model::render(){
	gl::Enable(gl::DEPTH_TEST);

	gl::ActiveTexture(gl::TEXTURE0);
	gl::BindTexture(gl::TEXTURE_2D, gTexture->object());

	gl::BindVertexArray(vaoHandle);
	gl::DrawArrays(gl::TRIANGLES, 0, positionData.size() / 3);
	gl::BindVertexArray(NULL);
}

void Model::loadObj(std::string dir)
{
	std::fstream modelfile(dir, std::ios_base::in);

	if (!modelfile.is_open())
	{
		std::cerr << "File " << dir << " not found.";
		DebugBreak();
		throw std::invalid_argument("File not found");
		return;	//ERROR!!!
	}

	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvCoords;
	glm::vec3 DimHigh(0,0,0);
	glm::vec3 DimLow(0,0,0);
	std::string line;
	std::string token = "";
	Dimensions = glm::vec3(0, 0, 0);
	while (getline(modelfile, line))
	{
		std::istringstream iss(line);

		// process the line
		token = "";

		iss >> token; // read to first whitespace

		if (token == "g")
			iss >> token; // read the model name

		else if (token == "v")
		{
			//Vertex position
			glm::vec3 Vert;

			iss >> Vert.x;
			iss >> Vert.y;
			iss >> Vert.z;

			vertices.push_back(Vert);
		}

		else if (token == "vt")
		{
			//UV coordinates
			glm::vec2 UV;
			
			iss >> UV.x;
			iss >> UV.y;
			uvCoords.push_back(UV);
		}

		else if (token == "f")
		{
			//Triangles
			unsigned int value; //Holds the value to be stored
			static const int forwardSlash = 0x2F; // "/" character

												  //For every item
			for (int i = 0; i <3; i++)
			{
				iss >> value; //Reads the face index
				int lookAhead = iss.peek();  	// peek character

				positionData.push_back(vertices.at(value - 1).x);

				if (positionData.at(positionData.size()-1) > DimHigh.x)
				{
					DimHigh.x = positionData.at(positionData.size()-1);
				}
				if (positionData.at(positionData.size() - 1) < DimLow.x)
				{
					DimLow.x = positionData.at(positionData.size() - 1);
				}

				positionData.push_back(vertices.at(value - 1).y);

				if (positionData.at(positionData.size()-1) > DimHigh.y)
				{
					DimHigh.y = positionData.at(positionData.size()-1);
				}
				if (positionData.at(positionData.size() - 1) < DimLow.y)
				{
					DimLow.y = positionData.at(positionData.size() - 1);
				}

				positionData.push_back(vertices.at(value - 1).z);

				if (positionData.at(positionData.size()-1) > DimHigh.z)
				{
					DimHigh.z = positionData.at(positionData.size()-1);
				}
				if (positionData.at(positionData.size() - 1) < DimLow.z)
				{
					DimLow.z = positionData.at(positionData.size() - 1);
				}

				if (lookAhead == forwardSlash)    //If the next character is a "/"
				{
					iss.ignore(1); //Ignore it
					iss >> value;
					uvData.push_back(uvCoords.at(value - 1).x);
					uvData.push_back(uvCoords.at(value - 1).y);

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


	Dimensions = (DimHigh - DimLow) * CurrentScale;
}
