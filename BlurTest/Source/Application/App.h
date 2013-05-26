#pragma once 

#include "../Time/Timer.h"

class App
{
public:
	App();
	~App();

	//void Update(rea
	bool InitWindow(HINSTANCE hInstance, int showWnd, int iWidth, int iHeight, bool bWindowed);
	bool InitDirectX11(int iWidth, int iHeight);
	virtual bool VInitSimulation();
	static LRESULT CALLBACK WndProc(HWND hWnd, int msg, WPARAM wParam, LPARAM lParam);
	virtual void VUpdate(real elapsedTime, real totalTime);
	virtual void VRender(real elapsedTime, real totalTime);
	int Run();


	/////////////////////////////////////
	//Constant Buffers
	/////////////////////////////////////

	/******** Structres for constant buffers ********/
	struct MatrixBuffer
	{
		XMMATRIX World;
		XMMATRIX WorldView;
		XMMATRIX WorldViewProjection; 
	};
	MatrixBuffer m_matrixData;

	ID3D11Buffer*	m_pcbMatrix;

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

	//////////////////////////////
	//Accessors
	//////////////////////////////
	ID3D11Device*			GetDevice() const					{ return m_d3d11Device; }
	ID3D11DeviceContext*	GetImmediateContext() const			{ return m_d3d11DeviceContext; }
	ID3D11RenderTargetView* GetBackBufferRenderTarget() const	{ return m_pbbRenderTargetView; }

private:
	HWND		m_hwnd;
	LPCTSTR		m_windowClassName;
	HINSTANCE	m_hInstance;
	bool		m_bWindowed;

	Timer*	m_pTimer;

protected:

	virtual void VKeyPressed(const Key key) {};
	//===============================
	//		DirectX11 Members
	//===============================
	ID3D11Device*			m_d3d11Device;
	ID3D11DeviceContext*	m_d3d11DeviceContext;
	IDXGISwapChain*			m_SwapChain;

	ID3D11RenderTargetView*	m_pbbRenderTargetView;
};