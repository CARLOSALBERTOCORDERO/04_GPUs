#pragma once
#include "CDXManager.h"
#include<vector>
using namespace std;
class CDXShader
{
protected:
	vector<ID3D11ComputeShader*> m_vShaders;
	vector<ID3D11Buffer*> m_vConstantBuffers;
	vector<ID3D11UnorderedAccessView*> m_vUAVs;
	vector<ID3D11ShaderResourceView*>  m_vSRVs;
	CDXManager* m_pOwner;
public:
	bool Initialize(CDXManager* pOwner,const wchar_t* pszSourceFile,
		const char** ppEntryPoints,int nEntryPoints);
	void Uninitialize();
	virtual bool OnBegin(void* pParams,size_t size)=0;
	virtual void OnCompute(int nPass)=0;
	virtual long GetNumberOfPasses() = 0;
	virtual void OnEnd() = 0;
	CDXShader();
	~CDXShader();
};

