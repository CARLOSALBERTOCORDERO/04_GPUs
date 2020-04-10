#include "stdafx.h"
#include "CDXShader.h"


bool CDXShader::Initialize(CDXManager * pOwner, const wchar_t * pszSourceFile, const char ** ppEntryPoints, int nEntryPoints)
{
	m_pOwner = pOwner;
	for (int i = 0; i < nEntryPoints; i++)
	{
		ID3D11ComputeShader* pCS = nullptr;
		pCS=m_pOwner->CompileCS(pszSourceFile, ppEntryPoints[i]);
		if (!pCS)
			return false;
		m_vShaders.push_back(pCS);
	}
	if (m_pOwner->m_pSwapChain)
	{
		ID3D11UnorderedAccessView* pUAV = nullptr;
		ID3D11Texture2D* pT2D = nullptr;
		m_pOwner->m_pSwapChain->GetBuffer(0, IID_ID3D11Texture2D, (void**)&pT2D);
		m_pOwner->GetDevice()->CreateUnorderedAccessView(pT2D, nullptr, &pUAV);
		m_vUAVs.push_back(pUAV);
	}
	return true;
}

void CDXShader::Unitilize()
{
	for (auto x:m_vShaders)	// con auto ya toma el tipo que le corresponde
	{
		x->Release();
	}
	for (auto x : m_vConstantBuffers)	// con auto ya toma el tipo que le corresponde
	{
		x->Release();
	}
	for (auto x : m_vUAVs)	// con auto ya toma el tipo que le corresponde
	{
		x->Release();
	}
	for (auto x : m_vSRVs)	// con auto ya toma el tipo que le corresponde
	{
		x->Release();
	}
	m_vShaders.clear();
	m_vConstantBuffers.clear();
	m_vUAVs.clear();
	m_vSRVs.clear();
}

CDXShader::CDXShader()
{
}


CDXShader::~CDXShader()
{
}
