#pragma once
#include <vector>
#include <GL/glew.h>
#include "Vector3.h"
#include "Matrix4.h"
class Obstacles
{
public:
	std::vector<Vector3> geometry;
	struct obstacle_data_t
	{
		float position[3];
		float radius;
		float velocity[4];
	};


	GLuint renderProgram;
	GLuint computeProgram;
	GLuint renderVAO;

	GLuint obstacleSSBO;

	GLuint mvpLoc;
	GLuint vpLoc;
	GLuint mLoc;

	unsigned static const int NUMSIDES = 32;
	unsigned static const int OBSTACLESCOUNT = 16;
	Matrix4 model = Matrix4::Scale(Vector3(1, 1, 1));

	void Render(double dt, Matrix4 vp);

	Obstacles();
	~Obstacles();
};

