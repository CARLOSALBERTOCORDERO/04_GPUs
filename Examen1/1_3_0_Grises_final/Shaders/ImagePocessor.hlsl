Texture2D<float4> Input;
RWTexture2D<float4> Output;

[numthreads(16,16,1)]
void LinearTransform(uint3 gid:SV_DispatchThreadID)
{
	float4 luma = float4(0.0, 0.0, 0.0, 0.0);
	float4 lumaCons = float4(0.2126, 0.7152, 0.0722, 0.0);
	luma.xyz = dot(Input[gid.xy], lumaCons);
	luma.w = Input[gid.xy].w;
	Output[gid.xy] = luma;
}