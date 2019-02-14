#version 450 core
layout (local_size_x = 256) in;
struct Boid
{
	vec4 Vel;
	vec4 Pos;
};
layout (binding = 0) buffer inBoids
{
	Boid bIN[32768];
};

uniform float dt = 0.0f;
uniform vec4 goal = vec4(0.0f);
uniform float speed = 80.0f;

float groupRadius = 5;
float avoidRadius = 2;

void main()
{
	uint myID = gl_GlobalInvocationID.x;
	Boid me = bIN[myID];
	vec3 avoid = vec3(0.0);
	vec3 group = vec3(0.0);
	vec3 align = vec3(0.0);
	vec3 home = (goal-me.Pos).xyz;
	home = home*dot(home, home)*0.000000002;
	
	int avoided = 0;
	int grouped = 0;
	int aligned = 0;
	
	
	uint range = 1000;
	uint start=0;
	uint end=16384*2;
	if (int(myID)-range>0)
		start = myID-range;
	if (int(myID)+range<end)
		end = myID+range;
	
	
	for (uint i = 0; i<16384*2; i++)
	{
		Boid other = bIN[i];
		// avoid
		vec4 distance = other.Pos - me.Pos;
		if (dot(distance, distance)<avoidRadius*avoidRadius)
		{
			avoid-=distance.xyz;
			avoided++;
		}
		// group
		if (dot(distance, distance)<groupRadius*groupRadius)
		{
			group+=distance.xyz;
			grouped++;
			align+=other.Vel.xyz;
			aligned++;
		}
	}
	avoid = avoid/avoided;
	group = group/grouped*0.15;
	align = align/aligned;
	
	Boid rez;
	rez.Vel = normalize(vec4(avoid+group+align+home, 0.0))*dt*speed;
	rez.Pos = rez.Vel + me.Pos;
	barrier();
	memoryBarrier();
	bIN[myID] = rez;
}