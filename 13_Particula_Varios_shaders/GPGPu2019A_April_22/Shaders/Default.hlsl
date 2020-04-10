/* 
Shader por defecto
Objetivo: Mostrar los fundamentos del lenguaje HLSL:
High Level Shader Language para DirectCompute
*/

RWTexture2D<float4> Output;

cbuffer PARAMS
{
	float4 CircleParams; //x=cx, y=cy, z= 1/d, w=r^2
};

float4 Funcion(uint i, uint j)
{
	//(x-512)^2+(y-512)^2 < r^2
	float2 p=float2(i,j);
	p -= CircleParams.xy;
	float resp=dot(p, p);
	float4 color= float4(i * CircleParams.z, j *CircleParams.z, 0, 0);
	if (resp < CircleParams.w)
		return color;
	else
		return 1 - color;
}

[numthreads(16,16,1)]
void main(uint3 lid:SV_GroupThreadID, uint3 gid : SV_DispatchThreadID)
{
	Output[gid.xy] = Funcion(gid.x, gid.y);
}