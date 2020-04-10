#pragma once
#include "CDXShader.h"
#include <vector>
using namespace std;

class CParticleSystem :
	public CDXShader
{
public:
	struct PARTICLE
	{
		VECTOR4D Position;
		VECTOR4D Velocity;
		VECTOR4D Acceleration;
		VECTOR4D Force;
		float InvMass;
	};

	vector<PARTICLE> m_Particles;
	struct PARAMS
	{
		VECTOR4D Gravity;
		float dt;
	}m_Params;

	CParticleSystem();
	~CParticleSystem();

	// Heredado v�a CDXShader
	virtual bool OnBegin(void * pParams, size_t Size) override;
	virtual void OnCompute(int nPass) override;
	virtual long GetNumberOfPasses(void) override;
	virtual void OnEnd(void) override;
};

