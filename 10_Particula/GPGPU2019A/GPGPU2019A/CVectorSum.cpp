#include "stdafx.h"
#include "CVectorSum.h"


CVectorSum::CVectorSum()
{
}


CVectorSum::~CVectorSum()
{
}

bool CVectorSum::OnBegin(void * pParams, size_t Size)
{
	D3D11_BUFFER_DESC dbd;
	memset(&dbd, 0, sizeof(dbd));
	dbd.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	dbd.ByteWidth = sizeof(VECTOR4D)*Size;
	dbd.Usage = D3D11_USAGE_DEFAULT;
	ID3D11Buffer* pBuf = nullptr;
	m_pOwner->GetDevice()->CreateBuffer(&dbd, nullptr, &pBuf);
	m_vConstantBuffers.push_back(pBuf);
	dbd.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
	dbd.ByteWidth = sizeof(VECTOR4D) * ((Size + 511) / 512);
	m_pOwner->GetDevice()->CreateBuffer(&dbd, nullptr, &pBuf);
	m_vConstantBuffers.push_back(pBuf);
	ID3D11ShaderResourceView* pSRV = nullptr; // Input
	ID3D11UnorderedAccessView* pUAV; // Output
	D3D11_SHADER_RESOURCE_VIEW_DESC dsrvd;
	memset(&dsrvd, 0, sizeof(dsrvd));
	dsrvd.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	dsrvd.Buffer.ElementWidth = sizeof(VECTOR4D);
	dsrvd.Buffer.NumElements = Size;
	dsrvd.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	m_pOwner->GetDevice()->CreateShaderResourceView(m_vConstantBuffers[0], &dsrvd, &pSRV);
	m_vSRVs.push_back(pSRV);
	D3D11_UNORDERED_ACCESS_VIEW_DESC duavd;
	memset(&duavd, 0, sizeof(duavd));
	duavd.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	duavd.Buffer.NumElements = (Size + 511) / 512;
	duavd.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	m_pOwner->GetDevice()->CreateUnorderedAccessView(m_vConstantBuffers[1], &duavd, &pUAV);
	m_vUAVs.push_back(pUAV);
	
	dbd.ByteWidth = sizeof(VECTOR4D)*Size;
	dbd.BindFlags = 0;
	dbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	dbd.Usage = D3D11_USAGE_STAGING;
	m_pOwner->GetDevice()->CreateBuffer(&dbd, 0, &pBuf);
	D3D11_MAPPED_SUBRESOURCE dms;
	m_pOwner->GetContext()->Map(pBuf, 0, D3D11_MAP_WRITE, 0, &dms);
	memcpy(dms.pData, pParams, sizeof(VECTOR4D)*Size);
	m_pOwner->GetContext()->Unmap(pBuf, 0);
	m_pOwner->GetContext()->CopyResource(m_vConstantBuffers[0], pBuf);
	pBuf->Release();

	return false;
}

void CVectorSum::OnCompute(int nPass)
{
	switch (nPass)
	{
		case 0:
		{
			auto pCtx = m_pOwner->GetContext();
			pCtx->CSSetShader(m_vShaders[0], 0, 0);
			pCtx->CSSetUnorderedAccessViews(0,1,&m_vUAVs[1],0);
			pCtx->CSSetShaderResources(0,1,&m_vSRVs[0]);
			D3D11_BUFFER_DESC dbd;
			// En base a la entrada
			//m_vConstantBuffers[0]->GetDesc(&dbd);
			//pCtx->Dispatch((((dbd.ByteWidth/sizeof(VECTOR4D)) + 511)/512), 1,1);
			// En base a la salida
			m_vConstantBuffers[1]->GetDesc(&dbd);
			pCtx->Dispatch((dbd.ByteWidth * sizeof(VECTOR4D)), 1,1);
		}
		break;
	}
}

long CVectorSum::GetNumberOfPasses(void)
{
	return 0;
}

void CVectorSum::OnEnd(void)
{
	D3D11_BUFFER_DESC dbd;
	m_vConstantBuffers[1]->GetDesc(&dbd);
	m_Results.resize(dbd.ByteWidth/ sizeof(VECTOR4D));
	dbd.BindFlags = 0;
	dbd.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	dbd.Usage = D3D11_USAGE_STAGING;
	ID3D11Buffer* pStaging = nullptr;
	m_pOwner->GetDevice()->CreateBuffer(&dbd, 0 ,&pStaging);
	m_pOwner->GetContext()->CopyResource(pStaging, m_vConstantBuffers[1]);
	D3D11_MAPPED_SUBRESOURCE dms;
	m_pOwner->GetContext()->Map(pStaging, 0, D3D11_MAP_READ, 0, &dms);
	memcpy(&m_Results[0], dms.pData, dbd.ByteWidth);
	m_pOwner->GetContext()->Unmap(pStaging, 0);
	pStaging->Release();


}
