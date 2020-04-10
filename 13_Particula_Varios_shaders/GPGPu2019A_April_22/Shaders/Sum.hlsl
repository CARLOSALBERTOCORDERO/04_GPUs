/*
L Input(N)
L Output(N/2)

No necesitamos constant buffer
Necesitamos una entrada o uuna salida
Puede ser uno no estructurado para usar tipos básicos

Posiciones del Changuito xyzw usaremos datos de tipo float4

*/

Buffer<float4> Input;
RWBuffer<float4> Output;

groupshared float4 SharedBuffer[256]; // 256 resultados temporales

[numthreads(256,1,1)]

void Sum(uint3 lid:SV_GroupThreadID, uint3 gid:SV_DispatchThreadID, uint3 group:SV_GroupID)
{/*Variables locales , no se ven entre hilos, son locales por hilo*/
	// solo podemos indexar local hasta 256, si recibe mas va a eliminar la info.
	// podria usar un uint enlugar de uint3 y el indice se llamaria lid solamente

	SharedBuffer[gid.x] = Input[2 * gid.x] + Input[2 * gid.x + 1];
	GroupMemoryBarrierWithGroupSync();

	// divergencia es cuando los hilos no estan haciendo nada, en este caso es necesario
	if (!(lid.x & 0x01))// Si es par , evalua que no sea non o que no termine con 1 en el bit lsb
		SharedBuffer[lid.x] += SharedBuffer[lid.x + 1];
	GroupMemoryBarrierWithGroupSync();

	if (!(lid.x & 0x03))
		SharedBuffer[lid.x] += SharedBuffer[lid.x + 2];
	GroupMemoryBarrierWithGroupSync();

	if (!(lid.x & 0x07))
		SharedBuffer[lid.x] += SharedBuffer[lid.x + 4];
	GroupMemoryBarrierWithGroupSync();

	if (!(lid.x & 0x0F))
		SharedBuffer[lid.x] += SharedBuffer[lid.x + 8];
	GroupMemoryBarrierWithGroupSync();

	if (!(lid.x & 0x1F))
		SharedBuffer[lid.x] += SharedBuffer[lid.x + 16];
	GroupMemoryBarrierWithGroupSync();

	if (!(lid.x & 0x3F))
		SharedBuffer[lid.x] += SharedBuffer[lid.x + 32];
	GroupMemoryBarrierWithGroupSync();

	if (!(lid.x & 0x7F))
		SharedBuffer[lid.x] += SharedBuffer[lid.x + 64];
	GroupMemoryBarrierWithGroupSync();

	if (!(lid.x & 0xFF))
		Output[group.x] = SharedBuffer[lid.x] + SharedBuffer[lid.x + 128];

}