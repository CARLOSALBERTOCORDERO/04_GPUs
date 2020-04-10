#pragma once
#include "CDXShader.h"
class CVectorSum :
	public CDXShader
{
public:
	vector<VECTOR4D> m_Results;
	CVectorSum();
	~CVectorSum();
	// Inherited via CDXShader
	virtual bool OnBegin(void * pParams, size_t size) override;
	virtual void OnCompute(int nPass) override;
	virtual long GetNumberOfPasses() override;
	virtual void OnEnd() override;
};

