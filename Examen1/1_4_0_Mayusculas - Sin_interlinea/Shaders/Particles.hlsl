

struct Particle
{
	float4 Position;
	int letter;
	int index;
};

cbuffer Params
{
	float4 Gravity;
	float dt;
};

RWStructuredBuffer <Particle> Particles;
RWTexture2D<float4> Output;


[numthreads(256,1,1)]
void Simulate(uint gid:SV_DispatchThreadId)
{
	if ((96 < Particles[gid].letter) && (123 > Particles[gid].letter))
	{
		Particles[gid].letter -= 32;
	}
	if (13 == Particles[gid].letter)
	{
		Particles[gid].letter = 32;
	}
}

[numthreads(256, 1, 1)]
void Plot(uint gid:SV_DispatchThreadID)
{
	int2 coord = (int2)Particles[gid].Position.xy;
	// los arreglos el indice negativo da nops
	Output[coord] = float4(1, 1, 1, 1);
}

