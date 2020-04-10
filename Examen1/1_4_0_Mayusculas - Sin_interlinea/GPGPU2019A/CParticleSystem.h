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
		int letter;
		int index;
	};

	vector<PARTICLE> m_Particles;
	struct PARAMS
	{
		VECTOR4D Gravity;
		float dt;
	}m_Params;

	CParticleSystem();
	~CParticleSystem();

	// Heredado vía CDXShader
	virtual bool OnBegin(void * pParams, size_t Size) override;
	virtual void OnCompute(int nPass) override;
	virtual long GetNumberOfPasses(void) override;
	virtual void OnEnd(void) override;
	void OnEndTxt(streampos sizeOut);
};

