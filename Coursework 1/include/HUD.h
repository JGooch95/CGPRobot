#pragma once
#include "Model.h"

class HUD : public Model
{
	public:
		HUD(GLuint uiProgramHandle2);
		void render();
		void update();
};