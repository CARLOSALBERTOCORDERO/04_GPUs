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
	ID3D11Buffer* pBuffer=nullptr;
	D3D11_BUFFER_DESC dbd;
	memset(&dbd, 0, sizeof(dbd));
	dbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	dbd.ByteWidth = 16 * ((sizeof(PARAMS) + 15) / 16);
	dbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	dbd.Usage = D3D11_USAGE_DYNAMIC;
	m_pOwner->GetDevice()->CreateBuffer(&dbd, nullptr, &pBuffer);
	m_vBuffers.push_back(pBuffer);

	D3D11_SAMPLER_DESC dsd;
	memset(&dsd, 0, sizeof(dsd));
	dsd.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;
	dsd.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	dsd.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	//dsd.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	dsd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	dsd.MaxAnisotropy = 1;
	//dsd.ComparisonFunc = D3D11_COMPARISON_ALWAYS;

	m_pOwner->GetDevice()->CreateSamplerState(&dsd, &m_pSampler);
	return false;
}

void CImageProcessor::OnCompute(int nPass)
{
	switch (nPass)
	{
	case 0:
		{
			PARAMS Temp = m_Params;
			D3D11_MAPPED_SUBRESOURCE dms;
			Temp.M = Transpose(m_Params.M);
			m_pOwner->GetContext()->Map(m_vBuffers[0], 0,
				D3D11_MAP_WRITE_DISCARD, 0, &dms);
			memcpy(dms.pData, &Temp, sizeof(PARAMS));
			m_pOwner->GetContext()->Unmap(m_vBuffers[0], 0);
			m_pOwner->GetContext()->CSSetConstantBuffers(0,
				(UINT)m_vBuffers.size(), &m_vBuffers[0]);
			m_pOwner->GetContext()->CSSetUnorderedAccessViews(0, 1, &m_vUAVs[0], 0);
			m_pOwner->GetContext()->CSSetShaderResources(0, 1, &m_vSRVs[0]);
			m_pOwner->GetContext()->CSSetSamplers(0,1,&m_pSampler);
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
	case 1:break;
	case 2: break;
	case 3: 
		{
			m_pOwner->GetContext()->CSSetUnorderedAccessViews(0, 1, &m_vUAVs[0], 0);
			m_pOwner->GetContext()->CSSetShaderResources(0, 1, &m_vSRVs[0]);
			m_pOwner->GetContext()->CSSetShader(m_vShaders[3], 0, 0);
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
	return 4;
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
	D3D11_TEXTURE2D_DESC dtd;
	pT2D->GetDesc(&dtd);
	m_Params.invSizeImage.x = 1.0f/ dtd.Width;
	m_Params.invSizeImage.y = 1.0f / dtd.Height;
	SAFE_RELEASE(pT2D);
	m_vSRVs.push_back(pSRV);
	return true;
}
