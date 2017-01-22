#include "UIText.h"
#include <iostream>

UIText::UIText()
{
	//Loads freetype
	if (FT_Init_FreeType(&m_Ft))
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl; //If it fails outputs a failure message

	//Loads the arial font
	if (FT_New_Face(m_Ft, "Assets/fonts/arial.ttf", 0, &m_Face))
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;  //If it fails outputs a failure message

	FT_Set_Pixel_Sizes(m_Face, 0, 25); //Sets the freetype pixel sizes
}

void UIText::loadCharacters()
{
	gl::PixelStorei(gl::UNPACK_ALIGNMENT, 1); // Disable byte-alignment restriction

	for (GLubyte c = 0; c < 128; c++) //For every character
	{
		// Load character glyph 
		if (FT_Load_Char(m_Face, c, FT_LOAD_RENDER))
		{
			std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
			continue;
		}

		// Generate texture
		GLuint iTexture;
		gl::GenTextures(1, &iTexture);
		gl::BindTexture(gl::TEXTURE_2D, iTexture);
		gl::TexImage2D(
			gl::TEXTURE_2D,
			0,
			gl::RED,
			m_Face->glyph->bitmap.width,
			m_Face->glyph->bitmap.rows,
			0,
			gl::RED,
			gl::UNSIGNED_BYTE,
			m_Face->glyph->bitmap.buffer
		);
		
		// Set texture options
		gl::TexParameteri(gl::TEXTURE_2D, gl::TEXTURE_WRAP_S, gl::CLAMP_TO_EDGE);
		gl::TexParameteri(gl::TEXTURE_2D, gl::TEXTURE_WRAP_T, gl::CLAMP_TO_EDGE);
		gl::TexParameteri(gl::TEXTURE_2D, gl::TEXTURE_MIN_FILTER, gl::LINEAR);
		gl::TexParameteri(gl::TEXTURE_2D, gl::TEXTURE_MAG_FILTER, gl::LINEAR);
		
		// Store character for later use
		Character character = {
			iTexture,
			glm::ivec2(m_Face->glyph->bitmap.width, m_Face->glyph->bitmap.rows),
			glm::ivec2(m_Face->glyph->bitmap_left, m_Face->glyph->bitmap_top),
			m_Face->glyph->advance.x
		};
		m_cCharacters.insert(std::pair<GLchar, Character>(c, character));
	}

	FT_Done_Face(m_Face);
	FT_Done_FreeType(m_Ft);
}

void UIText::setupBuffers()
{
	gl::GenBuffers(1, &m_iVBO); //Generates a vbo for the text

	//Fills the buffer holding the quad positions
	gl::BindBuffer(gl::ARRAY_BUFFER, m_iVBO);
	gl::BufferData(gl::ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, gl::DYNAMIC_DRAW);

	//Generates a vao
	gl::GenVertexArrays(1, &m_iVAO);
	gl::BindVertexArray(m_iVAO);

	gl::EnableVertexAttribArray(0); //Quad positions
	gl::VertexAttribPointer(0, 4, gl::FLOAT, 0, 4 * sizeof(GLfloat), 0);

	gl::BindBuffer(gl::ARRAY_BUFFER, 0);
	gl::BindVertexArray(0);
}

void UIText::render(GLuint uiProgramHandle, std::string sText, GLfloat fX, GLfloat fY, GLfloat fScale, glm::vec3 color)
{
	// Activate corresponding render state	
	gl::Uniform3f(gl::GetUniformLocation(uiProgramHandle, "textColor"), color.x, color.y, color.z);
	gl::ActiveTexture(gl::TEXTURE0);
	gl::BindVertexArray(m_iVAO);

	// Iterate through all characters
	std::string::const_iterator c;
	for (c = sText.begin(); c != sText.end(); c++)
	{
		Character ch = m_cCharacters[*c]; //Sets the character to be the matching character in the map

		GLfloat fXpos = fX + ch.bearing.x * fScale;
		GLfloat fYpos = fY - (ch.size.y - ch.bearing.y) * fScale;

		GLfloat fWidth = ch.size.x * fScale;
		GLfloat fHeight = ch.size.y * fScale;

		// Update VBO for each character
		GLfloat afVertices[6][4] = {
			{ fXpos			 , fYpos + fHeight,   0.0, 0.0 },
			{ fXpos			 , fYpos,			  0.0, 1.0 },
			{ fXpos + fWidth , fYpos,			  1.0, 1.0 },

			{ fXpos			, fYpos + fHeight,   0.0, 0.0 },
			{ fXpos + fWidth, fYpos,			 1.0, 1.0 },
			{ fXpos + fWidth, fYpos + fHeight,   1.0, 0.0 }
		};

		// Render glyph texture over quad
		gl::BindTexture(gl::TEXTURE_2D, ch.iTextureID);

		// Update content of VBO memory
		gl::BindBuffer(gl::ARRAY_BUFFER, m_iVBO);
		gl::BufferSubData(gl::ARRAY_BUFFER, 0, sizeof(afVertices), afVertices);
		gl::BindBuffer(gl::ARRAY_BUFFER, 0);

		// Render quad
		gl::DrawArrays(gl::TRIANGLES, 0, 6);

		// sets the next character to be further to the right
		fX += (ch.uiAdvance / 64) * fScale; 
	}
	gl::BindVertexArray(0);
	gl::BindTexture(gl::TEXTURE_2D, 0);
}

UIText::~UIText()
{
	
}