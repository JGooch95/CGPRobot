#pragma once
#include <gl_core_4_3.hpp>
#include <glm\glm.hpp>
#include <map>
#include <ft2build.h>
#include FT_FREETYPE_H


struct Character {
	GLuint textureID;
	glm::ivec2 size;
	glm::ivec2 bearing;
	GLuint advance;
};

class UIText
{
	private:
		FT_Library ft;
		FT_Face face;
		GLuint VAO;
		GLuint VBO;
		std::map<GLchar, Character> m_characters;

	public:
		UIText();
		void loadCharacters();
		void setupBuffers();
		void render(GLuint uiProgramHandle, std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color);
		~UIText();
};