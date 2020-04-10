

Buffer<float4> Input;
RWBuffer<float4> Output;

groupshared float4 SharedBuffer[256];

[numthreads(256,1,1)]
void Sum(uint3 lid:SV_GroupThreadID, uint3 gid:SV_DispatchThreadID, uint3 group:SV_GroupID)
{
	SharedBuffer[lid.x] = Input[2 * gid.x] + Input[2 * gid.x + 1];
	GroupMemoryBarrierWithGroupSync();
	if (!(lid.x & 0x01))
		SharedBuffer[lid.x] += SharedBuffer[lid.x + 1];
	GroupMemoryBarrierWithGroupSync();
	if(!(lid.x & 0x03))
		SharedBuffer[lid.x] += SharedBuffer[lid.x + 2];
	GroupMemoryBarrierWithGroupSync();
	if (!(lid.x & 0x07))
		SharedBuffer[lid.x] += SharedBuffer[lid.x + 4];
	GroupMemoryBarrierWithGroupSync();
	if (!(lid.x & 0x0f))
		SharedBuffer[lid.x] += SharedBuffer[lid.x + 8];
	GroupMemoryBarrierWithGroupSync();
	if (!(lid.x & 0x1f))
		SharedBuffer[lid.x] += SharedBuffer[lid.x + 16];
	GroupMemoryBarrierWithGroupSync();
	if (!(lid.x & 0x3f))
		SharedBuffer[lid.x] += SharedBuffer[lid.x + 32];
	GroupMemoryBarrierWithGroupSync();
	if (!(lid.x & 0x7f))
		SharedBuffer[lid.x] += SharedBuffer[lid.x + 64];
	GroupMemoryBarrierWithGroupSync();
	if (!(lid.x & 0xff))
		Output[group.x]=SharedBuffer[lid.x] + SharedBuffer[lid.x + 128];
}



