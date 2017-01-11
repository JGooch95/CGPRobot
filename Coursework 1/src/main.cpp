#include <gl_core_4_3.hpp>
#include <glfw3.h>
#include <iostream>
#include "scene.h"

GLFWwindow *window;
Scene *scene1;

void init()
{
	gl::ClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	scene1 = new Scene;
	scene1->init();
	scene1->load("Assets/Scene1.txt");
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_W && action == GLFW_REPEAT)
		if (scene1)
			scene1->moveRobot(1.0f);

	if (key == GLFW_KEY_S && action == GLFW_REPEAT)
		if (scene1)
			scene1->moveRobot(-1.0f);

	if (key == GLFW_KEY_A && action == GLFW_REPEAT)
		if (scene1)
			scene1->turnRobot(1.0f);

	if (key == GLFW_KEY_D && action == GLFW_REPEAT)
		if (scene1)
			scene1->turnRobot(-1.0f);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
		scene1->switchCamera(+1);
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
		scene1->switchCamera(-1);
}

void gameLoop()
{
	while (!glfwWindowShouldClose(window) && !glfwGetKey(window, GLFW_KEY_ESCAPE))
	{
		scene1->update();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}

void main()
{
	//If glfw didnt initialise exit with a failure
	if (!glfwInit()) exit(EXIT_FAILURE);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, FALSE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, TRUE);

	window = glfwCreateWindow(1280, 720, "Robot CW", NULL, NULL);

	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, key_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);

	gl::exts::LoadTest GLLoaded = gl::sys::LoadFunctions();
	

	if (!GLLoaded) {
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	init();
	gameLoop();

	glfwTerminate();
	exit(EXIT_FAILURE);
}