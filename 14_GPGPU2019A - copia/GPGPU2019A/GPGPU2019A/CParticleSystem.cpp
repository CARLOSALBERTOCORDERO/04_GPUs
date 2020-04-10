#include "stdafx.h"
#include "CParticleSystem.h"


CParticleSystem::CParticleSystem()
{
}


CParticleSystem::~CParticleSystem()
{
}

bool CParticleSystem::OnBegin(void * pParams, size_t size)
{
	D3D11_BUFFER_DESC dbd;
	memset(&dbd, 0, sizeof(dbd));
	// Constant buffer
	dbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	dbd.Usage = D3D11_USAGE_DYNAMIC;
	dbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	dbd.ByteWidth = 16 * ((sizeof(PARAMS) + 15) / 16);
	ID3D11Buffer* pCB=nullptr;
	m_pOwner->GetDevice()->CreateBuffer(&dbd, 0, &pCB);
	m_vBuffers.push_back(pCB);
	// Structured buffer GPU
	ID3D11UnorderedAccessView* pUAV = nullptr;
	ID3D11ShaderResourceView* pSRV = nullptr;//Input
	if (size)
	{
		
		dbd.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		dbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		dbd.StructureByteStride = sizeof(PARTICLE);
		dbd.Usage = D3D11_USAGE_DEFAULT;
		dbd.ByteWidth = size * sizeof(PARTICLE);
		dbd.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		m_pOwner->GetDevice()->CreateBuffer(&dbd, 0, &pCB);
		m_vBuffers.push_back(pCB);
		m_pOwner->GetDevice()->CreateShaderResourceView(pCB, NULL, &pSRV);
		
		dbd.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
		dbd.CPUAccessFlags = 0;
		dbd.StructureByteStride = sizeof(PARTICLE);
		dbd.Usage = D3D11_USAGE_DEFAULT;
		dbd.ByteWidth = size * sizeof(PARTICLE);
		dbd.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		m_pOwner->GetDevice()->CreateBuffer(&dbd, 0, &pCB);
		//m_vBuffers.push_back(pCB);
		m_pOwner->GetDevice()->CreateUnorderedAccessView(pCB, NULL, &pUAV);
	}
	m_vUAVs.push_back(pUAV);
	// Subir la información al GPU.
	if(size)
		m_Particles.resize(size);
	if (pParams)
	{
		memcpy(&m_Particles[0], pParams, size * sizeof(PARTICLE));
		dbd.BindFlags = 0;
		dbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		dbd.Usage = D3D11_USAGE_STAGING;
		m_pOwner->GetDevice()->CreateBuffer(&dbd, 0, &pCB);
		D3D11_MAPPED_SUBRESOURCE dms;
		m_pOwner->GetContext()->Map(pCB, 0, D3D11_MAP_WRITE, 0, &dms);
		memcpy(dms.pData, &m_Particles[0], sizeof(PARTICLE)*size);
		m_pOwner->GetContext()->Unmap(pCB, 0);
		m_pOwner->GetContext()->CopyResource(m_vBuffers[1], pCB);
		pCB->Release();
	}
	return true;
}

void CParticleSystem::OnCompute(int nPass)
{
	switch (nPass)
	{
	case 0: //Simulate
	{
		PARAMS Temp = m_Params;
		D3D11_MAPPED_SUBRESOURCE dms;
		m_pOwner->GetContext()->Map(m_vBuffers[0], 0,
			D3D11_MAP_WRITE_DISCARD, 0, &dms);
		memcpy(dms.pData, &Temp, sizeof(PARAMS));
		m_pOwner->GetContext()->Unmap(m_vBuffers[0], 0);
		m_pOwner->GetContext()->CSSetConstantBuffers(0,
			(UINT)m_vBuffers.size(), &m_vBuffers[0]);
		//Salida grafica
		m_pOwner->GetContext()->CSSetUnorderedAccessViews(1, 1, &m_vUAVs[0],0);
		//Entrada/Salida PARTICULAS
		m_pOwner->GetContext()->CSSetUnorderedAccessViews(0, 1, &m_vUAVs[1], 0);
		//Instalar el shader
		m_pOwner->GetContext()->CSSetShader(m_vShaders[0], 0, 0);
		//Dispatch
		m_pOwner->GetContext()->Dispatch((m_Particles.size() + 255) / 256, 1, 1);
	}
	break;

	case 1: //Plot
		//Salida grafica
		m_pOwner->GetContext()->CSSetUnorderedAccessViews(1, 1, &m_vUAVs[0], 0);
		//Entrada/Salida PARTICULAS
		m_pOwner->GetContext()->CSSetUnorderedAccessViews(0, 1, &m_vUAVs[1], 0);
		//Instalar el shader
		m_pOwner->GetContext()->CSSetShader(m_vShaders[1], 0, 0);
		//Dispatch
		m_pOwner->GetContext()->Dispatch((m_Particles.size() + 255) / 256, 1, 1);
	}
}

long CParticleSystem::GetNumberOfPasses()
{
	return 2;
}

void CParticleSystem::OnEnd()
{
	//Descargar el buffer estructurado del GPU -> m_Particles
}
