#include "stdafx.h"
#include "CDXShader.h"


bool CDXShader::Initialize(CDXManager * pOwner,const wchar_t * pszSourceFile,
	const char ** ppEntryPoints, int nEntryPoints)
{
	m_pOwner = pOwner;
	for (int i = 0; i < nEntryPoints; i++)
	{
		ID3D11ComputeShader* pCS =
			m_pOwner->CompileCS(pszSourceFile, ppEntryPoints[i]);
		if (!pCS) return false;
		m_vShaders.push_back(pCS);
	}
	//Salida por defecto si la hay
	if (m_pOwner->GetSwapChain())
	{
		ID3D11UnorderedAccessView* pUAV = nullptr;
		ID3D11Texture2D* pT2D = nullptr;
		m_pOwner->GetSwapChain()->GetBuffer(0, IID_ID3D11Texture2D, (void**)
			&pT2D);
		m_pOwner->GetDevice()->CreateUnorderedAccessView(pT2D, nullptr, &pUAV);
		m_vUAVs.push_back(pUAV);
	}
	return true;
}
void CDXShader::Uninitialize()
{
	for (auto x : m_vShaders) x->Release();
	for (auto x : m_vBuffers) x->Release();
	for (auto x : m_vUAVs) x->Release();
	for (auto x : m_vSRVs) x->Release();
	m_vShaders.clear();
	m_vBuffers.clear();
	m_vSRVs.clear();
	m_vUAVs.clear();
}
CDXShader::CDXShader()
{
}


CDXShader::~CDXShader()
{
}
