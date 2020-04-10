/*
Shader por defecto
Objetivo: Mostrar los fundamentos del lenguaje HLSL:
High Level Shader Language para DirectCompute
*/

RWTexture2D<float4> Output;
cbuffer PARAMS
{
	float4 CircleParams; //x=cx, y=cy, z = 1/d, w = r
};

float lineFunc(uint i, float2 p1, float2 p2);
bool coloring(float inY, float inLine, float2 p1, float2 p2);

float4 Function(uint i, uint j)
{
	float4 returnColor = float4(i / 1024.0f, j / 1024.0f, 0, 0);
	//puntos
	float2 p1 = float2(200.0f, 200.0f);
	float2 p2 = float2(700.0f, 600.0f);
	float2 p3 = float2(700.0f, 200.0f);
	float inY = j;
	//lines
	float line1 = 0.0f;
	float line2 = 0.0f;
	float line3 = 0.0f;
	//Evaluations
	bool color1 = false;
	bool color2 = false;
	bool color3 = false;

	//Avoid 0 div
	// Same horizontal
	if ((p1.x == p2.x) || ((p1.x == p3.x)))
	{
		p1.x++;
	}
	if ((p2.x == p1.x) || ((p2.x == p3.x)))
	{
		p2.x-=3;
	}
	if ((p3.x == p1.x) || ((p3.x == p2.x)))
	{
		p3.x += 5;
	}
	// Same vertical
	if ((p1.y == p2.y) || ((p1.y == p3.y)))
	{
		p1.y++;
	}
	if ((p2.y == p1.y) || ((p2.y == p3.y)))
	{
		p2.y -= 3;
	}
	if ((p3.y == p1.y) || ((p3.y == p2.y)))
	{
		p3.y += 5;
	}
	//lines
	line1 = lineFunc(i, p1, p2);
	line2 = lineFunc(i, p2, p3);
	line3 = lineFunc(i, p3, p1);
	//Coloring
	// Edges 1
	color1 = coloring(inY, line1, p1, p3);
	color2 = coloring(inY, line2, p2, p1);
	color3 = coloring(inY, line3, p3, p2);
	if ((false == color1) || (false == color2) || (false == color3))
	{
		returnColor = 0;
	}
	return returnColor;
}

[numthreads(16,16,1)]//tamaño del grupo, el grupo tiene acceso a la memoria compartida
void main(uint3 lid:SV_GroupThreadID, uint3 gid : SV_DispatchThreadID)
{
	Output[gid.xy] = Function(gid.x, gid.y);
}

float lineFunc(uint i, float2 p1, float2 p2)
{
	float returnVal = 0;
	float x = float(i);
	float slope = (p2.y - p1.y);
	slope /= (p2.x - p1.x);
	returnVal = slope * (x - p1.x);
	returnVal += p1.y;
	return returnVal;
}

bool coloring(float inY,float inLine, float2 p1, float2 p2)
{
	bool returnColor = true;
	if (p2.y >= p1.y)
	{
		if (inY < inLine)
		{
			returnColor = false;
		}
	}
	else
	{
		if (inY > inLine)
		{
			returnColor = false;
		}
	}
	return returnColor;
}