// GPGPU2019A.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "CDXManager.h"
#include "CParticleSystem.h"
#include <stdio.h>
#include <fstream>

/* Calling conventions */
/* Es la forma en la que la funcion invocadora, pasa
los parámetros a la pila y recoje los resultados tras
la ejecución de una función invocada */

// __cdecl Caller Pushes / Caller Pops (Variable Args)
// __stdcall / __pascal Caller Pushes / Callee Pops

HINSTANCE g_hInstance;
CDXManager g_DXManager;
ID3D11ComputeShader* g_pCS;
ID3D11Buffer* g_pCB; // Constant buffer
CParticleSystem g_PS; // Particle system
streampos sizeOut = 0;

char pszFileName[] = { "..\\texto.txt" };

struct PARAMS
{
	float CircleParams[4];
}g_Params = { {512.0f, 512.0f, 1.0f/ 1024.0f, 512.0f*512.0f} };

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
	//Paso 1

	//CDXManager Manager;
	//IDXGIAdapter* pTest = Manager.ChoseAdapter(nullptr);
	//if (pTest) pTest->Release();

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
	//ShowWindow(hWnd, nCmdShow);
	//IDXGIAdapter* pAdapter = g_DXManager.ChoseAdapter(hWnd);
	IDXGIAdapter* pAdapter = g_DXManager.ChoseAdapter(NULL);
	if (!pAdapter)
	{
		g_DXManager.Initialize(nullptr, hWnd, true, true);

	}
	else
	{
		g_DXManager.Initialize(pAdapter, hWnd, false, true);
	}
	SAFE_RELEASE(pAdapter);
	const char* apszEntry2[] = {"Simulate", "Plot"};
	g_PS.Initialize(&g_DXManager, L"..\\Shaders\\Particles.hlsl", apszEntry2, 2);
	// Crear particulas
	fstream textFile;
	streampos sizetxt = 0;
	char* memblock = nullptr;

	printf("Loading %s...\n", pszFileName);
	vector<CParticleSystem::PARTICLE> Test;
	textFile.open(pszFileName, ios::in | ios::binary| ios::ate);
	if (!textFile.is_open())
	{
		printf("Error: Unable to open file %s\n", pszFileName);
		return NULL;
	}
	else
	{
		sizetxt = textFile.tellg();
		memblock = new char[sizetxt];
		textFile.seekg(0, ios::beg);
		textFile.read(memblock, sizetxt);
		textFile.close();
	}
	Test.resize(sizetxt);
	sizeOut = sizetxt;
	for (int i = 0; i < Test.size(); i++)
	{
		Test[i].letter = (int)memblock[i];
		Test[i].index = i;
	}
	g_PS.m_Params.Gravity = { 0,10,0,0 }; // son pixeles obviamente
	g_PS.m_Params.dt = 0.01;
	g_PS.OnBegin(&Test[0], Test.size());

	D3D11_BUFFER_DESC dbd;
	memset(&dbd, 0, sizeof(dbd));
	dbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	dbd.ByteWidth = 16 * ((sizeof(PARAMS) + 15) / 16); 
	// COnstant buffer solo puede tener tamaño multiplo de 16 bytes
	// NOta aqui va a haber comm entre CPU y GPU y es por PCIe y el minimo de comm de PCIe es 16 bytes
	dbd.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
	/*
		USAGE		CPU		GPU
		DYNAMIC		W		R
		DEFAULT		N/A		R/W
		STAGING		R/W		COPY	(DMA PCI, DMA CPU, DMA GPU ESOS SON LOS 3 BUSSES)
		INMUTABLE	W/ONCE	R
		
	*/
	dbd.Usage = D3D11_USAGE_DYNAMIC;
	D3D11_SUBRESOURCE_DATA dsd;
	dsd.pSysMem = &g_Params;
	dsd.SysMemPitch = 0; //2D, row lenght mas padding
	dsd.SysMemSlicePitch = 0; //3D, matrix/plane lenght + paddin
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
	switch (message)
	{
	static bool bLeft, bRight, bUp, bDown;
	case WM_KEYDOWN:
		break;
	case WM_KEYUP:
		break;
	case WM_TIMER:
		switch (wParam)
		{
		case 1:
			{
				if (g_pCB)
				{
					g_PS.OnCompute(0);
					InvalidateRect(hWnd, nullptr, false);
				}
			}
		}
		break;
	case WM_CREATE:
		SetTimer(hWnd, 1, 20, nullptr);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_CLOSE:
		if (IDYES == MessageBox(hWnd, L"¿Desea salir?", L"Salir",
			MB_YESNO | MB_ICONQUESTION))
		{
			g_PS.OnEndTxt(sizeOut);
			DestroyWindow(hWnd);
		}
		return 0;
	case WM_PAINT:
		{
			g_PS.OnCompute(1);
			g_DXManager.GetContext()->Flush();
			g_DXManager.GetSwapChain()->Present(1,0);
			g_DXManager.GetContext()->ClearState();
			ValidateRect(hWnd,0);
		}
	return 0;
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

