// GPGPU2019A.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "CDXManager.h"
#include "CImageProcessor.h"
#include "CParticleSystem.h"
#include "CVectorSum.h"
#include <fstream>
using namespace std;

/* Calling conventions */
/* Es la forma en la que la funcion invocadora, pasa
los parámetros a la pila y recoje los resultados tras
la ejecución de una función invocada */

// __cdecl Caller Pushes / Caller Pops (Variable Args)
// __stdcall / __pascal Caller Pushes / Callee Pops

HINSTANCE g_hInstance;
CDXManager g_DXManager;
ID3D11ComputeShader* g_pCS;
ID3D11Buffer* g_pCB; //Constant Buffer
CImageProcessor g_IP; //Image Processor
CParticleSystem g_PS; //Particle System
CVectorSum g_VS; //Vector sum
struct PARAMS
{
	float CircleParams[4];
}g_Params = { {100.0f,100.0f,1.0f / 1024.0f,512.0f * 512.0f} };
/* 
	Paso 1: Registrar al menos una clase de ventana 
	        Nombre de Clase
			Un procedimiento ventana (puntero a una función)
			(otros datos)

			-> OS.
    Paso 2: Crear al menos uno ventana a partir de una clase registrada.
			Nombre de Clase
			Titulo Ventana
			Tamaño y posición en pixeles
			Estilo visual y comportamiento básico

			-> OS -> Ventana
	Paso 3: Implementar el bucle de mensajes. Cada hilo implementa
	        una cola de mensajes, donde los eventos del sistema y 
			periféricos depositarán los sucesos.
*/

