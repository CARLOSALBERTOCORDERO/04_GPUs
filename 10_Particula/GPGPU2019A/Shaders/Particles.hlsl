

struct Particle
{
	float4 Position;
	float4 Velocity;
	float4 Acceleration;
	float4 Force;
	float InvMass;
};


cbuffer Params
{
	float4 Gravity;
	float dt;
};

RWStructuredBuffer <Particle> Particles:register(u1);
RWTexture2D<float4> Output:register(u0);


[numthreads(256,1,1)]
void Simulate(uint gid:SV_DispatchThreadId)
{
	Particles[gid].Acceleration = (Particles[gid].Force * Particles[gid].InvMass) + Gravity;
	Particles[gid].Velocity += Particles[gid].Acceleration * dt;
	Particles[gid].Position += Particles[gid].Velocity * dt;
}

[numthreads(256, 1, 1)]
void Plot(uint gid:SV_DispatchThreadID)
{
	int2 coord = (int2)Particles[gid].Position.xy;
	// los arreglos el indice negativo da nops
	Output[coord] = float4(1, 1, 1, 1);
}

