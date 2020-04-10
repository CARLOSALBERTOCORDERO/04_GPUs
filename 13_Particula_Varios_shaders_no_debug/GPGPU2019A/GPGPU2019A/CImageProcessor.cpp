#include "stdafx.h"
#include "CImageProcessor.h"


CImageProcessor::CImageProcessor()
{
}


CImageProcessor::~CImageProcessor()
{
	m_Params.M = Identity();
}

bool CImageProcessor::OnBegin(void * pParams, size_t Size)
{
	ID3D11Buffer* pBuffer = nullptr;
	D3D11_BUFFER_DESC dbd;
	memset(&dbd, 0, sizeof(dbd));
	dbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	dbd.ByteWidth = 16 * ((sizeof(dbd)+15)/16);
	dbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	dbd.Usage = D3D11_USAGE_DYNAMIC;
	m_pOwner->GetDevice()->CreateBuffer(&dbd, nullptr, &pBuffer);
	m_vBuffers.push_back(pBuffer);
	return false;
}

void CImageProcessor::OnCompute(int nPass)
{
	switch (nPass)
	{
	case 0:
		PARAMS Temp = m_Params;
		D3D11_MAPPED_SUBRESOURCE dms;
		ID3D11Resource* pRes = nullptr;
		ID3D11Texture2D* pT2D = nullptr;
		D3D11_TEXTURE2D_DESC dtd;

		// los shaders ya esperan la informacion de la matriz transpuesta para eficentizar la multiplicacion
		Temp.M = Transpose(m_Params.M);
		m_pOwner->GetContext()->Map(m_vBuffers[0], 0, D3D11_MAP_WRITE_DISCARD, 0, &dms);
		memcpy(dms.pData, &Temp, sizeof(PARAMS));
		m_pOwner->GetContext()->Unmap(m_vBuffers[0], 0);
		m_pOwner->GetContext()->CSSetConstantBuffers(0, m_vBuffers.size(), &m_vBuffers[0]);

		// por lo general los driver para la GPU de windows mapean y pasanla info en una sola vez
		// por lo que si ponemos el setconstanbuffer antes puede ser ignorado porque cuando mapea es cuando
		// decide que va a pasar.

		m_pOwner->GetContext()->CSSetUnorderedAccessViews(0, 1, &m_vUAVs[0], 0);
		m_pOwner->GetContext()->CSSetShaderResources(0, 1, &m_vSRVs[0]);
		m_pOwner->GetContext()->CSSetShader(m_vShaders[0], 0, 0);
		// Consultar tamaño de salida
		m_vUAVs[0]->GetResource(&pRes);
		pRes->QueryInterface(IID_ID3D11Texture2D, (void**)&pT2D);
		pT2D->GetDesc(&dtd);
		pT2D->Release();
		pRes->Release();

		m_pOwner->GetContext()->Dispatch((dtd.Width + 15) / 16, (dtd.Height + 15) / 16, 1);
		break;
	}
}

long CImageProcessor::GetNumberOfPasses(void)
{
	return 0;
}

void CImageProcessor::OnEnd(void)
{

}

bool CImageProcessor::LoadPicture(const char* pszImageFileName)
{
	ID3D11ShaderResourceView* pSRV = nullptr;
	ID3D11Texture2D* pT2D = nullptr;
	// e1 1 es el mip map
	pT2D = m_pOwner->LoadTexture(pszImageFileName, 1, &pSRV, nullptr, nullptr);
	if (!pT2D)
	{
		return false;
	}
	SAFE_RELEASE(pT2D);
	m_vSRVs.push_back(pSRV);
	return true;

}