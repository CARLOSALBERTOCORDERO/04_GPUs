cbuffer PARAMS
{
	matrix M;
};
Texture2D<float4> Input;
Buffer<float4>    InputMap;
RWTexture2D<float4> Output;
[numthreads(16,16,1)]
void LinearTransform(uint3 gid:SV_DispatchThreadID)
{
	float4 OutputPosition = float4(gid, 1);
	float4 InputPosition = mul(OutputPosition, M);
	Output[gid.xy] = Input[int2(InputPosition.xy)];
}
[numthreads(16,16,1)]
void Not(uint3 gid:SV_DispatchThreadID)
{
	uint4 input = uint4(Input[gid.xy] * 65535);
	input |= 0xffff0000;
	Output[gid.xy] = (~input) / 65535.0f;
}
[numthreads(16,16,1)]
void Map(uint3 gid:SV_DispatchThreadID)
{
	uint i = int(65535*dot(Input[gid.xy], float4(0.3, 0.5, 0.2, 0.0)));
	Output[gid.xy] = InputMap[i];
}

[numthreads(16, 16, 1)]
void Soft3x3(uint3 gid:SV_DispatchThreadID)
{
	float4 Accum = 0;
	uint2 Size;
	Output.GetDimensions(Size.x, Size.y);
	Size -= uint2(1, 1); // GetDimentions te da el numero de pizeles no la ultima posicion
	for (int j = -1; j < 2; j++)
	{
		for (int i = -1; i < 2; i++)
		{
			// clamp de borde izquierdo
			int2 index = min(int2(Size),max(int2(0,0), int2(gid.xy) + int2(i, j)));
			Accum += Input[index];
		}
	}
	Output[gid.xy] =  Accum / 9.0;
}

