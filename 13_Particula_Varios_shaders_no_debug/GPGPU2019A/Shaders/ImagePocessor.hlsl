cbuffer PARAMS
{
	matrix M;
};

Texture2D<float4> Input;
Buffer<float4> inputMap;
RWTexture2D<float4> Output;

[numthreads(16,16,1)]
void LinearTransform(uint3 gid:SV_DispatchThreadID)
{
	float4 OutputPossition = float4(gid, 1); // le agregamos un elemento para que sean 4
	float4 InputPosition = mul(OutputPossition,M);
	Output[gid.xy] = Input[int2(InputPosition.xy)];
}

[numthreads(16,16,1)]
void Not(uint3 gid:SV_DispatchThreadID)
{
	uint4 input = uint4(Input[gid.xy] * 65535);
	input |= 0xffff0000; // al negar las ff seran 0
	Output[gid.xy] = (~input)/65535.0f;
}

[numthreads(16, 16, 1)]
void Map(uint3 gid:SV_DispatchThreadID)
{
	uint i = int(65535 * dot(Input[gid.xy], float4(0.3, 0.5, 0.2, 0.0)));
	Output[gid.xy] = inputMap[i];
}