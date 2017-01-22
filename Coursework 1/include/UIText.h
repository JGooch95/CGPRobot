#pragma once
#include <gl_core_4_3.hpp>
#include <glm\glm.hpp>
#include <map>
#include <ft2build.h>
#include FT_FREETYPE_H


struct Character {
	GLuint iTextureID; //Holds the ID for the texture
	glm::ivec2 size; //Holds the width and height of the character
	glm::ivec2 bearing; //Holds the distance from the axes
	GLuint uiAdvance; //Holds the overall space taken in the x axis
};

class UIText
{
	private:
		FT_Library m_Ft; //Holds freetype
		FT_Face m_Face; //Holds the typeface of the text
		GLuint m_iVAO; //The vertex arrray holding the object
		GLuint m_iVBO; //The vbo arrray holding the object
		std::map<GLchar, Character> m_cCharacters; //Holds all of the character textures

	public:
		UIText(); //Constructor
		void loadCharacters(); //Loads all of the characters into the map
		void setupBuffers(); //Sets up the buffers
		void render(GLuint uiProgramHandle, std::string sText, GLfloat fX, GLfloat fY, GLfloat fScale, glm::vec3 color); //Renders the text
		~UIText(); //Destructor
};