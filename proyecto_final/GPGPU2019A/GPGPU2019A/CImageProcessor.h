#pragma once
#include "CDXShader.h"
class CImageProcessor :
	public CDXShader
{
public:
	struct PARAMS
	{
		MATRIX4D M;
	}m_Params;
	bool LoadPicture(const char* pszImageFileName);
	CImageProcessor();
	~CImageProcessor();
	// Inherited via CDXShader
	virtual bool OnBegin(void * pParams, size_t size) override;
	virtual void OnCompute(int nPass) override;
	virtual long GetNumberOfPasses() override;
	virtual void OnEnd() override;
};

