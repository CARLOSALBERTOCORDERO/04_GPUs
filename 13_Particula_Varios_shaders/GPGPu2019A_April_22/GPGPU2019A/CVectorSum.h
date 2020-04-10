#pragma once
#include "CDXShader.h"
class CVectorSum :
	public CDXShader
{
public:
	CVectorSum();
	~CVectorSum();


	vector<VECTOR4D> m_Results;

	// Inherited via CDXShader
	virtual bool OnBegin(void * pParams, size_t size) override;
	virtual void OnCompute(int nPass) override;
	virtual long GetNumberOfPasses() override;
	virtual void OnEnd() override;
};

