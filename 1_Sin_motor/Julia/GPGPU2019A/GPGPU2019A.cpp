// GPGPU2019A.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "CDXManager.h"

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
struct PARAMS
{
	float Params[4];
	float Move[2];
}g_Params = { {0.1f, 0.1f, 1.0f, 100.0f} , { 0.0f, 0.0f } };

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
	g_pCS = g_DXManager.CompileCS(L"..\\Shaders\\julia.hlsl", "main");
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



	//g_pCS = g_DXManager.CompileCS(L"..\\Shaders\\Circulo.hlsl", "main");
	//g_pCS = g_DXManager.CompileCS(L"..\\Shaders\\Triangulo.hlsl", "main");

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
	static bool bLeft, bRight, bUp, bDown, bA, bW, bS, bD, bJ, bL, bI, bK;
	switch (message)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_LEFT:bLeft = true; break;
		case VK_RIGHT:bRight = true; break;
		case VK_DOWN:bDown = true; break;
		case VK_UP:bUp = true; break;
		case 0x41:bA = true; break; //a
		case 0x44:bD = true; break; //d
		case 0x57:bW = true; break; // w
		case 0x53:bS = true; break;  //s
		case 0x4A:bJ = true; break; // j
		case 0x4C:bL = true; break;  //l
		case 0x49:bI = true; break; // i
		case 0x4B:bK = true; break;  //k
		}
		break;
	case WM_KEYUP:
		switch (wParam)
		{
		case VK_LEFT:bLeft = false; break;
		case VK_RIGHT:bRight = false; break;
		case VK_DOWN:bDown = false; break;
		case VK_UP:bUp = false; break;
		case 0x41:bA = false; break; //a
		case 0x44:bD = false; break; //d
		case 0x57:bW = false; break; // w
		case 0x53:bS = false; break;  //s
		case 0x4A:bJ = false; break; // j
		case 0x4C:bL = false; break;  //l
		case 0x49:bI = false; break; // i
		case 0x4B:bK = false; break;  //k
		}
		break;
	case WM_TIMER:
		switch (wParam)
		{
		case 1:
			{
			D3D11_MAPPED_SUBRESOURCE dms;
				if (bLeft)
				{
					g_Params.Params[0] -= 0.1f;
				}
				if (bRight)
				{
					g_Params.Params[0] += 0.1f;
				}
				if (bUp)
				{
					g_Params.Params[1] += 0.1f;
				}
				if (bDown)
				{
					g_Params.Params[1] -= 0.1f;
				}
				if (bA && (0 < g_Params.Params[2]))
				{
					g_Params.Params[2] -= 1.0f;
				}
				if (bD && (254 > g_Params.Params[2]))
				{
					g_Params.Params[2] += 1.0f;
				}
				if (bW && (10 < g_Params.Params[3]))
				{
					g_Params.Params[3] -= 10.0f;
				}
				if (bS && (1000 > g_Params.Params[3]))
				{
					g_Params.Params[3] += 10.0f;
				}
				if (bJ)
				{
					g_Params.Move[0] += 10.0f;
				}
				if (bL)
				{
					g_Params.Move[0] -= 10.0f;
				}
				if (bI)
				{
					g_Params.Move[1] += 10.0f;
				}
				if (bK)
				{
					g_Params.Move[1] -= 10.0f;
				}
				if (g_pCB)
				{
					g_DXManager.m_pCtx->Map(g_pCB, 0, D3D11_MAP_WRITE_DISCARD, 0, &dms); // se mapea memoria de APP a driver qeu tiene acceso al buffer
					memcpy(dms.pData, &g_Params, sizeof(PARAMS)); // pasamos info a ese mapeo o mirror
					g_DXManager.m_pCtx->Unmap(g_pCB, 0); //liberamos el map y eso permite a que se actualice el buffer
														// y asi actializara 
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
			DestroyWindow(hWnd);
		}
		return 0;
	case WM_PAINT:
		{
		// LECTura y escritura UAV
		ID3D11UnorderedAccessView* pUAV = nullptr;
		// Recurso
		ID3D11Texture2D* pTexture2D = nullptr;
		g_DXManager.m_pSwapChain->GetBuffer(0,IID_ID3D11Texture2D, (void**)&pTexture2D);
		// Conectar recurso con View
		g_DXManager.m_pDev->CreateUnorderedAccessView(pTexture2D, nullptr, &pUAV);
		// Conectar el View con el Shader socket, numero de views, arreglo de views, 0
		g_DXManager.m_pCtx->CSSetUnorderedAccessViews(0,1,&pUAV, 0);
		// Elejir shader
		g_DXManager.m_pCtx->CSSetShader(g_pCS, 0, 0);
		// Empieza a trabajar Total en x / pixel por grupo en x, Total en y / pixel por grupo en y, el 15 es para 
		// asegurar que el grupo se complete, ajustar residuos, (n+(g-1)/g)
		// El tamaño del grupo se define en el HLSL numthreads
		// Se puede configurar lo que gustes ANTES DEL DISPATCH
		g_DXManager.m_pCtx->CSSetConstantBuffers(0, 1, &g_pCB);

		g_DXManager.m_pCtx->Dispatch((1024+15) / 16, (1024 + 15) / 16,1);
		g_DXManager.m_pSwapChain->Present(1, 0);
		SAFE_RELEASE(pUAV);
		SAFE_RELEASE(pTexture2D);
		ValidateRect(hWnd,0);

		}
	return 0;
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

