#include "BlurTestStd.h"
#include "BlurApp.h"

#include "../Scene/Box.h"

ID3D11RenderTargetView* pNullRTV;
ID3D11ShaderResourceView* pNullSRV;

BlurApp::BlurApp()
{
	m_vCameraPos = XMVectorSet(0.0f, 0.0f, -5.0f, 1.0f);
	m_vCameraTarget = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	m_vCameraUp	= XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f); 

	m_fFovY = 0.4f * 3.14f;
	m_fAspectRatio = (float)SCREEN_WIDTH  / (float)SCREEN_HEIGHT;
	m_fNearZ = 1.0f;
	m_fFarZ = 100.0f; 
}

BlurApp::~BlurApp()
{
	while (!m_meshes.empty())
	{
		m_meshes.pop_back();
	}
}

void BlurApp::InitializeTextures()
{
	HRESULT hr;

	// ******************************************
	//
	//			Create Texture Resources
	//
	// ******************************************

	//Fill texture desc
	D3D11_TEXTURE2D_DESC tDesc;
	ZeroMemory(&tDesc, sizeof(D3D11_TEXTURE2D_DESC));
	tDesc.Width = SCREEN_WIDTH;
	tDesc.Height = SCREEN_HEIGHT;
	tDesc.ArraySize = 1;
	tDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	tDesc.CPUAccessFlags = 0;
	tDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	tDesc.SampleDesc.Count = 1;
	tDesc.Usage = D3D11_USAGE_DEFAULT;
	tDesc.MiscFlags = 0;
	tDesc.MipLevels = 1;

	///////////////////////////////////////////
	//Create textures
	hr = m_d3d11Device->CreateTexture2D(&tDesc, NULL, &m_pMaskTexture);
	if (hr != S_OK)
	{
		assert(0 && "error");
	}

	hr = m_d3d11Device->CreateTexture2D(&tDesc, NULL, &m_pSceneTexture);
	if (hr != S_OK)
	{
		assert(0 && "error");
	}

	// ******************************************
	//
	//			Create Shader Resource Views
	//
	// ******************************************

	//fill shader resource view structure
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	srvDesc.Format = tDesc.Format;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;

	/////////////////////////////////////////////
	//Initialize shader resource views
	hr = m_d3d11Device->CreateShaderResourceView(m_pMaskTexture, &srvDesc, &m_pMaskSRV);
	if (hr != S_OK)
	{
		assert(0 && "error");
	}
	hr = m_d3d11Device->CreateShaderResourceView(m_pSceneTexture, &srvDesc, &m_pSceneSRV);
	if (hr != S_OK)
	{
		assert(0 && "error");
	}

	// ******************************************
	//
	//			Create Render Target Views
	//
	// ******************************************

	//fill render target view structure
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
	ZeroMemory(&rtvDesc, sizeof(&rtvDesc));
	rtvDesc.Format = tDesc.Format;
	rtvDesc.Texture2D.MipSlice = 0;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

	//////////////////////////////////////////////
	//Initialize render target views
	hr = m_d3d11Device->CreateRenderTargetView(m_pMaskTexture, &rtvDesc, &m_pMaskRTV);
	if (hr != S_OK)
	{
		assert(0 && "error");
	}
	hr = m_d3d11Device->CreateRenderTargetView(m_pSceneTexture, &rtvDesc, &m_pSceneRTV);
	if (hr != S_OK)
	{
		assert(0 && "error");
	}
}

