/*
Shader por defecto
Objetivo: Mostrar los fundamentos del lenguaje HLSL:
High Level Shader Language para DirectCompute
*/

RWTexture2D<float4> Output;
cbuffer PARAMS
{
	float4 Params; 
	float2 Move;
};

float lineFunc(uint i, float2 p1, float2 p2);
bool coloring(float inY, float inLine, float2 p1, float2 p2);
float2  scaleNorm(int positionX, int positionY);
int  scaledX(int positionX);
int  scaledY(int positionY);
float  absSquare(float2 position);
float2 pointSquare(float2 position);

float4 Function(uint i, uint j, uint cnt)
{
	//float4 returnColor = float4(i / 1024.0f, j / 1024.0f, 0, 0);
	float4 returnColor = float4(0.0f, 0.0f, 0.0f, 0);
	float color = 0.0f;
	if (255 > cnt)
	{
		color = cnt / 255.0f;
	}
	returnColor.x = color;
	returnColor.y = color;
	returnColor.z = color;
	return returnColor;
}

[numthreads(16,16,1)]//tamaño del grupo, el grupo tiene acceso a la memoria compartida
void main(uint3 lid:SV_GroupThreadID, uint3 gid : SV_DispatchThreadID)
{
	float2 j = float2(Params.x, Params.y);
	uint groupSize = Params.z;
	uint cnt = 0;
	float2 pointJulia = float2(0.0f, 0.0f);
	float pointJuliaAbs = 0.0f;
	float4 colorValue = float4(gid.x / 1024.0f, gid.y / 1024.0f, 0, 0);
	
	pointJulia = scaleNorm(gid.x, gid.y);
	pointJuliaAbs = absSquare(pointJulia);
	while ((4 > pointJuliaAbs) && (groupSize > cnt))
	{
		pointJulia = pointSquare(pointJulia);
		pointJulia += j;
		cnt++;
		pointJuliaAbs = absSquare(pointJulia);
		if (4 > pointJuliaAbs)
		{
			colorValue = Function(pointJulia.x, pointJulia.y, cnt);
		}
		Output[gid.xy] = colorValue;
	}
}

float2 pointSquare(float2 position)
{
	float2 returnVal = float2(0.0f, 0.0f);
	returnVal.x = position.x * position.x;
	returnVal.x -= position.y * position.y;
	returnVal.y = position.x * position.y;
	returnVal.y *= 2.0f;
	return returnVal;
}

float2  scaleNorm(int positionX, int positionY)
{
	float2 returnVal = float2(positionX, positionY);
	returnVal -= 512.0f;
	returnVal += Move;
	returnVal /= 512.0f;
	returnVal *= Params.w;
	returnVal /= 100.0;
	return returnVal;
}


int  scaledX(int positionX)
{
	int returnVal = positionX;

	return returnVal;
}
int  scaledY(int positionY)
{
	int returnVal = positionY;

	return returnVal;
}

float  absSquare(float2 position)
{
	float returnVal = 0.0f;
	returnVal = dot(position, position);
	return returnVal;
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