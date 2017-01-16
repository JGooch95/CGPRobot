#include "HUD.h"
#include "glm\glm.hpp"


HUD::HUD(GLuint uiProgramHandle2)
{
	m_uiProgramHandle = uiProgramHandle2;
}

void HUD::render()
{
	//Links each variable to the shaders
	gl::UniformMatrix4fv(gl::GetUniformLocation(m_uiProgramHandle, "M"), 1, gl::FALSE_, glm::value_ptr(m_ModelMatrix));
	gl::UniformMatrix4fv(gl::GetUniformLocation(m_uiProgramHandle, "V"), 1, gl::FALSE_, glm::value_ptr(glm::mat4(1)));

	gl::Disable(gl::DEPTH_TEST);

	//Set texture0 to the model's texture
	gl::ActiveTexture(gl::TEXTURE0);
	gl::BindTexture(gl::TEXTURE_2D, m_Texture->object());

	//Use this model's VAO and draw the model
	gl::BindVertexArray(m_uiVAOHandle);
	gl::DrawArrays(gl::TRIANGLES, 0, m_vfPositionData.size() / 3);

	gl::BindTexture(gl::TEXTURE_2D, NULL); //Unbind the Texture
	gl::BindVertexArray(NULL); //Unbind the VAO
}

void HUD::update()
{
	start();
	scale(m_Scale);
	rotate(m_Rotation, LOCAL_COORDS);
	translate(m_Position);
}