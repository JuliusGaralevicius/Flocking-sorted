#pragma once
#include <vector>
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4.h"
#include <GL/glew.h>
class Obstacles;
class Boid
{
	
	// layout in the shader
	enum buffers {
		POSITION = 0,
		COLOUR = 1,
		NORMAL = 2,
		MAX_BUFFER = 3
	};
private:
	std::vector<Vector4> colours = {
		Vector4(0.5f, 0.5f, 0.5f, 1.0f),
		Vector4(0.5f, 0.5f, 0.5f, 1.0f),
		Vector4(0.5f, 0.5f, 0.5f, 1.0f),
		Vector4(0.5f, 0.5f, 0.5f, 1.0f),
		Vector4(0.5f, 0.5f, 0.5f, 1.0f),
		Vector4(0.5f, 0.5f, 0.5f, 1.0f),
		Vector4(0.5f, 0.5f, 0.5f, 1.0f),

		Vector4(0.5f, 0.5f, 0.5f, 1.0f),
		Vector4(0.5f, 0.5f, 0.5f, 1.0f),
		Vector4(0.5f, 0.5f, 0.5f, 1.0f),
		Vector4(0.5f, 0.5f, 0.5f, 1.0f),
		Vector4(0.5f, 0.5f, 0.5f, 1.0f),
		Vector4(0.5f, 0.5f, 0.5f, 1.0f),
		Vector4(0.5f, 0.5f, 0.5f, 1.0f),

		Vector4(0.5f, 0.5f, 0.5f, 1.0f),
		Vector4(0.5f, 0.5f, 0.5f, 1.0f),
		Vector4(0.5f, 0.5f, 0.5f, 1.0f),
		Vector4(0.5f, 0.5f, 0.5f, 1.0f),
		Vector4(0.5f, 0.5f, 0.5f, 1.0f),
		Vector4(0.5f, 0.5f, 0.5f, 1.0f),
		Vector4(0.5f, 0.5f, 0.5f, 1.0f),

		Vector4(0.5f, 0.5f, 0.5f, 1.0f),
		Vector4(0.5f, 0.5f, 0.5f, 1.0f),
		Vector4(0.5f, 0.5f, 0.5f, 1.0f),
		Vector4(0.5f, 0.5f, 0.5f, 1.0f),
		Vector4(0.5f, 0.5f, 0.5f, 1.0f),
		Vector4(0.5f, 0.5f, 0.5f, 1.0f),
		Vector4(0.5f, 0.5f, 0.5f, 1.0f),

		Vector4(0.5f, 0.5f, 0.5f, 1.0f),
		Vector4(0.5f, 0.5f, 0.5f, 1.0f),
		Vector4(0.5f, 0.5f, 0.5f, 1.0f),
		Vector4(0.5f, 0.5f, 0.5f, 1.0f),
		Vector4(0.5f, 0.5f, 0.5f, 1.0f),
		Vector4(0.5f, 0.5f, 0.5f, 1.0f),
		Vector4(0.5f, 0.5f, 0.5f, 1.0f),

		Vector4(0.5f, 0.5f, 0.5f, 1.0f),
		Vector4(0.5f, 0.5f, 0.5f, 1.0f),
		Vector4(0.5f, 0.5f, 0.5f, 1.0f),
		Vector4(0.5f, 0.5f, 0.5f, 1.0f),
		Vector4(0.5f, 0.5f, 0.5f, 1.0f),
		Vector4(0.5f, 0.5f, 0.5f, 1.0f),
		Vector4(0.5f, 0.5f, 0.5f, 1.0f)
	};
	std::vector<Vector4> positions = {
		Vector4(1, -1, -1, 1.0f),
		Vector4(-1, -1, -1, 1.0f),
		Vector4(0, 0, 1, 1.0f),

		Vector4(-1, -1, -1, 1.0f),
		Vector4(-1, -1, 1, 1.0f),
		Vector4(0, 0, 1, 1.0f),

		Vector4(-1, -1, 1, 1.0f),
		Vector4(1, -1, 1, 1.0f),
		Vector4(0, 0, 1, 1.0f),

		Vector4(1, -1, 1, 1.0f),
		Vector4(1, -1, -1, 1.0f),
		Vector4(0, 0, 1, 1.0f),

		Vector4(1, -1, -1, 1.0f),
		Vector4(-1, -1, -1, 1.0f),
		Vector4(-1, -1, 1, 1.0f),

		Vector4(-1, -1, 1, 1.0f),
		Vector4(1, -1, 1, 1.0f),
		Vector4(1, -1, -1, 1.0f)
	};

	unsigned static const int FLOCKSIZE = 16384*2;

	unsigned static const int CELLCOUNTDIMENSION = 16; // ^2 = total cells
	struct boid_data_t
	{
		float velocity[4];
		float position[4];
	};
	GLuint renderVAO[2];
	GLuint sortVAO;
	GLuint ssboBoids[2];
	GLuint sumSSBO;
	GLuint sumSSBOOG;
	GLuint obstacleSSBO;
	GLuint atomicCounter;
	Obstacles* obs;

	bool odd = false;

	GLuint computePositions;
	GLuint computeHash;
	GLuint computeScan;
	GLuint computeSort;

	GLuint IDShader;
	GLuint mvpLoc;
	GLuint goalLoc;
	GLuint dtLoc;
	
public:
	
	GLuint emptyArr[256] = { 0 };
	Matrix4 model = Matrix4::Scale(Vector3(1, 1, 1));
	Boid(Obstacles* obs);
	void render(double deltaTime, Vector4 goal, Matrix4 vp);
	std::vector<Vector4> getPositions() { return positions; };
	GLuint getRenderProgram() { return IDShader; };
	~Boid();
};