//========================================================================
// App.cpp
//
// This code is part of Ubisoft Programmer Test 
//
// Coded by Muralev Evgeny
//========================================================================

#include "BlurTestStd.h"
#include "App.h"

App::App()
{
	m_pTimer = new Timer();
}

App::~App()
{
	SAFE_RELEASE(m_pbbRenderTargetView);
	SAFE_RELEASE(m_SwapChain);
	SAFE_RELEASE(m_d3d11DeviceContext);
	SAFE_RELEASE(m_d3d11Device);

	delete m_pTimer;
}

bool App::InitWindow(LPCWSTR windowName, HINSTANCE hInstance, int showWnd, int iWidth, int iHeight, bool bWindowed)
{
	//assign class members
	m_hInstance = hInstance;
	m_bWindowed = bWindowed;
	m_windowClassName = L"AppClass";
	m_windowName = windowName;

	///////////////////////////////
	//fill the WNDCLASS structure
	// contains window params
	WNDCLASSEX wc;
	
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = (WNDPROC)App::WndProc;
	wc.cbClsExtra = NULL;
	wc.cbWndExtra = NULL;
	wc.hInstance = m_hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 2);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = m_windowClassName;
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION); 

	//register window
	if (!RegisterClassEx(&wc))
	{
		MessageBox(NULL, L"Error registering window class",	
			L"Error", MB_OK | MB_ICONERROR);
		return false;
	}

	HRESULT hr = HRESULT_FROM_WIN32(GetLastError());

	//create window
	m_hwnd = CreateWindowEx(
		NULL,
		m_windowClassName,
		m_windowName,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		iWidth, iHeight,
		NULL,
		NULL,
		m_hInstance,
		NULL
		); 

	//check wether window was created correctly
	if (!m_hwnd)
	{
		MessageBox(NULL, L"Error creating window",
			L"Error", MB_OK | MB_ICONERROR);
		return false;
	}

	//show and update window
	ShowWindow(m_hwnd, showWnd);
	UpdateWindow(m_hwnd);

	return true;
}

bool App::InitDirectX11(int iWidth, int iHeight)
{
	//===============================================
	//Describe back buffer structure
	DXGI_MODE_DESC backBufferDesc;
	ZeroMemory(&backBufferDesc, sizeof(DXGI_MODE_DESC));

	backBufferDesc.Width = iWidth;
	backBufferDesc.Height = iHeight;
	backBufferDesc.RefreshRate.Numerator = 60;
	backBufferDesc.RefreshRate.Denominator = 1;
	backBufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	backBufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	backBufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	//=============================
	//Describe SwapChain structure
	DXGI_SWAP_CHAIN_DESC swapChainDesc; 
	ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC)); //zero memory for the structure

	swapChainDesc.BufferDesc = backBufferDesc; //set created back buffer
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; //use back buffer as render target
	swapChainDesc.BufferCount = 1;
	swapChainDesc.OutputWindow = m_hwnd; 
	swapChainDesc.Windowed = TRUE; 
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;


	//=============================
	//Create swap chain
	D3D_FEATURE_LEVEL pLevel = D3D_FEATURE_LEVEL_11_0; //set feature desired feature level
	HRESULT hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, D3D11_CREATE_DEVICE_DEBUG, &pLevel, 1,
		D3D11_SDK_VERSION, &swapChainDesc, &m_SwapChain, &m_d3d11Device, NULL, &m_d3d11DeviceContext);
	VALID(hr);

	//Get BackBuffer texture
	ID3D11Texture2D* BackBuffer;
	hr = m_SwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), (void**)&BackBuffer );
	VALID(hr);

	//Create Render Target
	hr = m_d3d11Device->CreateRenderTargetView( BackBuffer, NULL, &m_pbbRenderTargetView );
	VALID(hr);
	BackBuffer->Release();

	//Set Render Target
	m_d3d11DeviceContext->OMSetRenderTargets( 1, &m_pbbRenderTargetView, NULL );

	return true;
}

bool App::VInitSimulation()
{
	//////////////////////////////////////////
	//Initialize constant buffers
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));

	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(MatrixBuffer)*3;

	HRESULT hr = m_d3d11Device->CreateBuffer(&bufferDesc, 0, &m_pcbMatrix);
	VALID(hr);

	/////////////////////////////////////////
	//Create camera matrices
	/////////////////////////////////////////
	m_matrixView = XMMatrixLookAtLH(m_vCameraPos, m_vCameraTarget, m_vCameraUp);
	m_matrixProj = XMMatrixPerspectiveFovLH(m_fFovY, m_fAspectRatio, m_fNearZ, m_fFarZ);
	m_matrixOrtho = XMMatrixOrthographicLH(SCREEN_WIDTH, SCREEN_HEIGHT, m_fNearZ, m_fFarZ);

	return true;
}

void App::InitAndSetStandardViewport()
{
	ZeroMemory(&m_viewport, sizeof(D3D11_VIEWPORT));

	m_viewport.TopLeftX = 0;
	m_viewport.TopLeftY = 0;
	m_viewport.Width = SCREEN_WIDTH;
	m_viewport.Height = SCREEN_HEIGHT;
	m_d3d11DeviceContext->RSSetViewports(1, &m_viewport);
}

LRESULT CALLBACK App::WndProc(HWND hWnd, int msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE)
		{
			DestroyWindow(hWnd);
		}
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	};

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

void App::VUpdate(const float elapsedTime, const float totalTime)
{
}

void App::VRender(const float elapsedTime, const float totalTime)
{
}

int App::Run()
{
	MSG msg;
	ZeroMemory(&msg, sizeof(msg));

	while(true)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			switch(msg.message)
			{
			case WM_QUIT:
				DestroyWindow(m_hwnd);
				return msg.wParam;
			case WM_KEYDOWN:
				VKeyPressed(msg.wParam);
				break;
			}

		TranslateMessage(&msg);
		DispatchMessage(&msg);
		}
		else
		{
			m_pTimer->Tick();

			VUpdate(m_pTimer->GetDeltaTime(), m_pTimer->GetGameTime());
			VRender(m_pTimer->GetDeltaTime(), m_pTimer->GetGameTime());
		}
	}

	return msg.wParam;
}