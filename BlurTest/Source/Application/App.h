//========================================================================
// App.h
//
// This code is part of Ubisoft Programmer Test 
//
// Coded by Muralev Evgeny
//========================================================================

#pragma once 

#include "../Time/Timer.h"

class App
{
public:
	App();
	virtual ~App();

	/***** Initializing methods *****/
	bool			InitWindow		(LPCWSTR windowName, HINSTANCE hInstance, int showWnd, int iWidth, int iHeight, bool bWindowed);
	bool			InitDirectX11	(int iWidth, int iHeight);
	virtual bool	VInitSimulation	();

	//win32 message processing callback
	static LRESULT CALLBACK WndProc(HWND hWnd, int msg, WPARAM wParam, LPARAM lParam);
	
	/***** Simulation *****/
	virtual void VUpdate(const float elapsedTime, const float totalTime);
	virtual void VRender(const float elapsedTime, const float totalTime);

	int Run(); //game loop

	/***** Accessors *****/
	ID3D11Device*			GetDevice()					const	{ return m_d3d11Device; }
	ID3D11DeviceContext*	GetImmediateContext()		const	{ return m_d3d11DeviceContext; }
	ID3D11RenderTargetView* GetBackBufferRenderTarget() const	{ return m_pbbRenderTargetView; }

	ID3D11Buffer*	GetMatrixConstantBuffer()		const { return m_pcbMatrix; }
	ID3D11Buffer*	GetMatrixTexConstantBuffer()	const { return m_pcbMatrixTex; }

	XMMATRIX GetView()		const { return m_matrixView; }
	XMMATRIX GetProjection()const { return m_matrixProj; }
	XMMATRIX GetOrtho()		const { return m_matrixOrtho; }

	void InitAndSetStandardViewport();

	/******** Structres for constant buffers ********/

	struct MatrixBuffer 
	{
		XMMATRIX World;
		XMMATRIX WorldView;
		XMMATRIX WorldViewProjection; 
	};
	struct MatrixTexBuffer
	{
		XMMATRIX WorldViewProjection;
	};

protected:

	//===============================
	//		DirectX11 Members
	//===============================
	ID3D11Device*			m_d3d11Device;
	ID3D11DeviceContext*	m_d3d11DeviceContext;
	IDXGISwapChain*			m_SwapChain;

	//back buffer render target view
	ID3D11RenderTargetView*	m_pbbRenderTargetView;

	//viewport
	D3D11_VIEWPORT m_viewport;

	/////////////////////////////////////
	//Constant Buffers
	/////////////////////////////////////
	MatrixBuffer m_matrixData;

	ID3D11Buffer*	m_pcbMatrix;	//3 matrices buffer
	ID3D11Buffer*	m_pcbMatrixTex; //one matrix buffer

	/////////////////////////////////////
	//Camera
	/////////////////////////////////////
	XMVECTOR	m_vCameraPos;
	XMVECTOR	m_vCameraTarget;
	XMVECTOR	m_vCameraUp;

	float		m_fFovY;
	float		m_fAspectRatio;
	float		m_fNearZ;
	float		m_fFarZ;

	XMMATRIX	m_matrixView;
	XMMATRIX	m_matrixProj; 
	XMMATRIX	m_matrixOrtho;

private:

	/***** Application Data *****/
	HWND		m_hwnd;
	LPCTSTR		m_windowClassName;
	LPCWSTR		m_windowName;
	HINSTANCE	m_hInstance;
	bool		m_bWindowed;

	//game timing
	Timer*	m_pTimer;

protected:

	/***** Message Handlers *****/
	virtual void VKeyPressed(const Key key) {};
};