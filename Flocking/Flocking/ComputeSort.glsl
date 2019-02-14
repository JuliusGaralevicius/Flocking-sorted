#version 450 core
layout (local_size_x = 256) in;

layout (binding = 0) uniform atomic_uint prefixSum[256];

struct Boid
{
	vec4 Vel;
	vec4 Pos;
};
layout (binding = 1) buffer inBoids
{
	Boid bIN[];
};
layout (binding = 2) buffer outBoids
{
	Boid bOUT[];
};
uint getCell(vec2 pos)
{
	uint cellSize = 32;
	uint cellCount = 16;
	return uint(pos.y/cellSize)*cellCount + uint(pos.x/cellSize);
}
void main()
{
	uint myCell = getCell(bIN[gl_GlobalInvocationID.x].Pos.xy);
	uint sortedID = atomicCounterIncrement(prefixSum[myCell]);
	Boid me = bIN[gl_GlobalInvocationID.x];
	memoryBarrier();
	barrier();
	bOUT[sortedID] = me;
}