#version 450 core
layout (local_size_x = 256) in;

layout (binding = 0) uniform atomic_uint count[256];
layout (binding = 1) buffer bufferName
{
	uint sum[256];
};

shared uint shared0[256];
shared uint shared1[256];

void main()
{
	int myID = int(gl_LocalInvocationID.x);
	shared0[myID] = atomicCounter(count[myID]);
	int neighbour;
	
	for (int i = 0; i<=log2(256); i++)
	{
		barrier();
		neighbour = int(pow(2, i));
		if (mod(i, 2) == 0)
		{
			if (myID-neighbour >= 0)
				shared1[myID] = shared0[myID] + shared0[myID-neighbour];
			else 
				shared1[myID] = shared0[myID];
		}
		else
		{
			if (myID-neighbour >= 0)
				shared0[myID] = shared1[myID] + shared1[myID-neighbour];
			else 
				shared0[myID] = shared1[myID];
		}
	}
	
	barrier();
	sum[myID+1] = shared1[myID];
	if (myID == 0)
	{
		sum[0] = 0;
	}
}