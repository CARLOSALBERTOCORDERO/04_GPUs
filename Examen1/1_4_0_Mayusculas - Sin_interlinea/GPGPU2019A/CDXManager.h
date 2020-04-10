#pragma once
#include <dxgi.h>
#include <d3d11.h>
#include "matrix4D.h"

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(x) {if((x)) {(x)->Release(); (x)=nullptr;}}
#endif



class CDXManager
{
public:
	ID3D11Device* m_pDev; //D3D Device (Rosoure factory or resource provider)
	ID3D11DeviceContext* m_pCtx; //D3D Context (functional, contextos para cado caso)
	IDXGISwapChain* m_pSwapChain; // DXGI Swapchain (Cadena de intercambio)

public:
	IDXGIAdapter* ChoseAdapter(HWND hWnd); // Para considerar el device como GPU
	bool Initialize(IDXGIAdapter* pAdapter, HWND hWnd, bool bUseCPU, bool bUseGraphicsOutput);
	IDXGISwapChain* GetSwapChain() { return m_pSwapChain; };
	void Uninitilize();
	ID3D11ComputeShader* CompileCS(const wchar_t* pwszFileName, const  char* pszEntryPoint);
	ID3D11Device* GetDevice() { return m_pDev; }
	ID3D11DeviceContext* GetContext() { return m_pCtx; }
	ID3D11Texture2D* LoadTexture(
		const char* pszFileName,		//The file name
		unsigned long MipMapLevels, //Number of mipmaps to generate, -1 automatic (int)log2(min(sizeX,sizeY))+1 levels
		ID3D11ShaderResourceView** ppSRV, //The Shader Resource View
		float(*pAlphaFunction)(float r, float g, float b),  //Operates pixel's rgb channels to build an alpha channel (normalized), can be null
		VECTOR4D(*pColorFunction)(const VECTOR4D& Color));

	CDXManager();
	~CDXManager();
};

