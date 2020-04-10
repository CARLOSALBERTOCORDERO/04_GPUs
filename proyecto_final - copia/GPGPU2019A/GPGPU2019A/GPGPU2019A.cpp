// GPGPU2019A.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "CDXManager.h"
#include "CImageProcessor.h"
#include "CParticleSystem.h"
#include "ConfigScale.h"
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

int nRockets = N_ROCKETS_50;
int nParticlesRocket = PARTICLES_PER_ROCKET_1000;

struct PARAMS
{
	float Params[1];
}g_Params = { {0.0f} };
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
	const char* apszEntry[] = { "Soft3x3" };
	g_IP.Initialize(&g_DXManager, L"..\\Shaders\\ImageProcessor.hlsl",
		apszEntry, g_IP.GetNumberOfPasses());
	g_IP.OnBegin(nullptr, 0);
	g_IP.LoadPicture("..\\Imagen.bmp");
	const char* apszEntry2[] = { "Simulate","Plot" };
	g_PS.Initialize(&g_DXManager, L"..\\Shaders\\Particles.hlsl", apszEntry2, 2);
	//Creamos particular
	vector<CParticleSystem::PARTICLE> Test;
	Test.resize(nParticlesRocket * nRockets);
	for (int i = 0; i < nRockets; i++)
	{
		float positionX = 1024 * rand() / RAND_MAX;
		float positionY = 1024 * rand() / RAND_MAX;
		float red = (float)rand() / RAND_MAX;
		float green = (float)rand() / RAND_MAX;
		float blue = (float)rand() / RAND_MAX;

		for (int j = 0; j < nParticlesRocket; j++)
		{
			int index = (i * nParticlesRocket) + j;
			float theta = 2.0f*3.141592f*rand() / RAND_MAX;
			float v = 10.0f + 100.0f*rand() / RAND_MAX;
			Test[index].Position = { positionX,positionY,0,1 };
			Test[index].Velocity = { v*cos(theta),v*sin(theta),0,0 };
			Test[index].Force = { 0,0,0,0 };
			Test[index].Color = { red,green,blue,1};
			Test[index].InvMass = 1;
		}
	}
	g_PS.m_Params.Gravity = { 0,10,0,0 };
	g_PS.m_Params.dt = 0.01f;
	g_PS.OnBegin(&Test[0], Test.size());

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
	int startTime;

	switch (message)
	{
	case WM_KEYDOWN:
		break;
	case WM_KEYUP:
		break;
	case WM_TIMER:
		switch (wParam)
		{
		case 1:
			{
				D3D11_MAPPED_SUBRESOURCE dms;
				if (g_pCB)
				{
					g_PS.OnCompute(0);
					InvalidateRect(hWnd, nullptr, false);
				}
			}
			break;
		case 2:
			{
				D3D11_MAPPED_SUBRESOURCE dms;
				if (g_pCB)
				{
					g_PS.OnCompute(2);
					InvalidateRect(hWnd, nullptr, false);
					startTime = MAX_START_ROCKET_TIME_MS_10000 * rand() / RAND_MAX;
					SetTimer(hWnd, 2, startTime, nullptr);
				}
			}
			break;
		}
		break;
	case WM_CREATE:
		startTime = 40 + (1000 * rand() / RAND_MAX);
		SetTimer(hWnd, 1, 20,nullptr);
		SetTimer(hWnd, 2, startTime, nullptr);
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
		static ID3D11Texture2D* pTemp=nullptr,*pOldOutput, *pTemp1;
		static ID3D11UnorderedAccessView* pUAV = nullptr, *pOldUAV, *pUAV1;
		static ID3D11ShaderResourceView*  pSRV = nullptr, *pOldSRV, *pSRV1;
		if (!pTemp)
		{
			D3D11_TEXTURE2D_DESC dtd;
			g_PS.m_vUAVs[0]->GetResource((ID3D11Resource**)&pOldOutput);
			pOldOutput->GetDesc(&dtd);
			dtd.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
			pOldOutput->Release();
			g_DXManager.GetDevice()->CreateTexture2D(&dtd, nullptr, &pTemp);
			g_DXManager.GetDevice()->CreateUnorderedAccessView(pTemp, nullptr, &pUAV);
			g_DXManager.GetDevice()->CreateShaderResourceView(pTemp, nullptr, &pSRV);

			g_DXManager.GetDevice()->CreateTexture2D(&dtd, nullptr, &pTemp1);
			g_DXManager.GetDevice()->CreateUnorderedAccessView(pTemp1, nullptr, &pUAV1);
			g_DXManager.GetDevice()->CreateShaderResourceView(pTemp1, nullptr, &pSRV1);

		}
		VECTOR4D Zero = {0,0,0,0};
		//g_DXManager.GetContext()->ClearUnorderedAccessViewFloat(pUAV, Zero.v);
		//Operación 1
		pOldUAV = g_PS.m_vUAVs[0];
		g_PS.m_vUAVs[0] = pUAV;
		g_PS.OnCompute(1);

		g_DXManager.GetContext()->ClearState();
		
		//Operacion 2

		pOldSRV = g_IP.m_vSRVs[0];
		auto pOldUAVIP = g_IP.m_vUAVs[0];
		for (int i = 0 ; i < FILTERING_TIMES; i++)
		{
			ID3D11ShaderResourceView* ptSRV = (!(i%2))? pSRV : pSRV1;
			ID3D11UnorderedAccessView* ptUAV = (!(i % 2)) ? pUAV1 : pUAV;
			g_IP.m_vSRVs[0] = ptSRV;
			g_IP.m_vUAVs[0] = ptUAV;
			g_IP.OnCompute(0);
		}
		g_DXManager.GetContext()->CopyResource(pOldOutput, pTemp);
		
		g_IP.m_vUAVs[0] = pOldUAVIP;
		g_PS.m_vUAVs[0] = pOldUAV;
		g_IP.m_vSRVs[0] = pOldSRV;
		g_DXManager.GetSwapChain()->Present(1, 0);
		ValidateRect(hWnd, 0);
		}
		return 0;
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