bool BlurApp::VInitSimulation()
{
	App::VInitSimulation();

	//Initialize textures and their views for rendering
	InitializeTextures();

	HRESULT hr;

	///////////////////////////////////
	//Create shaders
	ID3D10Blob* pCompiledLightningVertexShader;
	ID3D10Blob* pCompiledLightningPixelShader;
	ID3D10Blob* pErrors;

	hr = D3DX11CompileFromFile(L"Shaders\\Lightning.hlsl", 0, 0, "VS", "vs_5_0", 0, 0, 0, &pCompiledLightningVertexShader, &pErrors, 0);
	if (hr != S_OK)
	{
		if (pErrors != 0)
			OutputDebugStringA((char*)pErrors->GetBufferPointer());
	}

	hr = D3DX11CompileFromFile(L"Shaders\\Lightning.hlsl", 0, 0, "PS", "ps_5_0", 0, 0, 0, &pCompiledLightningPixelShader, &pErrors, 0);
	if (hr != S_OK)
	{
		if (pErrors != 0)
			OutputDebugStringA((char*)pErrors->GetBufferPointer());
	}

	hr = m_d3d11Device->CreateVertexShader(pCompiledLightningVertexShader->GetBufferPointer(), pCompiledLightningVertexShader->GetBufferSize(), nullptr, &m_pLightningVertexShader);
	hr = m_d3d11Device->CreatePixelShader(pCompiledLightningPixelShader->GetBufferPointer(), pCompiledLightningPixelShader->GetBufferSize(), nullptr, &m_pLightningPixelShader);
	
	///////////////////////////////////
	//Initialize layout
	D3D11_INPUT_ELEMENT_DESC layout[] = 
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "TEXCOORDS", 0, DXGI_FORMAT_R32G32_FLOAT,	  1, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 2, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 2, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "BINORMAL",  0, DXGI_FORMAT_R32G32B32_FLOAT, 2, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	m_d3d11Device->CreateInputLayout(layout, 5, pCompiledLightningVertexShader->GetBufferPointer(), pCompiledLightningVertexShader->GetBufferSize(), &m_pGeometryLayout);

	/////////////////////////////////////
	//Create blur shader
	ID3D10Blob*	pCompiledHorizontalGaussComputeShader;
	ID3D10Blob*	pCompiledVerticalGaussComputeShader;

	hr = D3DX11CompileFromFile(L"Shaders\\Blur.hlsl", 0, 0, "CS_Horizontal", "cs_5_0", 0, 0, 0, &pCompiledHorizontalGaussComputeShader, &pErrors, 0);
	if (hr != S_OK)
	{
		if (pErrors != 0)
			OutputDebugStringA((char*)pErrors->GetBufferPointer());
	}

	hr = D3DX11CompileFromFile(L"Shaders\\Blur.hlsl", 0, 0, "CS_Vertical", "cs_5_0", 0, 0, 0, &pCompiledVerticalGaussComputeShader, &pErrors, 0);
	if (hr != S_OK)
	{
		if (pErrors != 0)
			OutputDebugStringA((char*)pErrors->GetBufferPointer());
	}

	hr = m_d3d11Device->CreateComputeShader(pCompiledHorizontalGaussComputeShader->GetBufferPointer(), pCompiledHorizontalGaussComputeShader->GetBufferSize(), nullptr, &m_pGaussHorizontalComputeShader); 
	hr = m_d3d11Device->CreateComputeShader(pCompiledVerticalGaussComputeShader->GetBufferPointer(), pCompiledVerticalGaussComputeShader->GetBufferSize(), nullptr, &m_pGaussVerticalComputeShader); 

	/////////////////////////////////////
	//Create texture rendering  shaders
	ID3D10Blob* pCompiledTextureVertexShader;
	ID3D10Blob*	pCompiledTexturePixelShader;

	hr = D3DX11CompileFromFile(L"Shaders\\FinalPass.hlsl", 0, 0, "VS", "vs_5_0", 0, 0, 0, &pCompiledTextureVertexShader, &pErrors, 0);
	if (hr != S_OK)
	{
		if (pErrors != 0)
			OutputDebugStringA((char*)pErrors->GetBufferPointer());
	}

	hr = D3DX11CompileFromFile(L"Shaders\\FinalPass.hlsl", 0, 0, "PS", "ps_5_0", 0, 0, 0, &pCompiledTexturePixelShader, &pErrors, 0);
	if (hr != S_OK)
	{
		if (pErrors != 0)
			OutputDebugStringA((char*)pErrors->GetBufferPointer());
	}

	hr = m_d3d11Device->CreateVertexShader(pCompiledTextureVertexShader->GetBufferPointer(), pCompiledTextureVertexShader->GetBufferSize(), nullptr, &m_pFinalPassVertexShader);
	hr = m_d3d11Device->CreatePixelShader(pCompiledTexturePixelShader->GetBufferPointer(), pCompiledTexturePixelShader->GetBufferSize(), nullptr, &m_pFinalPassPixelShader);

	D3D11_INPUT_ELEMENT_DESC texlayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORDS", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	hr = m_d3d11Device->CreateInputLayout(texlayout, 2, pCompiledTextureVertexShader->GetBufferPointer(), pCompiledTextureVertexShader->GetBufferSize(), &m_pTextureLayout);

	//release blobs
	pCompiledLightningVertexShader->Release();
	pCompiledLightningPixelShader->Release();
	pCompiledTextureVertexShader->Release();
	pCompiledTexturePixelShader->Release();

	///////////////////////////////////////////
	//Set viewport
	///////////////////////////////////////////
	ZeroMemory(&m_viewport, sizeof(D3D11_VIEWPORT));

	m_viewport.TopLeftX = 0;
	m_viewport.TopLeftY = 0;
	m_viewport.Width = SCREEN_WIDTH;
	m_viewport.Height = SCREEN_HEIGHT;
	m_d3d11DeviceContext->RSSetViewports(1, &m_viewport);

	//////////////////////////////////////////
	//Initialize constant buffers
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));

	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(MatrixTexBuffer)*3;

	m_d3d11Device->CreateBuffer(&bufferDesc, 0, &m_pcbMatrixTex);

	//update it
	MatrixTexBuffer buffer;
	buffer.WorldViewProjection = XMMatrixTranspose(XMMatrixIdentity() * m_matrixView * XMMatrixOrthographicLH(SCREEN_WIDTH, SCREEN_HEIGHT, m_fNearZ, m_fFarZ));
	m_d3d11DeviceContext->UpdateSubresource(m_pcbMatrixTex, 0, nullptr, &buffer, 0, 0);


	/////////////////////////////////////////////
	//Vertex Buffer Description 
	/////////////////////////////////////////////
	D3D11_BUFFER_DESC vbDesc;
	ZeroMemory(&vbDesc, sizeof(D3D11_BUFFER_DESC));

	vbDesc.Usage = D3D11_USAGE_DEFAULT;
	vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbDesc.CPUAccessFlags = 0;
	vbDesc.MiscFlags = 0;
	vbDesc.ByteWidth = 20 * 6;

	float left = (float)(SCREEN_WIDTH / 2 * -1);
	float right = left + SCREEN_WIDTH;
	float top = (float)(SCREEN_HEIGHT / 2);
	float bottom = top - SCREEN_HEIGHT;

	RectVertex verts [] = 
	{
		RectVertex(left,	top,	0.0f,	0.0f,	0.0f ), //left top
		RectVertex(right,	top,	0.0f,	1.0f,	0.0f ), //right top
		RectVertex(left,	bottom, 0.0f,	0.0f,	1.0f ), //left bottom
		RectVertex(left,	bottom, 0.0f,	0.0f,	1.0f ), //left bottom
		RectVertex(right,	top,	0.0f,	1.0f,	0.0f ), //right top
		RectVertex(right,	bottom, 0.0f,	1.0f,	1.0f ), //right bottom
	};


	//////////////////////////////////////////////
	//Subresource data
	//////////////////////////////////////////////
	D3D11_SUBRESOURCE_DATA bufferData;
	ZeroMemory(&bufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	bufferData.pSysMem = verts;
	hr = m_d3d11Device->CreateBuffer(&vbDesc, &bufferData, &m_pRectVertexBuffer);

	//???!!!delete [] verts;

	///////////////////////////////////////////
	//Initialize meshes
	///////////////////////////////////////////
	m_currentShape = box_shape;

	//add box shape to the list
	Box* pBox = new Box();
	pBox->VInitialize(m_d3d11Device);

	m_meshes.push_back(pBox);

	return true;
}

