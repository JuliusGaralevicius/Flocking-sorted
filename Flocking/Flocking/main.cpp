// GLEW
#define GLEW_STATIC
#include <GL/glew.h>
// GLFW
#include <GLFW/glfw3.h>
#include "Vector3.h"
#include "Matrix4.h"
#include <iostream>
#include <fstream>
#include <cstring>
#include <vector>
#include "Boid.h"
#include "Shader.h"
#include <string>
#include "DebugShape.h"

using namespace std;

// Window dimensions
GLuint WIDTH = 1680, HEIGHT = 1050;

unsigned int currentShader = 0;

void initglfw();
void initglew();
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
Vector3 cameraTranslate(0, 0, 0);
Vector4 lastClick(256, 256, 0, 0);

Matrix4 view = Matrix4::Translation(Vector3(-256, -256, -1400));
Matrix4 projection = Matrix4::Perspective(30.0, 3000.0f, WIDTH/ HEIGHT, 45.0f);


Matrix4 vp = projection * view;

int main()
{
	initglfw();
	GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Title", NULL, NULL);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetKeyCallback(window, key_callback);
	glfwMakeContextCurrent(window);
	glViewport(0, 0, WIDTH, HEIGHT);

	initglew();

	Boid* b = new Boid();
	DebugShape *quad = new DebugShape();
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	double previousTime= glfwGetTime();
	double currentTime = glfwGetTime();
	double dt = 0;

	glfwSwapInterval(1);
	for (unsigned long int i = 0; !glfwWindowShouldClose(window); i++) {
		currentTime= glfwGetTime();
		dt = currentTime - previousTime;
		previousTime = currentTime;
		glfwPollEvents();
		glfwSwapBuffers(window);
		glClear(GL_COLOR_BUFFER_BIT);

		quad->render(vp, dt);
		b->render(dt, lastClick, vp);
	}

	glfwTerminate();
}
void initglfw() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

}
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		double xpos, ypos;
		//getting cursor position
		glfwGetCursorPos(window, &xpos, &ypos);
		xpos = (xpos / WIDTH)*2-1;
		ypos = -((ypos / HEIGHT)*2-1);
		lastClick = Vector4(xpos*150, ypos*150, 0, 0);
	}
}
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_W && action == GLFW_PRESS)
	{
		cameraTranslate.z = 0;
		cameraTranslate.z = 1.0;
	}
	if (key == GLFW_KEY_S && action == GLFW_PRESS)
	{
		cameraTranslate.z = 0;
		cameraTranslate.z = -1.0;
	}
}
void initglew() {
	GLenum error = glewInit();
}