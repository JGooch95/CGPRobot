#include "UIText.h"
#include <iostream>

UIText::UIText()
{
	if (FT_Init_FreeType(&ft))
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
	if (FT_New_Face(ft, "Assets/fonts/arial.ttf", 0, &face))
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;

	FT_Set_Pixel_Sizes(face, 0, 25);
}

void UIText::loadCharacters()
{
	gl::PixelStorei(gl::UNPACK_ALIGNMENT, 1); // Disable byte-alignment restriction

	for (GLubyte c = 0; c < 128; c++)
	{
		// Load character glyph 
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
			continue;
		}
		// Generate texture
		GLuint texture;
		gl::GenTextures(1, &texture);
		gl::BindTexture(gl::TEXTURE_2D, texture);
		gl::TexImage2D(
			gl::TEXTURE_2D,
			0,
			gl::RED,
			face->glyph->bitmap.width,
			face->glyph->bitmap.rows,
			0,
			gl::RED,
			gl::UNSIGNED_BYTE,
			face->glyph->bitmap.buffer
		);
		// Set texture options
		gl::TexParameteri(gl::TEXTURE_2D, gl::TEXTURE_WRAP_S, gl::CLAMP_TO_EDGE);
		gl::TexParameteri(gl::TEXTURE_2D, gl::TEXTURE_WRAP_T, gl::CLAMP_TO_EDGE);
		gl::TexParameteri(gl::TEXTURE_2D, gl::TEXTURE_MIN_FILTER, gl::LINEAR);
		gl::TexParameteri(gl::TEXTURE_2D, gl::TEXTURE_MAG_FILTER, gl::LINEAR);
		// Now store character for later use
		Character character = {
			texture,
			glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			face->glyph->advance.x
		};
		m_characters.insert(std::pair<GLchar, Character>(c, character));
	}
	FT_Done_Face(face);
	FT_Done_FreeType(ft);
}

void UIText::setupBuffers()
{
	gl::GenVertexArrays(1, &VAO);
	gl::GenBuffers(1, &VBO);
	gl::BindVertexArray(VAO);
	gl::BindBuffer(gl::ARRAY_BUFFER, VBO);
	gl::BufferData(gl::ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, gl::DYNAMIC_DRAW);
	gl::EnableVertexAttribArray(0);
	gl::VertexAttribPointer(0, 4, gl::FLOAT, 0, 4 * sizeof(GLfloat), 0);
	gl::BindBuffer(gl::ARRAY_BUFFER, 0);
	gl::BindVertexArray(0);
}

void UIText::render(GLuint uiProgramHandle, std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color)
{
	// Activate corresponding render state	
	gl::Uniform3f(gl::GetUniformLocation(uiProgramHandle, "textColor"), color.x, color.y, color.z);
	gl::ActiveTexture(gl::TEXTURE0);
	gl::BindVertexArray(VAO);

	// Iterate through all characters
	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++)
	{
		Character ch = m_characters[*c];

		GLfloat xpos = x + ch.bearing.x * scale;
		GLfloat ypos = y - (ch.size.y - ch.bearing.y) * scale;

		GLfloat w = ch.size.x * scale;
		GLfloat h = ch.size.y * scale;
		// Update VBO for each character
		GLfloat vertices[6][4] = {
			{ xpos,     ypos + h,   0.0, 0.0 },
			{ xpos,     ypos,       0.0, 1.0 },
			{ xpos + w, ypos,       1.0, 1.0 },

			{ xpos,     ypos + h,   0.0, 0.0 },
			{ xpos + w, ypos,       1.0, 1.0 },
			{ xpos + w, ypos + h,   1.0, 0.0 }
		};
		// Render glyph texture over quad
		gl::BindTexture(gl::TEXTURE_2D, ch.textureID);
		// Update content of VBO memory
		gl::BindBuffer(gl::ARRAY_BUFFER, VBO);
		gl::BufferSubData(gl::ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		gl::BindBuffer(gl::ARRAY_BUFFER, 0);
		// Render quad
		gl::DrawArrays(gl::TRIANGLES, 0, 6);
		// Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		x += (ch.advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64)
	}
	gl::BindVertexArray(0);
	gl::BindTexture(gl::TEXTURE_2D, 0);
}

UIText::~UIText()
{
	
}