void BlurApp::VUpdate(real elapsedTime, real totalTime)
{
	for (Meshes::iterator it = m_meshes.begin(); it != m_meshes.end(); it++)
	{
		(*it)->VUpdate(elapsedTime, totalTime);
	}
}

void BlurApp::VRender(real elapsedTime, real totalTime)
{
	//App::VRender(elapsedTime, totalTime);

	////////////////////////////////////////////
	//Render meshes

	//set necessary states first
	m_d3d11DeviceContext->IASetInputLayout(m_pGeometryLayout);
	m_d3d11DeviceContext->VSSetShader(m_pLightningVertexShader, 0, 0);
	m_d3d11DeviceContext->PSSetShader(m_pLightningPixelShader, 0, 0);
	m_d3d11DeviceContext->OMSetRenderTargets(1, &m_pSceneRTV, nullptr);

	for (Meshes::iterator it = m_meshes.begin(); it != m_meshes.end(); it++)
	{
		(*it)->VPreRender(this, elapsedTime, totalTime);
		(*it)->VRender(m_d3d11DeviceContext, elapsedTime, totalTime);
		(*it)->VPostRender(this, elapsedTime, totalTime);
	}

	//float color[4] = { 0, 0, 0, 1 };
	//m_d3d11DeviceContext->ClearRenderTargetView(m_pbbRenderTargetView, color);

	m_d3d11DeviceContext->OMSetRenderTargets(1, &pNullRTV, NULL);

	UINT stride = sizeof(RectVertex);
	UINT offset = 0;
	m_d3d11DeviceContext->IASetVertexBuffers(0, 1, &m_pRectVertexBuffer, &stride, &offset);
	m_d3d11DeviceContext->IASetInputLayout(m_pTextureLayout);
	m_d3d11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_d3d11DeviceContext->VSSetShader(m_pFinalPassVertexShader, 0, 0);
	m_d3d11DeviceContext->VSSetConstantBuffers(0, 1, &m_pcbMatrixTex);
	m_d3d11DeviceContext->PSSetShader(m_pFinalPassPixelShader, 0, 0);
	m_d3d11DeviceContext->PSSetShaderResources(0, 1, &m_pSceneSRV);
	m_d3d11DeviceContext->OMSetRenderTargets(1, &m_pbbRenderTargetView, nullptr);
	m_d3d11DeviceContext->RSSetViewports(1, &m_viewport);
	m_d3d11DeviceContext->Draw(6, 0);

	//unbind resources
	m_d3d11DeviceContext->PSSetShaderResources(0, 1, &pNullSRV);
	m_d3d11DeviceContext->OMSetRenderTargets(1, &pNullRTV, NULL);

	//flip back buffer
	m_SwapChain->Present(0, 0);
}