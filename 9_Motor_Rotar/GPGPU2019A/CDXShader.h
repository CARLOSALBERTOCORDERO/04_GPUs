#pragma once
#include "CDXManager.h"
#include <vector>
using namespace std;

class CDXShader
{
protected:
	vector<ID3D11ComputeShader*> m_vShaders;
	vector<ID3D11Buffer*> m_vConstantBuffers;
	vector<ID3D11UnorderedAccessView*> m_vUAVs;
	vector<ID3D11ShaderResourceView*> m_vSRVs;
	//Agregacion delegacion el hijo depende del padre pero no alrevez y lo agrega
	CDXManager* m_pOwner;

public:
	bool Initialize(CDXManager* pOwner, const wchar_t* pszSourceFile, const char** ppEntryPoints, int nEntryPoints);
	void Unitilize();
	virtual bool OnBegin(void* pParams, size_t Size)=0;
	virtual void OnCompute(int nPass)=0;
	virtual long GetNumberOfPasses(void) = 0;
	virtual void OnEnd(void)=0;

	CDXShader();
	~CDXShader();
};

