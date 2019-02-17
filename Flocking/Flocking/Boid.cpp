#include "Boid.h"
#include <time.h>
#include "Shader.h"
#include <GLFW/glfw3.h>
Boid::Boid()
{
	Shader *cShader = new Shader("ComputeTest.glsl");
	Shader *rShader = new Shader("VertexShader.glsl", "FragShader.glsl", "");
	Shader *hashShader = new Shader("ComputeHash.glsl");
	Shader *scanShader = new Shader("ComputeScan.glsl");
	Shader *sortShader = new Shader("ComputeSort.glsl");


	computeScan = scanShader->getID();
	computeHash = hashShader->getID();
	IDShader = rShader->getID();
	computePositions = cShader->getID();
	computeSort = sortShader->getID();

	goalLoc = glGetUniformLocation(computePositions, "goal");
	dtLoc = glGetUniformLocation(computePositions, "dt");
	mvpLoc = glGetUniformLocation(IDShader, "mvp");


	boid_data_t* boidData = new boid_data_t[FLOCKSIZE];
	// initiate positions
	srand(time(0));
	for (int i = 0; i < FLOCKSIZE; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			boidData[i].velocity[j] = (static_cast<float>(rand()) / static_cast <float> (RAND_MAX)) * 512;
			boidData[i].position[j] = (static_cast<float>(rand()) / static_cast <float> (RAND_MAX))* 512;
		}
		boidData[i].velocity[2] = 0;
		boidData[i].position[2] = 0;
		boidData[i].velocity[3] = 0;
		boidData[i].position[3] = 0;
	}
	// initiate positions

	// setup BOID SSBO
	glGenBuffers(2,  ssboBoids);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboBoids[0]);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(boid_data_t)*FLOCKSIZE, boidData, GL_DYNAMIC_COPY);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboBoids[1]);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(boid_data_t)*FLOCKSIZE, NULL, GL_DYNAMIC_COPY);

	// setup sum SSBO
	glGenBuffers(1, &sumSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, sumSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(GLuint)*CELLCOUNTDIMENSION*CELLCOUNTDIMENSION, NULL, GL_DYNAMIC_COPY);

	// generate atomic counter
	glGenBuffers(1, &atomicCounter);
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, atomicCounter);
	glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint) * CELLCOUNTDIMENSION*CELLCOUNTDIMENSION, NULL, GL_DYNAMIC_COPY);
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);


	// setup geometry VBO
	GLuint posBuff;
	glGenBuffers(1, &posBuff);
	glBindBuffer(GL_ARRAY_BUFFER, posBuff);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vector4)*positions.size(), positions.data(), GL_STATIC_DRAW);
	// setup geometry VBO

	// setup VAO for rendering
	glGenVertexArrays(2, renderVAO);
	for (int i = 0; i < 2; i++)
	{
		glBindVertexArray(renderVAO[i]);

		glBindBuffer(GL_ARRAY_BUFFER, posBuff);
		glVertexAttribPointer((GLuint)0, 4, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ARRAY_BUFFER, ssboBoids[i]);

		glVertexAttribPointer((GLuint)3, 4, GL_FLOAT, GL_FALSE, sizeof(boid_data_t), 0);
		glVertexAttribPointer((GLuint)2, 4, GL_FLOAT, GL_FALSE, sizeof(boid_data_t), (void*)(sizeof(Vector4)));

		glVertexAttribDivisor(2, 1);
		glVertexAttribDivisor(3, 1);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(2);
		glEnableVertexAttribArray(3);
	}

	glBindVertexArray(0);
}
void Boid::render(double deltaTime, Vector4 goal, Matrix4 vp)
{
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, atomicCounter);
	glBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint) * 256, emptyArr);

	bool DEBUG =false;
	// Compute hash for each Boid
	glUseProgram(computeHash);
	glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, atomicCounter);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ssboBoids[odd]);
	
	glDispatchCompute(FLOCKSIZE / 256, 1, 1);
	glMemoryBarrier(GL_COMPUTE_SHADER_BIT);
	// Compute hash for each Boid

	if (DEBUG)
	{
		glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, atomicCounter);
		GLuint *ptr;
		ptr = (GLuint*)glMapBuffer(GL_ATOMIC_COUNTER_BUFFER, GL_READ_ONLY);
		glUnmapBuffer(GL_ATOMIC_COUNTER_BUFFER);
		int sum = 0;
		
		for (int i = 0; i < CELLCOUNTDIMENSION*CELLCOUNTDIMENSION; i++)
		{
			std::cout << " " << ptr[i];
			sum += ptr[i];
			if ((i+1 )% 10 == 0)
				std::cout << std::endl;
		}
		std::cout << std::endl << std::endl;
	}


	// Calculate prefix sum
	glUseProgram(computeScan);

	glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, atomicCounter);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, sumSSBO);

	glDispatchCompute(1, 1, 1);
	glMemoryBarrier(GL_COMPUTE_SHADER_BIT);

	// Calculate prefix sum
	if (DEBUG)
	{
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, sumSSBO);
		GLuint *pt = (GLuint*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
		glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
		for (int i = 0; i < CELLCOUNTDIMENSION*CELLCOUNTDIMENSION; i++)
		{
			std::cout << " " << pt[i];
			if ((i + 1) % 10 == 0)
				std::cout << std::endl;
		}	
		int n;
		std::cout << std::endl << std::endl;
	}

	// sort boids
	glUseProgram(computeSort);
	glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, sumSSBO);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ssboBoids[odd]);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, ssboBoids[!odd]);

	glDispatchCompute(FLOCKSIZE/256, 1, 1);

	glMemoryBarrier(GL_COMPUTE_SHADER_BIT);

	// sort boids
	if (DEBUG)
	{
		glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, sumSSBO);
		GLuint *pt = (GLuint*)glMapBuffer(GL_ATOMIC_COUNTER_BUFFER, GL_READ_ONLY);
		glUnmapBuffer(GL_ATOMIC_COUNTER_BUFFER);
		for (int i = 0; i < CELLCOUNTDIMENSION*CELLCOUNTDIMENSION; i++)
		{
			std::cout << " " << pt[i];
			if ((i + 1) % 10 == 0)
				std::cout << std::endl;
		}
		int n;

		//std::cin >> n;
		std::cout << std::endl << "-----------------------------------------" << std::endl;
	}

	// calculate new positions
	glUseProgram(computePositions);

	glUniform1f(dtLoc, deltaTime);
	glUniform4f(goalLoc, goal.x, goal.y, goal.z, goal.w);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssboBoids[!odd]);
	glDispatchCompute(64, 1, 1);
	glMemoryBarrier(GL_COMPUTE_SHADER_BIT);
	// render
	glUseProgram(IDShader);
	glBindVertexArray(renderVAO[!odd]);


	Matrix4 mvp = model * vp;
	glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, mvp.values);
	glDrawArraysInstanced(GL_TRIANGLES, 0, positions.size(), FLOCKSIZE);
	glBindVertexArray(0);

	odd = !odd;
}

Boid::~Boid()
{
}