LRESULT CALLBACK WinProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance, //0
	char* pszCmdLine, //Linea de comandos
	int nCmdShow)
{
	/*CDXManager Manager;
	IDXGIAdapter* pTest = Manager.ChooseAdapter(nullptr);
	if (pTest) pTest->Release();*/


	//Paso 1
	g_hInstance = hInstance;
	WNDCLASSEX wnc;
	memset(&wnc, 0, sizeof(WNDCLASSEX));
	wnc.cbSize = sizeof(WNDCLASSEX);
	wnc.hInstance = hInstance;
	wnc.lpszClassName = L"GPGPU2019A";
	wnc.lpfnWndProc = WinProc;
	wnc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	RegisterClassEx(&wnc);
	//Paso 2
	HWND hWnd =
		CreateWindowEx(WS_EX_OVERLAPPEDWINDOW,
			L"GPGPU2019A",
			L"Programación de procesadores gráficos 2019A",
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT, CW_USEDEFAULT,
			CW_USEDEFAULT, CW_USEDEFAULT,
			NULL,
			NULL,
			hInstance,
			NULL);
	IDXGIAdapter* pAdapter = g_DXManager.ChooseAdapter(NULL);
	if (!pAdapter)
		g_DXManager.Initialize(nullptr, hWnd, true, true);
	else
		g_DXManager.Initialize(pAdapter, hWnd, false, true);
	SAFE_RELEASE(pAdapter);
	const char* apszEntry[] = { "LinearTransform","Not","Map" };
	g_IP.Initialize(&g_DXManager, L"..\\Shaders\\ImageProcessor.hlsl",
		apszEntry, 3);
	g_IP.OnBegin(nullptr,0);
	g_IP.LoadPicture("..\\Imagen.bmp");
	const char* apszEntry2[] = { "Simulate","Plot" };
	g_PS.Initialize(&g_DXManager, L"..\\Shaders\\Particles.hlsl", apszEntry2, 2);
	//Creamos particular
	vector<CParticleSystem::PARTICLE> Test;
	Test.resize(1000);
	for (int i = 0; i < Test.size(); i++)
	{
		float theta = 2.0f*3.141592f*rand() / RAND_MAX;
		float v = 10.0f+100.0f*rand()/RAND_MAX ;
		Test[i].Position = { 512,512,0,1 };
		Test[i].Velocity = { v*cos(theta),v*sin(theta),0,0 };
		Test[i].Force = { 0,0,0,0 };
		Test[i].InvMass = 1;
	}
	g_PS.m_Params.Gravity = { 0,10,0,0 };
	g_PS.m_Params.dt = 0.01;
	g_PS.OnBegin(&Test[0], Test.size());
	
	/***************************************/
	const char* SumEntry[] = { "Sum" };
	g_VS.Initialize(&g_DXManager, L"..\\Shaders\\Sum.hlsl", SumEntry, 1);
	//VECTOR4D Test2[2] = { {1,2,3,4},{4,3,2,1} }; // Espero como resultado un 5 5 5 5
	vector<VECTOR4D> Test2;
	/*
	Test2.resize(512);
	for (int i = 0; i < Test2.size(); i++) {
		Test2[i] = { (float)i,(float)i,(float)i,(float)i };
	}
	g_VS.OnBegin(&Test2[0], Test2.size());

	*/
	fstream monkey;
	monkey.open("..\\Data\\monkey.vertex", ios::in | ios::binary);
	long nVertex;
	monkey.read((char*)&nVertex, sizeof(long));
	Test2.resize(nVertex);
	monkey.read((char*)&Test2[0], nVertex * sizeof(VECTOR4D));

	g_VS.OnBegin(&Test2[0], Test2.size());
	g_VS.OnCompute(0);
	g_VS.OnEnd();
	VECTOR4D Avg = { 0,0,0,0 };

	for (int i = 0; i < g_VS.m_Results.size(); i++) {
		Avg.x = g_VS.m_Results[i].x;
		Avg.y = g_VS.m_Results[i].y;
		Avg.z = g_VS.m_Results[i].z;
		Avg.w = g_VS.m_Results[i].w;
	}

	Avg.x /= Test2.size();
	Avg.y /= Test2.size();
	Avg.z /= Test2.size();
	Avg.w /= Test2.size();

	/***************************************/
	g_pCS=g_DXManager.CompileCS(L"..\\Shaders\\Default.hlsl", "main");
	D3D11_BUFFER_DESC dbd;
	memset(&dbd, 0, sizeof(dbd));
	dbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	dbd.ByteWidth = 16 * ((sizeof(PARAMS)+ 15) / 16);  /* 16 byte multiple of */
	dbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	/*
	    USAGE       CPU     GPU
		DYNAMIC      W       R
		DEFAULT     N/A     R/W
		STAGING     R/W     Copy (DMA PCI DMA CPU DMA GPU)
		INMUTABLE  W/Once    R
	*/
	dbd.Usage = D3D11_USAGE_DYNAMIC;
	D3D11_SUBRESOURCE_DATA dsd;
	dsd.pSysMem = &g_Params;
	dsd.SysMemPitch = 0; //2D row length bytes + padding
	dsd.SysMemSlicePitch = 0;  //3D plane length + padding
	g_DXManager.m_pDev->CreateBuffer(&dbd, &dsd, &g_pCB);
	//Paso 3 
	ShowWindow(hWnd, nCmdShow);
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		DispatchMessage(&msg);
	}
	return 0;
}

