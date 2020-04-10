// GPGPU2019A.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "CDXManager.h"
#include "CImageProcessor.h"

/* Calling conventions */
/* Es la forma en la que la funcion invocadora, pasa
los parámetros a la pila y recoje los resultados tras
la ejecución de una función invocada */

// __cdecl Caller Pushes / Caller Pops (Variable Args)
// __stdcall / __pascal Caller Pushes / Callee Pops

HINSTANCE g_hInstance;
CDXManager g_DXManager;
ID3D11ComputeShader* g_pCS;
CImageProcessor g_IP; // Image Processor

const char imageFile[] = {"..\\Image.bmp"};

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
	const char* pszEntry[] = { "LinearTransform" };
	g_IP.Initialize(&g_DXManager, L"..\\Shaders\\ImagePocessor.hlsl", pszEntry, 1 );
	g_IP.OnBegin(nullptr,0);
	g_IP.LoadPicture(&imageFile[0]);

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
	case WM_KEYDOWN:
		break;
	case WM_KEYUP:
		break;
	case WM_TIMER:
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
			DestroyWindow(hWnd);
		}
		return 0;
	case WM_PAINT:
		{
		g_IP.OnCompute(0);
		g_DXManager.GetContext()->Flush();
		g_DXManager.GetSwapChain()->Present(1,0);
		g_DXManager.GetContext()->ClearState();
		ValidateRect(hWnd,0);

		}
	return 0;
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

