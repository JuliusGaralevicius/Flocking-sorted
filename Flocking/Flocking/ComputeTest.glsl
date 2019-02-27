#version 450 core
layout (local_size_x = 256) in;
struct Boid
{
	vec4 Vel;
	vec4 Pos;
};
struct Obstacle
{
	vec3 position;
	float radius;
	vec4 velocity;
};
layout (binding = 0) buffer inBoids
{
	Boid bIN[16384];
};
layout (binding = 1) buffer inIndexes
{
	uint sortedIndexes[];
};
layout (binding = 2) buffer inObstacles
{
	Obstacle oIN[];
};

uniform uint dimension = 16;
uniform uint nbhdRange = 255;
uniform float dt = 0.0f;
uniform vec4 goal = vec4(0.0f);
uniform float speed = 100.0f;

float groupRadius = 4;
float avoidRadius = 2;

vec2 clampVectorLength(vec2 vector, float max)
{
	if (length(vector)>max)
	{
		return normalize(vector)*max;
	}
	else
	{
		return vector;
	}
}
uint getCell(vec2 p)
{
	uint cellSize = 32;
	uint numCells = 16;
	return int(p.y/cellSize)*numCells + int(p.x/cellSize);
}
bool inrange(uint value)
{
	return (value<=nbhdRange);
}

void main()
{
	uint myID = gl_GlobalInvocationID.x;
	Boid me = bIN[myID];
	// will return invalid if out of bounds
	uint myCell = getCell(me.Pos.xy);

	vec3 avoid = vec3(0.0);
	vec3 group = vec3(0.0);
	vec3 align = vec3(0.0);
	vec3 home = (goal-me.Pos).xyz;
	home = home*dot(home, home)*0.00000000005;
	
	int avoided = 0;
	int grouped = 0;
	int aligned = 0;
	
	// table which shows which neighbourhood cells are present and which are not
	uint offsetFlags[9] = {1, 1, 1,
						   1, 1, 1,
						   1, 1, 1
						   };
						   
	// table where each neighbourhood entry has start index of sorted array					   
	uint offsetTable[9];
	for (int i = 0; i<9; i++)
	{
		offsetTable[i] = nbhdRange+1;
	}
						
	if (mod(myCell, dimension) == 0)
	{
		// leftmost cell
		offsetFlags[0] = nbhdRange+1;
		offsetFlags[3] = nbhdRange+1;
		offsetFlags[6] = nbhdRange+1;
	}
	if (mod(myCell, dimension) == dimension-1)
	{
		// rightmost cell
		offsetFlags[2] = nbhdRange+1;
		offsetFlags[5] = nbhdRange+1;
		offsetFlags[8] = nbhdRange+1;
	}
	if (myCell/16 == 0)
	{
	   // topmost cell
	   	offsetFlags[0] = nbhdRange+1;
		offsetFlags[1] = nbhdRange+1;
		offsetFlags[2] = nbhdRange+1;
	}
	if (myCell/16 == dimension-1)
	{
	    // bottom cell
	   	offsetFlags[6] = nbhdRange+1;
		offsetFlags[7] = nbhdRange+1;
		offsetFlags[8] = nbhdRange+1;
	}
	// fill in with cellID of neighbour or invalid
	for (int i = 0; i<3; i++)
	{
		for (int j = 0; j<3; j++)
		{
			if (inrange(offsetFlags[i*3+j]))
			{
				uint checkCell = (myCell + (i-1)*dimension) + (j-1);
				uint start = sortedIndexes[checkCell];
				uint end;
				if (inrange(checkCell+1))
				{
					end = sortedIndexes[checkCell+1];
				}
				else 
				{
					end = gl_WorkGroupSize.x*gl_NumWorkGroups.x;
				}
				for (uint q = start; q<end; q++)
				{
					Boid other = bIN[q];
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
				for (uint i = 0; i<16; i++)
				{
					Obstacle ob = oIN[i];
					vec3 distance = ob.position - me.Pos.xyz;
					if (dot(distance, distance)<ob.radius*ob.radius+100)
					{
						avoid-=distance.xyz;
						avoided++;
					}
				}
			}
		}
	}
	
	avoid = avoid/avoided;
	group = group/grouped*0.05;
	align = align/aligned;
	
	Boid rez;
	rez.Vel = vec4(clampVectorLength(me.Vel.xy, 0.3), 0.0, 0.0) + vec4(clampVectorLength(vec2(avoid.xy+group.xy+align.xy+home.xy), 0.1), 0.0, 0.0);
	rez.Pos = rez.Vel + me.Pos;
	
	barrier();
	memoryBarrier();
	
	if (rez.Pos.x>512.0f)
		rez.Pos.x=rez.Pos.x-512.0f;
	else if (rez.Pos.x<0)
		rez.Pos.x=rez.Pos.x + 512.0f;
	if (rez.Pos.y>512.0f)
		rez.Pos.y=rez.Pos.y-512.0f;
	else if (rez.Pos.y<0)
		rez.Pos.y=rez.Pos.y + 512.0f;
	bIN[myID] = rez;
}