/*
shader por deecto
Objetivo: Mostrar los fundamenteos del lenguaje HLSL
High level Shader Lenguage para DirectCompute
*/
RWTexture2D<float4> Output;

// Son parametros (solo lectura) y no necesitan Viewer ni sampler
cbuffer PARAMS
{
	
	float4 CircleParams; // x = cx, y = cy, z = 1/ d, w = r^2
	float4 CirclePox; // x = cx, y = cy, zw = NA
};
float4 Function(uint i, uint j)
{
	//return float4(cos(i/320.0f), sin(j/320.0f), (i+j)%256, (i>>8)%16)*
		//float4(0.5, 0.5, 1/255.0f, 1/15.0f) + float4(0.5, 0.5,0,0);
	float2 p = float2(i,j);
	//p -= float(512, 512);
	//p -= 512;
	p -= CircleParams.xy;
	//p *= p;
	float resp = dot(p, p);
	//float4 color = float4(i / 1024.0f, j / 1024.0f, 0, 0);
	float4 color = float4(i * CircleParams.z, j * CircleParams.z, 0, 0);
	//if ((512*512) > resp)
	if ((CircleParams.w) > resp)
	{
		return color;
	}
	else
	{
		return 1-color; // 4 ceros
	}
	
}

[numthreads(16,16,1)]
void main(uint3 lid:SV_GroupThreadID, uint3 gid:SV_DispatchThreadID) 
{
	Output[gid.xy] = Function(gid.x, gid.y);
}