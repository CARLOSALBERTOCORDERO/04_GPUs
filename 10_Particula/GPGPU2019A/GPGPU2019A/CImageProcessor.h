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

	// Heredado vía CDXShader
	virtual bool OnBegin(void * pParams, size_t Size) override;
	virtual void OnCompute(int nPass) override;
	virtual long GetNumberOfPasses(void) override;
	virtual void OnEnd(void) override;
};

