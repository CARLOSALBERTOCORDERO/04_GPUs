#include "stdafx.h"
#include "CVectorSum.h"


CVectorSum::CVectorSum()
{
}


CVectorSum::~CVectorSum()
{
}

bool CVectorSum::OnBegin(void * pParams, size_t size)
{
	D3D11_BUFFER_DESC dbd;
	ID3D11Buffer* pInput = nullptr;
	ID3D11Buffer* pOutput = nullptr;

	// Crear el SRV
	ID3D11ShaderResourceView *pSRV;		// Input
	ID3D11UnorderedAccessView *pUAV;	// Output

	if (size)
	{

		memset(&dbd, 0, sizeof(dbd));
		//el primer buffer debe ser shared resource pero los bits deben estar encendidos
		dbd.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		dbd.Usage = D3D11_USAGE_DEFAULT;
		dbd.ByteWidth = sizeof(VECTOR4D)*size;
		// Entrada
		m_pOwner->GetDevice()->CreateBuffer(&dbd, 0, &pInput);

		// Salida
		dbd.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
		dbd.ByteWidth = sizeof(VECTOR4D)*((size + 511) / 512);
		m_pOwner->GetDevice()->CreateBuffer(&dbd, 0, &pOutput);


		// SRV = INPUT 
		// Aqui no le hemos puesto el formato, porque solo le estamos dando un buffer de datos lo mas sencillo
		D3D11_SHADER_RESOURCE_VIEW_DESC dsrvd;
		memset(&dsrvd, 0, sizeof(dsrvd));
		dsrvd.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		dsrvd.Buffer.ElementWidth = sizeof(VECTOR4D);
		dsrvd.Buffer.NumElements = size;
		dsrvd.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
		m_pOwner->GetDevice()->CreateShaderResourceView(pInput, &dsrvd, &pSRV);
		m_vSRVs.push_back(pSRV);

		// UAV = OUTPUT 
		D3D11_UNORDERED_ACCESS_VIEW_DESC duavd;
		memset(&duavd, 0, sizeof(duavd));
		duavd.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		duavd.Buffer.NumElements = (size + 511) / 512;
		duavd.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		m_pOwner->GetDevice()->CreateUnorderedAccessView(pOutput, &duavd, &pUAV);

		if (pParams)
		{
			ID3D11Buffer* pBuf;
			dbd.ByteWidth = sizeof(VECTOR4D)*size;
			dbd.BindFlags = 0;
			dbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			dbd.Usage = D3D11_USAGE_STAGING;
			m_pOwner->GetDevice()->CreateBuffer(&dbd, 0, &pBuf);

			D3D11_MAPPED_SUBRESOURCE dms;
			m_pOwner->GetContext()->Map(pBuf, 0, D3D11_MAP_WRITE, 0, &dms);
			memcpy(dms.pData, pParams, sizeof(VECTOR4D)*size);
			m_pOwner->GetContext()->Unmap(pBuf, 0);
			m_pOwner->GetContext()->CopyResource(pInput, pBuf);
		}
	}
	//Entrada
	m_vBuffers.push_back(pInput);
	//Salida
	m_vBuffers.push_back(pOutput);
	// SRV = input
	m_vSRVs.push_back(pSRV);
	// UAV = OUTPUT 
	m_vUAVs.push_back(pUAV);
	return true;
}

void CVectorSum::OnCompute(int nPass)
{
	switch (nPass)
	{
	case 0:
		auto pCtx = m_pOwner->GetContext();
		pCtx->CSSetShader(m_vShaders[0], 0, 0);
		pCtx->CSSetUnorderedAccessViews(0, 1, &m_vUAVs[1], 0);  // es 1 porque ya metimos en el initialized el UAV de la salida grafica
		pCtx->CSSetShaderResources(0, 1, &m_vSRVs[0]);
		D3D11_BUFFER_DESC dbd;
		/*
		m_vBuffers[0]->GetDesc(&dbd); // 31488 + 511 / 512 = 62 * 16 = 992
		//size es el tama;o de problema: (dbd.ByteWidth / sizeof(VECTOR4D)
		pCtx->Dispatch(((dbd.ByteWidth / sizeof(VECTOR4D) + 511) / 512),1,1);
		*/
		m_vBuffers[1]->GetDesc(&dbd); // 31488 + 511 / 512 = 62 * 16 = 
		pCtx->Dispatch(dbd.ByteWidth / sizeof(VECTOR4D), 1, 1);
		
		break;

	}
}

long CVectorSum::GetNumberOfPasses()
{
	return 1;
}

void CVectorSum::OnEnd()
{
	D3D11_BUFFER_DESC dbd;
	m_vBuffers[1]->GetDesc(&dbd);
	m_Results.resize(dbd.ByteWidth / sizeof(VECTOR4D));
	dbd.BindFlags = 0;
	dbd.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	dbd.Usage = D3D11_USAGE_STAGING;

	D3D11_MAPPED_SUBRESOURCE dms;

	ID3D11Buffer* pStaging = nullptr;
	m_pOwner->GetDevice()->CreateBuffer(&dbd, 0, &pStaging);
	m_pOwner->GetContext()->CopyResource(pStaging, m_vBuffers[1]);
	m_pOwner->GetContext()->Map(pStaging, 0, D3D11_MAP_READ, 0, &dms);
	memcpy(&m_Results[0], dms.pData, dbd.ByteWidth);
	m_pOwner->GetContext()->Unmap(pStaging, 0);
	pStaging->Release();
}
