#include "stdafx.h"
#include "CImageProcessor.h"

CImageProcessor::CImageProcessor()
{
	m_Params.M = Identity();
}
CImageProcessor::~CImageProcessor()
{
}
bool CImageProcessor::OnBegin(void * pParams, size_t size)
{

	return false;
}

void CImageProcessor::OnCompute(int nPass)
{
	switch (nPass)
	{
	case 0: 
		{
			m_pOwner->GetContext()->CSSetUnorderedAccessViews(0, 1, &m_vUAVs[0], 0);
			m_pOwner->GetContext()->CSSetShaderResources(0, 1, &m_vSRVs[0]);
			m_pOwner->GetContext()->CSSetShader(m_vShaders[0], 0, 0);
			ID3D11Resource* pRes = nullptr;
			ID3D11Texture2D* pT2D = nullptr;
			D3D11_TEXTURE2D_DESC dtd;
			m_vUAVs[0]->GetResource(&pRes);
			pRes->QueryInterface(IID_ID3D11Texture2D, (void**)&pT2D);
			pT2D->GetDesc(&dtd);
			pT2D->Release();
			pRes->Release();
			m_pOwner->GetContext()->Dispatch(
				(dtd.Width + 15) / 16,
				(dtd.Height + 15) / 16, 1);
			
		}break;
	}

}

long CImageProcessor::GetNumberOfPasses()
{
	return 1;
}

void CImageProcessor::OnEnd()
{
}
bool CImageProcessor::LoadPicture(const char* pszImageFileName)
{
	ID3D11ShaderResourceView* pSRV = nullptr;
	ID3D11Texture2D* pT2D = nullptr;
	pT2D = m_pOwner->LoadTexture(pszImageFileName, 1, &pSRV, nullptr, nullptr);
	if (!pT2D)
		return false;
	SAFE_RELEASE(pT2D);
	m_vSRVs.push_back(pSRV);
	return true;
}