LRESULT CALLBACK WinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static bool bUp, bDown, bLeft, bRight;

	switch (message)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_LEFT:  bLeft = true; break;
		case VK_RIGHT: bRight = true; break;
		case VK_DOWN:  bDown = true; break;
		case VK_UP:    bUp = true; break;
		}
		break;
	case WM_KEYUP:
		switch (wParam)
		{
		case VK_LEFT:  bLeft = false; break;
		case VK_RIGHT: bRight = false; break;
		case VK_DOWN:  bDown = false; break;
		case VK_UP:    bUp = false; break;
		}
		break;
	case WM_TIMER:
		switch (wParam)
		{
		case 1:
			{
				D3D11_MAPPED_SUBRESOURCE dms;
				if (bLeft)  g_Params.CircleParams[0] += 10.0f;
				if (bRight) g_Params.CircleParams[0] -= 10.0f;
				if (bUp)    g_Params.CircleParams[1] += 10.0f;
				if (bDown)  g_Params.CircleParams[1] -= 10.0f;
				g_IP.m_Params.M = RotationZ(g_Params.CircleParams[0] / 40);
				if (g_pCB)
				{
					g_DXManager.m_pCtx->Map(g_pCB, 0, D3D11_MAP_WRITE_DISCARD, 0, &dms);
					memcpy(dms.pData, &g_Params, sizeof(PARAMS));
					g_DXManager.m_pCtx->Unmap(g_pCB, 0);
					g_PS.OnCompute(0);
					InvalidateRect(hWnd, nullptr, false);
				}
				
			}
			break;
		}
		break;
	case WM_CREATE:
		SetTimer(hWnd, 1, 20,nullptr);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_CLOSE:
		if (IDYES == MessageBox(hWnd, L"¿Desea salir?", L"Salir",
			MB_YESNO | MB_ICONQUESTION))
		{
			DestroyWindow(hWnd);
		}
		return 0;
	case WM_PAINT:
		{
		/*
		ID3D11UnorderedAccessView* pUAV = nullptr;
		ID3D11Texture2D* pTexture2D = nullptr;
		g_DXManager.m_pSwapChain->GetBuffer(0, IID_ID3D11Texture2D, (void**)&pTexture2D);
		g_DXManager.m_pDev->CreateUnorderedAccessView(pTexture2D, nullptr, &pUAV);
		g_DXManager.m_pCtx->CSSetUnorderedAccessViews(0, 1, &pUAV, 0);
		g_DXManager.m_pCtx->CSSetShader(g_pCS, 0, 0);
		g_DXManager.m_pCtx->CSSetConstantBuffers(0, 1, &g_pCB);
		g_DXManager.m_pCtx->Dispatch(1024/16, 1024/16, 1); // (n+(g-1))/g)
		g_DXManager.m_pSwapChain->Present(1, 0);
		SAFE_RELEASE(pUAV);
		SAFE_RELEASE(pTexture2D);*/
		//g_IP.OnCompute(0);

		ID3D11Texture2D *pTemp = nullptr, *pOldOutput;
		D3D11_TEXTURE2D_DESC dtd;
		ID3D11UnorderedAccessView* pUAV = nullptr, *pOldUAV;
		ID3D11ShaderResourceView* pSRV = nullptr, *pOldSRV;

		g_PS.m_vUAVs[0]->GetResource((ID3D11Resource**)&pOldOutput);
		pOldOutput->GetDesc(&dtd);
		dtd.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
		pOldOutput->Release();
		g_DXManager.GetDevice()->CreateTexture2D(&dtd, nullptr, &pTemp);
		g_DXManager.GetDevice()->CreateUnorderedAccessView(pTemp, nullptr, &pUAV);
		g_DXManager.GetDevice()->CreateShaderResourceView(pTemp, nullptr, &pSRV);
		// Operacion 1
		// rescatar la salida gráfica
		pOldUAV = g_PS.m_vUAVs[0];
		g_PS.m_vUAVs[0] = pUAV;
		g_PS.OnCompute(1);
		//desconecta todos los recursos de entrada y salida 
		g_DXManager.GetContext()->ClearState();
		// Operacion 2
		pOldSRV = g_IP.m_vSRVs[0];
		g_IP.m_vSRVs[0] = pSRV;
		g_IP.OnCompute(0);

		SAFE_RELEASE(pTemp);
		SAFE_RELEASE(pUAV);
		SAFE_RELEASE(pSRV);

		g_PS.m_vUAVs[0] = pOldUAV;
		g_IP.m_vSRVs[0] = pOldSRV;
	
		g_DXManager.GetSwapChain()->Present(1, 0);
		ValidateRect(hWnd, 0);
		g_PS.OnEnd();
		}
		return 0;
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

