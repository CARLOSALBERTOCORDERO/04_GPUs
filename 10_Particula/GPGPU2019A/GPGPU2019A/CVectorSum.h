#pragma once
#include "CDXShader.h"
class CVectorSum :
	public CDXShader
{
public:
	vector<VECTOR4D> m_Results;
	CVectorSum();
	~CVectorSum();

	// Heredado vía CDXShader
	virtual bool OnBegin(void * pParams, size_t Size) override;
	virtual void OnCompute(int nPass) override;
	virtual long GetNumberOfPasses(void) override;
	virtual void OnEnd(void) override;
};

