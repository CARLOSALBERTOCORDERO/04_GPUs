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
		float    InvMass;
	};
	vector<PARTICLE> m_Particles;
	struct PARAMS
	{
		VECTOR4D Gravity;
		float dt;
	}m_Params;
	size_t sizeParticles;
	CParticleSystem();
	~CParticleSystem();

	// Inherited via CDXShader
	virtual bool OnBegin(void * pParams, size_t size) override;
	virtual void OnCompute(int nPass) override;
	virtual long GetNumberOfPasses() override;
	virtual void OnEnd() override;
};

