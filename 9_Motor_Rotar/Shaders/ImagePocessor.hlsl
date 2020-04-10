cbuffer PARAMS
{
	matrix M;
};

Texture2D<float4> Input;
RWTexture2D<float4> Output;

[numthreads(16,16,1)]
void LinearTransform(uint3 gid:SV_DispatchThreadID)
{
	float4 OutputPossition = float4(gid, 1); // le agregamos un elemento para que sean 4
	float4 InputPosition = mul(OutputPossition,M);
	Output[gid.xy] = Input[int2(InputPosition.xy)];
}