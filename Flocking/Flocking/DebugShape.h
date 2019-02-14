#pragma once
#include <vector>
#include "Vector4.h"
#include "Matrix4.h"
#include <GL/glew.h>
class DebugShape
{
public:
	DebugShape();
	~DebugShape();
	std::vector<Vector4> positions =
	{
		Vector4(-1, -1, 0, 1),
		Vector4(-1, 1, 0, 1),
		Vector4(1, -1, 0, 1),
		Vector4(1, 1, 0, 1)
	};
	void render(Matrix4 mvp, float dt);

	Matrix4 model = Matrix4::Translation(Vector3(256, 256, 0))*Matrix4::Scale(Vector3(256, 256, 1));

	GLuint posVBO;
	GLuint vao;
	GLuint renderProgram;
	GLuint mvpLoc;
};
