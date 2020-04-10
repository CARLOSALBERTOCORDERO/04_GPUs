
struct Particle
{
	float4 Position;
	float4 Velocity;
	float4 Acceleration;
	float4 Force;
	float  InvMass;
};

cbuffer PARAMS
{
	float4 Gravity;
	float  dt;
};

RWStructuredBuffer<Particle> Particles;
RWTexture2D<float4> Output;

[numthreads(256,1,1)]
void Simulate(uint gid:SV_DispatchThreadID)
{
	Particles[gid].Acceleration = Particles[gid].Force *Particles[gid].InvMass + Gravity;
	Particles[gid].Velocity += Particles[gid].Acceleration*dt;
	Particles[gid].Position += Particles[gid].Velocity*dt;
}

[numthreads(256,1,1)]
void Plot(uint gid:SV_DispatchThreadID)
{
	int2 coord=(int2)Particles[gid].Position.xy;
	Output[coord] = float4(1, 1, 1, 1);
}



