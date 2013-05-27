//========================================================================
// BlurApp.cpp
//
// This code is part of Ubisoft Programmer Test 
//
// Coded by Muralev Evgeny
//========================================================================

#include "BlurTestStd.h"
#include "BlurApp.h"

#include "../DirectX11/dx.h"

#include "../Scene/Box.h"

ID3D11RenderTargetView* pNullRTV;
ID3D11ShaderResourceView* pNullSRV;
ID3D11UnorderedAccessView*	pNullUAV;

BlurApp::BlurApp()
{
	m_lightingMode = LM_Blinn;
	m_maskMode = MM_Everything;

	//default values
	m_vCameraPos	= XMVectorSet(0.0f, 0.0f, -5.0f, 1.0f);
	m_vCameraTarget = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	m_vCameraUp		= XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f); 

	m_fFovY = 0.4f * 3.14f;
	m_fAspectRatio = (float)SCREEN_WIDTH  / (float)SCREEN_HEIGHT;
	m_fNearZ = 1.0f;
	m_fFarZ = 100.0f; 

	m_pFont = new Font();

	m_pLightingClassLinkage = NULL;
}

BlurApp::~BlurApp()
{
	//clean vertex buffers
	SAFE_RELEASE(m_pRectVertexBuffer);
	SAFE_RELEASE(m_pScreenRectVertexBuffer);

	//clean constant buffers
	SAFE_RELEASE(m_pcbLight);
	SAFE_RELEASE(m_pcbLighting);
	SAFE_RELEASE(m_pcbScreen);

	//clean depth buffer stuff
	SAFE_RELEASE(m_pDepthStencilSRV);
	SAFE_RELEASE(m_pDepthStencilView);
	SAFE_RELEASE(m_pDepthDisable);
	SAFE_RELEASE(m_pDepthEnable);
	SAFE_RELEASE(m_pDepthStencilTexture);
	
	//clean sampler states
	SAFE_RELEASE(m_pTiledSampler);
	SAFE_RELEASE(m_pLinearSampler);
	SAFE_RELEASE(m_pAnisotropicSampler);

	//clean mask resource and views
	SAFE_RELEASE(m_pMaskRTV);
	SAFE_RELEASE(m_pMaskUAV);
	SAFE_RELEASE(m_pMaskSRV);
	SAFE_RELEASE(m_pMaskTexture);

	//clean scene resource and views
	SAFE_RELEASE(m_pSceneRTV);
	SAFE_RELEASE(m_pSceneSRV);
	SAFE_RELEASE(m_pSceneUAV);
	SAFE_RELEASE(m_pSceneTexture);

	SAFE_RELEASE(m_pBackgroundSRV);

	//clean blurred resource and views
	SAFE_RELEASE(m_pBlurredSRV);
	SAFE_RELEASE(m_pBlurredUAV);
	SAFE_RELEASE(m_pBlurredTexture);

	//clean shader linkage
	for (auto i = 0; i < LM_NumLightingModes; i++)
	{
		ID3D11ClassInstance* pInstance = m_lightingClassInstances[static_cast<LightingMode>(i)];
		m_lightingClassInstances.erase(static_cast<LightingMode>(i));

		SAFE_RELEASE(pInstance);
	}
	SAFE_RELEASE(m_pLightingClassLinkage);

	//clean sgaders
	SAFE_RELEASE(m_pMaskPixelShader);
	SAFE_RELEASE(m_pMaskModifiedPixelShader);

	SAFE_RELEASE(m_pLightingVertexShader);
	SAFE_RELEASE(m_pLightingPixelShader);

	SAFE_RELEASE(m_pFinalPassVertexShader);
	SAFE_RELEASE(m_pFinalPassPixelShader);

	SAFE_RELEASE(m_pBackgroundPixelShader);

	SAFE_RELEASE(m_pGaussHorizontalComputeShader);
	SAFE_RELEASE(m_pGaussVerticalComputeShader);
	SAFE_RELEASE(m_pGaussComputeShader);

	SAFE_RELEASE(m_pFontVertexShader);
	SAFE_RELEASE(m_pFontPixelShader);

	//clean input layouts
	SAFE_RELEASE(m_pGeometryLayout);
	SAFE_RELEASE(m_pTextureLayout);

	//clean blend states
	SAFE_RELEASE(m_pBlendOff);
	SAFE_RELEASE(m_pAdditiveBlendOn);
	
	//clean meshes
	while (!m_meshes.empty())
	{
		Mesh* pMesh = m_meshes.back();
		SAFE_DELETE(pMesh);

		m_meshes.pop_back();
	}

	//clean texts
	while(!m_texts.empty())
	{
		Text* pText = m_texts.back();
		SAFE_DELETE(pText);

		m_texts.pop_back();
	}

	SAFE_DELETE(m_pFont);

}

bool BlurApp::InitializeShadersAndInputLayouts()
{
	///////////////////////////////////
	//Lighting shaders
	ID3D10Blob* pCompiledLightingVertexShader;
	ID3D10Blob* pCompiledLightingPixelShader;

	if (!CompileShader(L"Shaders\\Lighting.hlsl", "VS", vertex_shader, &pCompiledLightingVertexShader))
		return false;
	HRESULT hr = m_d3d11Device->CreateVertexShader(pCompiledLightingVertexShader->GetBufferPointer(), pCompiledLightingVertexShader->GetBufferSize(), nullptr, &m_pLightingVertexShader);
	VALID(hr);

	if (!CompileShader(L"Shaders\\Lighting.hlsl", "PS", pixel_shader, &pCompiledLightingPixelShader))
		return false;

	// ======================================================== //
	// ====	Create class linkage for lighting pixel shader ==== //
	// ======================================================== //
	hr = m_d3d11Device->CreateClassLinkage(&m_pLightingClassLinkage);
	VALID(hr);

	hr = m_d3d11Device->CreatePixelShader(pCompiledLightingPixelShader->GetBufferPointer(), pCompiledLightingPixelShader->GetBufferSize(), m_pLightingClassLinkage, &m_pLightingPixelShader);
	VALID(hr);

	//get lambert class
	ID3D11ClassInstance* pLambert = NULL;
	hr = m_pLightingClassLinkage->GetClassInstance("g_lambert", 0, &pLambert);
	VALID(hr);
	m_lightingClassInstances.insert(std::make_pair<LightingMode, ID3D11ClassInstance*>(LM_Lambert, (ID3D11ClassInstance*&&)pLambert));

	//get lambert wrap around class
	ID3D11ClassInstance* pLambertWrapAround = NULL;
	hr = m_pLightingClassLinkage->GetClassInstance("g_lambertWrapAround", 0, &pLambertWrapAround);
	VALID(hr);
	m_lightingClassInstances.insert(std::make_pair<LightingMode, ID3D11ClassInstance*>(LM_LambertWrapAround, (ID3D11ClassInstance*&&)pLambertWrapAround));

	//get phong class
	ID3D11ClassInstance* pPhong = NULL;
	hr = m_pLightingClassLinkage->GetClassInstance("g_phong", 0, &pPhong);
	VALID(hr);
	m_lightingClassInstances.insert(std::make_pair<LightingMode, ID3D11ClassInstance*>(LM_Phong, (ID3D11ClassInstance*&&)pPhong));

	//get blinn class
	ID3D11ClassInstance* pBlinn = NULL;
	hr = m_pLightingClassLinkage->GetClassInstance("g_blinn", 0, &pBlinn);
	VALID(hr);
	m_lightingClassInstances.insert(std::make_pair<LightingMode, ID3D11ClassInstance*>(LM_Blinn, (ID3D11ClassInstance*&&)pBlinn));

	//get isotropic ward class
	ID3D11ClassInstance* pIsotropicWard = NULL;
	hr = m_pLightingClassLinkage->GetClassInstance("g_isotropicWard", 0, &pIsotropicWard);
	VALID(hr);
	m_lightingClassInstances.insert(std::make_pair<LightingMode, ID3D11ClassInstance*>(LM_IsotropicWard, (ID3D11ClassInstance*&&)pIsotropicWard));
	
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

	hr = m_d3d11Device->CreateInputLayout(layout, 5, pCompiledLightingVertexShader->GetBufferPointer(), pCompiledLightingVertexShader->GetBufferSize(), &m_pGeometryLayout);
	VALID(hr);

	/////////////////////////////////////
	//Create blur shader
	ID3D10Blob*	pCompiledHorizontalGaussComputeShader;
	ID3D10Blob*	pCompiledVerticalGaussComputeShader;
	ID3D10Blob* pCompiledGaussComputeShader;

	if (!CompileShader(L"Shaders\\Gauss_Separable.hlsl", "CS_Horizontal", compute_shader, &pCompiledHorizontalGaussComputeShader))
		return false;
	hr = m_d3d11Device->CreateComputeShader(pCompiledHorizontalGaussComputeShader->GetBufferPointer(), pCompiledHorizontalGaussComputeShader->GetBufferSize(), nullptr, &m_pGaussHorizontalComputeShader);
	VALID(hr);

	if (!CompileShader(L"Shaders\\Gauss_Separable.hlsl", "CS_Vertical", compute_shader, &pCompiledVerticalGaussComputeShader))
		return false;
	hr = m_d3d11Device->CreateComputeShader(pCompiledVerticalGaussComputeShader->GetBufferPointer(), pCompiledVerticalGaussComputeShader->GetBufferSize(), nullptr, &m_pGaussVerticalComputeShader); 
	VALID(hr);

	if (!CompileShader(L"Shaders\\Gauss.hlsl", "CS", compute_shader, &pCompiledGaussComputeShader))
		return false;
	hr = m_d3d11Device->CreateComputeShader(pCompiledGaussComputeShader->GetBufferPointer(), pCompiledGaussComputeShader->GetBufferSize(), nullptr, &m_pGaussComputeShader);
	VALID(hr);

	/////////////////////////////////////
	//Create texture rendering shaders
	ID3D10Blob* pCompiledTextureVertexShader;
	ID3D10Blob*	pCompiledTexturePixelShader;
	ID3D10Blob*	pCompiledBackgroundPixelShader;

	if (!CompileShader(L"Shaders\\FinalPass.hlsl", "VS", vertex_shader, &pCompiledTextureVertexShader))
		return false;
	hr = m_d3d11Device->CreateVertexShader(pCompiledTextureVertexShader->GetBufferPointer(), pCompiledTextureVertexShader->GetBufferSize(), nullptr, &m_pFinalPassVertexShader);
	VALID(hr);

	if (!CompileShader(L"Shaders\\FinalPass.hlsl", "PS", pixel_shader, &pCompiledTexturePixelShader))
		return false;
	hr = m_d3d11Device->CreatePixelShader(pCompiledTexturePixelShader->GetBufferPointer(), pCompiledTexturePixelShader->GetBufferSize(), nullptr, &m_pFinalPassPixelShader);
	VALID(hr);

	if (!CompileShader(L"Shaders\\FinalPass.hlsl", "PSBackground", pixel_shader, &pCompiledBackgroundPixelShader))
		return false;
	VALID(hr);

	hr = m_d3d11Device->CreatePixelShader(pCompiledBackgroundPixelShader->GetBufferPointer(), pCompiledBackgroundPixelShader->GetBufferSize(), nullptr, &m_pBackgroundPixelShader);

	//create input layouts fot texture vertex shaders
	D3D11_INPUT_ELEMENT_DESC texlayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORDS", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	hr = m_d3d11Device->CreateInputLayout(texlayout, 2, pCompiledTextureVertexShader->GetBufferPointer(), pCompiledTextureVertexShader->GetBufferSize(), &m_pTextureLayout);
	VALID(hr);

	/////////////////////////////////////
	//Create mask shader
	ID3D10Blob*	pCompiledMaskPixelShader;
	ID3D10Blob* pCompiledMaskModifiedPixelShader;

	if (!CompileShader(L"Shaders\\MaskGeneration.hlsl", "MaskSimplePS", pixel_shader, &pCompiledMaskPixelShader))
		return false;
	hr = m_d3d11Device->CreatePixelShader(pCompiledMaskPixelShader->GetBufferPointer(), pCompiledMaskPixelShader->GetBufferSize(), nullptr, &m_pMaskPixelShader); 
	VALID(hr);

	if (!CompileShader(L"Shaders\\MaskGeneration.hlsl", "MaskDepthPS", pixel_shader, &pCompiledMaskModifiedPixelShader))
		return false;
	hr = m_d3d11Device->CreatePixelShader(pCompiledMaskModifiedPixelShader->GetBufferPointer(), pCompiledMaskModifiedPixelShader->GetBufferSize(), nullptr, &m_pMaskModifiedPixelShader); 
	VALID(hr);

	/////////////////////////////////////
	//Create font shader
	ID3D10Blob*	pCompiledFontVertexShader;
	ID3D10Blob* pCompiledFontPixelShader;

	if (!CompileShader(L"Shaders\\FontVS.hlsl", "FontVS", vertex_shader, &pCompiledFontVertexShader))
		return false;
	hr = m_d3d11Device->CreateVertexShader(pCompiledFontVertexShader->GetBufferPointer(), pCompiledFontVertexShader->GetBufferSize(), nullptr, &m_pFontVertexShader); 
	VALID(hr);

	if (!CompileShader(L"Shaders\\FontPS.hlsl", "FontPS", pixel_shader, &pCompiledFontPixelShader))
		return false;
	hr = m_d3d11Device->CreatePixelShader(pCompiledFontPixelShader->GetBufferPointer(), pCompiledFontPixelShader->GetBufferSize(), nullptr, &m_pFontPixelShader); 
	VALID(hr);

	//release blobs
	SAFE_RELEASE(pCompiledLightingVertexShader);
	SAFE_RELEASE(pCompiledLightingPixelShader);
	SAFE_RELEASE(pCompiledHorizontalGaussComputeShader);
	SAFE_RELEASE(pCompiledVerticalGaussComputeShader);
	SAFE_RELEASE(pCompiledGaussComputeShader);
	SAFE_RELEASE(pCompiledBackgroundPixelShader);
	SAFE_RELEASE(pCompiledTextureVertexShader);
	SAFE_RELEASE(pCompiledTexturePixelShader);
	SAFE_RELEASE(pCompiledMaskPixelShader);
	SAFE_RELEASE(pCompiledMaskModifiedPixelShader);
	SAFE_RELEASE(pCompiledFontVertexShader);
	SAFE_RELEASE(pCompiledFontPixelShader);

	return true;
}

bool BlurApp::InitializeTextures()
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
	tDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_RENDER_TARGET;
	tDesc.CPUAccessFlags = 0;
	tDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	tDesc.SampleDesc.Count = 1;
	tDesc.Usage = D3D11_USAGE_DEFAULT;
	tDesc.MiscFlags = 0;
	tDesc.MipLevels = 1;

	///////////////////////////////////////////
	//Create textures
	hr = m_d3d11Device->CreateTexture2D(&tDesc, NULL, &m_pMaskTexture);
	VALID(hr);

	//scene resource
	tDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET | D3D11_BIND_UNORDERED_ACCESS;
	hr = m_d3d11Device->CreateTexture2D(&tDesc, NULL, &m_pSceneTexture);
	VALID(hr);

	//intermidiate blur resource
	tDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	hr = m_d3d11Device->CreateTexture2D(&tDesc, NULL, &m_pBlurredTexture);
	VALID(hr);

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
	VALID(hr);

	//scene srv
	hr = m_d3d11Device->CreateShaderResourceView(m_pSceneTexture, &srvDesc, &m_pSceneSRV);
	VALID(hr);

	//intermidiate blur srv
	hr = m_d3d11Device->CreateShaderResourceView(m_pBlurredTexture, &srvDesc, &m_pBlurredSRV);
	VALID(hr);

	//create background shader resource view
	hr = D3DX11CreateShaderResourceViewFromFile(m_d3d11Device, L"Resources\\bg.png", NULL, NULL, &m_pBackgroundSRV, NULL);
	VALID(hr);

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
	VALID(hr);

	hr = m_d3d11Device->CreateRenderTargetView(m_pSceneTexture, &rtvDesc, &m_pSceneRTV);
	VALID(hr);

	// ******************************************
	//
	//		Create Unordered Access Views
	//
	// ******************************************
	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	ZeroMemory(&uavDesc, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC));
	uavDesc.Format = tDesc.Format;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	uavDesc.Texture2D.MipSlice = 0;

	//////////////////////////////////////////////
	//Initialize unordered access views
	hr = m_d3d11Device->CreateUnorderedAccessView(m_pSceneTexture, &uavDesc, &m_pSceneUAV);
	VALID(hr);

	hr = m_d3d11Device->CreateUnorderedAccessView(m_pBlurredTexture, &uavDesc, &m_pBlurredUAV);
	VALID(hr);

	hr = m_d3d11Device->CreateUnorderedAccessView(m_pMaskTexture, &uavDesc, &m_pMaskUAV);
	VALID(hr);

	return true;
}

bool BlurApp::InitializeDepthBuffer()
{
	// ******************************************
	//	Initialize depth buffer texture and views
	// ******************************************
	D3D11_TEXTURE2D_DESC	depthDesc;
	ZeroMemory(&depthDesc, sizeof(D3D11_TEXTURE2D_DESC));
	depthDesc.Width = SCREEN_WIDTH;
	depthDesc.Height = SCREEN_HEIGHT;
	depthDesc.MipLevels = 1;
	depthDesc.ArraySize = 1;
	depthDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	depthDesc.SampleDesc.Count = 1;
	depthDesc.SampleDesc.Quality = 0;
	depthDesc.Usage = D3D11_USAGE_DEFAULT;
	depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	depthDesc.CPUAccessFlags = 0;
	depthDesc.MiscFlags = 0;

	//texture
	HRESULT hr = m_d3d11Device->CreateTexture2D(&depthDesc, nullptr, &m_pDepthStencilTexture);
	VALID(hr);

	//Initialize depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	ZeroMemory(&dsvDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;

	hr = m_d3d11Device->CreateDepthStencilView(m_pDepthStencilTexture, &dsvDesc, &m_pDepthStencilView);
	VALID(hr);

	//Initialize shader resource view for depth texture
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	hr = m_d3d11Device->CreateShaderResourceView(m_pDepthStencilTexture, &srvDesc, &m_pDepthStencilSRV);
	VALID(hr);

	// ******************************************
	//	Initialize depth buffer states
	// ******************************************
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	ZeroMemory(&depthStencilDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthStencilDesc.StencilEnable = false;
	
	//create depth stencil state
	hr = m_d3d11Device->CreateDepthStencilState(&depthStencilDesc, &m_pDepthEnable);
	VALID(hr);

	depthStencilDesc.DepthEnable = false;
	hr = m_d3d11Device->CreateDepthStencilState(&depthStencilDesc, &m_pDepthDisable);
	VALID(hr);

	return true;
}

bool BlurApp::InitializeBlendStates()
{
	//blend description
	D3D11_BLEND_DESC blendDesc;
	ZeroMemory(&blendDesc, sizeof(D3D11_BLEND_DESC));
	blendDesc.AlphaToCoverageEnable = false;
	blendDesc.IndependentBlendEnable = false;

	//render target blend description
	D3D11_RENDER_TARGET_BLEND_DESC rtBlendDesc;
	ZeroMemory(&rtBlendDesc, sizeof(D3D11_RENDER_TARGET_BLEND_DESC));
	rtBlendDesc.BlendEnable = true;
	rtBlendDesc.SrcBlend  = D3D11_BLEND_SRC_ALPHA;
	rtBlendDesc.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	rtBlendDesc.BlendOp = D3D11_BLEND_OP_ADD;
	rtBlendDesc.SrcBlendAlpha = D3D11_BLEND_ONE;
	rtBlendDesc.DestBlendAlpha = D3D11_BLEND_ZERO;
	rtBlendDesc.BlendOpAlpha = D3D11_BLEND_OP_ADD;
	rtBlendDesc.RenderTargetWriteMask = 0x0f;

	blendDesc.RenderTarget[0] = rtBlendDesc;

	//additive blend state
	HRESULT hr = m_d3d11Device->CreateBlendState(&blendDesc, &m_pAdditiveBlendOn);
	VALID(hr);

	//switch blending off
	rtBlendDesc.BlendEnable = false;
	hr = m_d3d11Device->CreateBlendState(&blendDesc, &m_pBlendOff);
	VALID(hr);

	return true;
}

bool BlurApp::InitializeSamplerStates()
{
	/****** Sampler state description ******/
	D3D11_SAMPLER_DESC sDesc;
	ZeroMemory(&sDesc, sizeof(D3D11_SAMPLER_DESC));
	sDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	sDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sDesc.MinLOD = 0;
	sDesc.MaxLOD = D3D11_FLOAT32_MAX;
	sDesc.MaxAnisotropy = 16;

	//Anisotropic sampler state
	HRESULT hr = m_d3d11Device->CreateSamplerState(&sDesc, &m_pAnisotropicSampler);
	VALID(hr);

	//Linear sampler state
	sDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	hr = m_d3d11Device->CreateSamplerState(&sDesc, &m_pLinearSampler);
	VALID(hr);

	//Background tiled point sampler
	sDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	sDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	hr = m_d3d11Device->CreateSamplerState(&sDesc, &m_pTiledSampler);
	VALID(hr);

	return true;
}

bool BlurApp::InitializeText()
{
	//
	//Initialize fonts
	//
	if (!m_pFont->LoadAsciiData("Resources\\fonts\\simplefont.txt"))
	{
		assert(0 && "Error loading fotn ascii data!");
		return false;
	}

	if (!m_pFont->LoadFontTexture(m_d3d11Device, L"Resources\\fonts\\simplefont.dds"))
	{
		assert(0 && "Error loading font texture!");
		return false;
	}

	//
	//Initialize texts
	//

	//lighting model text
	Text* m_pLightingModeText = new Text();
	m_pLightingModeText->Initialize(m_d3d11Device, "Lighting Model (press A and D to change): ", -SCREEN_WIDTH / 2.0f + 10.0f, SCREEN_HEIGHT / 2.0f - 10.0f, m_pFont);
	m_texts.push_back(m_pLightingModeText);

	//lambert
	m_pLambertText = new Text();
	m_pLambertText->Initialize(m_d3d11Device, "Lambert", -SCREEN_WIDTH / 2.0f + 210.0f, SCREEN_HEIGHT / 2.0f - 10.0f, m_pFont);
	m_texts.push_back(m_pLambertText);

	//lambert wrap around
	m_pLambertWrapAroundText = new Text();
	m_pLambertWrapAroundText->Initialize(m_d3d11Device, "Lambert Wrap Around", -SCREEN_WIDTH / 2.0f + 210.0f, SCREEN_HEIGHT / 2.0f - 10.0f, m_pFont);
	m_texts.push_back(m_pLambertWrapAroundText);

	//phong
	m_pPhongText = new Text();
	m_pPhongText->Initialize(m_d3d11Device, "Phong", -SCREEN_WIDTH / 2.0f + 210.0f, SCREEN_HEIGHT / 2.0f - 10.0f, m_pFont);
	m_texts.push_back(m_pPhongText);

	//blinn
	m_pBlinnText = new Text();
	m_pBlinnText->Initialize(m_d3d11Device, "Blinn", -SCREEN_WIDTH / 2.0f + 210.0f, SCREEN_HEIGHT / 2.0f - 10.0f, m_pFont);
	m_texts.push_back(m_pBlinnText);

	//isotropic ward
	m_pIsotropicWardText = new Text();
	m_pIsotropicWardText->Initialize(m_d3d11Device, "IsotropicWard", -SCREEN_WIDTH / 2.0f + 210.0f, SCREEN_HEIGHT / 2.0f - 10.0f, m_pFont);
	m_texts.push_back(m_pIsotropicWardText);

	//mask type text
	Text* m_pMaskModeText = new Text();
	m_pMaskModeText->Initialize(m_d3d11Device, "Press Q to  change  mask  mode: ", -SCREEN_WIDTH / 2.0f + 10.0f, SCREEN_HEIGHT / 2.0f - 26.0f, m_pFont);
	m_texts.push_back(m_pMaskModeText);
}

bool BlurApp::VInitSimulation()
{
	App::VInitSimulation();

	//Initialize textures and their views for rendering
	if (!InitializeTextures())
	{
		assert(0 && "Error initializing textures!");
		return false;
	}

	//Initialize depth stencil buffer
	if (!InitializeDepthBuffer())
	{
		assert(0 && "Error initializing depth buffer!");
		return false;
	}

	//Initialize sampler states
	if (!InitializeSamplerStates())
	{
		assert(0 && "Error initializing sampler states!");
		return false;
	}

	//Initialize blend states
	if (!InitializeBlendStates())
	{
		assert(0 && "Error initializing blend states!");
		return false;
	}

	//Initialize font and application text
	if (!InitializeText())
	{
		assert(0 && "Error initializing game texts!");
		return false;
	}

	if (!InitializeShadersAndInputLayouts())
	{
		assert(0 && "Error initializing shaders or input layout!");
		return false;
	}

	if (!InitializeConstantBuffers())
	{
		assert(0 && "Error initializing constant buffers!");
		return false;
	}

	if (!InitializeVertexBuffers())
	{
		assert(0 && "Error initializing vertex buffers!");
		return false;
	}

	if (!InitializeGameObjects())
	{
		assert(0 && "Error initializing game objects!");
		return false;
	}

	//Set viewport
	InitAndSetStandardViewport();

	return true;
}

bool BlurApp::InitializeConstantBuffers()
{
	//////////////////////////////////////////////////
	//Matrix Texture Constant Buffer
	//////////////////////////////////////////////////
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));

	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(MatrixTexBuffer); 

	HRESULT hr = m_d3d11Device->CreateBuffer(&bufferDesc, 0, &m_pcbMatrixTex);
	VALID(hr);

	//update it
	MatrixTexBuffer buffer;
	buffer.WorldViewProjection = XMMatrixTranspose(XMMatrixIdentity() * m_matrixView * XMMatrixOrthographicLH(SCREEN_WIDTH, SCREEN_HEIGHT, m_fNearZ, m_fFarZ));
	m_d3d11DeviceContext->UpdateSubresource(m_pcbMatrixTex, 0, nullptr, &buffer, 0, 0);

	//////////////////////////////////////////////////
	//Screen Data Constant Buffer
	//////////////////////////////////////////////////
	bufferDesc.ByteWidth = sizeof(ScreenData);
	hr = m_d3d11Device->CreateBuffer(&bufferDesc, 0, &m_pcbScreen);
	VALID(hr);

	//update it
	ScreenData screenBuffer;
	screenBuffer.screen_width = SCREEN_WIDTH;
	screenBuffer.screen_height = SCREEN_HEIGHT;
	m_d3d11DeviceContext->UpdateSubresource(m_pcbScreen, 0, nullptr, &screenBuffer, 0, 0);

	//////////////////////////////////////////////////
	//Lighting Data Constant Buffer
	//////////////////////////////////////////////////
	bufferDesc.ByteWidth = sizeof(LightingData);
	hr = m_d3d11Device->CreateBuffer(&bufferDesc, 0, &m_pcbLighting);
	VALID(hr);

	LightingData lightingData;
	lightingData.lambertLightColor = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
	lightingData.lambertWALightColorAndFactor = XMVectorSet(1.0f, 1.0f, 1.0f, 0.3f);
	lightingData.phongLightColorAndSpecPower = XMVectorSet(1.0f, 1.0f, 1.0f, 120.0f);
	lightingData.blinnLightColorAndSpecPower = XMVectorSet(1.0f, 1.0f, 1.0f, 22.0f);
	lightingData.isotropicWardLightColorAndRoughness = XMVectorSet(1.0f, 1.0f, 1.0f, 30.0f);
	m_d3d11DeviceContext->UpdateSubresource(m_pcbLighting, 0, nullptr, &lightingData, 0, 0);

	//////////////////////////////////////////////////
	//Light Data Constant Buffer
	//////////////////////////////////////////////////
	bufferDesc.ByteWidth = sizeof(LightBuffer);
	hr = m_d3d11Device->CreateBuffer(&bufferDesc, 0, &m_pcbLight);
	VALID(hr);

	LightBuffer lightData;
	lightData.lightPos = XMVectorSet(0.0f, 0.0f, -3.0f, 1.0f);
	lightData.cameraPos = m_vCameraPos;
	m_d3d11DeviceContext->UpdateSubresource(m_pcbLight, 0, nullptr, &lightData, 0, 0);

	return true;
}

bool BlurApp::InitializeVertexBuffers()
{
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

	//create buffer
	HRESULT hr = m_d3d11Device->CreateBuffer(&vbDesc, &bufferData, &m_pRectVertexBuffer);
	VALID(hr);

	RectVertex sverts [] = 
	{
		RectVertex(left,	top,	0.0f,	0.0f,							0.0f ), //left top
		RectVertex(right,	top,	0.0f,	(float)SCREEN_WIDTH / 128.0f,	0.0f ), //right top
		RectVertex(left,	bottom, 0.0f,	0.0f,							(float)SCREEN_HEIGHT / 128.0f ), //left bottom
		RectVertex(left,	bottom, 0.0f,	0.0f,							(float)SCREEN_HEIGHT / 128.0f ), //left bottom
		RectVertex(right,	top,	0.0f,	(float)SCREEN_WIDTH / 128.0f,	0.0f ), //right top
		RectVertex(right,	bottom, 0.0f,	(float)SCREEN_WIDTH / 128.0f,	(float)SCREEN_HEIGHT / 128.0f ), //right bottom
	};

	bufferData.pSysMem = sverts;

	//create buffer
	hr = m_d3d11Device->CreateBuffer(&vbDesc, &bufferData, &m_pScreenRectVertexBuffer);
	VALID(hr);

	return true;
}

bool BlurApp::InitializeGameObjects()
{
	//add box shape to the list
	Box* pBox = new Box();
	if (!pBox->VInitialize(m_d3d11Device))
	{
		return false;
	}

	m_meshes.push_back(pBox);

	return true;
}

void BlurApp::VUpdate(float elapsedTime, float totalTime)
{
	//update meshes
	for (auto it = m_meshes.begin(); it != m_meshes.end(); it++)
	{
		(*it)->VUpdate(this, elapsedTime, totalTime);
	}

#pragma region
	//manage texts
	if (m_lightingMode ==LM_Lambert)
		m_pLambertText->SetEnable(true);
	else
		m_pLambertText->SetEnable(false);

	if (m_lightingMode ==LM_LambertWrapAround)
		m_pLambertWrapAroundText->SetEnable(true);
	else
		m_pLambertWrapAroundText->SetEnable(false);

	if (m_lightingMode ==LM_Phong)
		m_pPhongText->SetEnable(true);
	else
		m_pPhongText->SetEnable(false);

	if (m_lightingMode ==LM_Blinn)
		m_pBlinnText->SetEnable(true);
	else
		m_pBlinnText->SetEnable(false);

	if (m_lightingMode ==LM_IsotropicWard)
		m_pIsotropicWardText->SetEnable(true);
	else
		m_pIsotropicWardText->SetEnable(false);
#pragma endregion
}

void BlurApp::VRender(float elapsedTime, float totalTime)
{
	//App::VRender(elapsedTime, totalTime);

	UINT stride = sizeof(RectVertex);
	UINT offset = 0;

	////////////////////////////////////////////
	//Render background
	m_d3d11DeviceContext->IASetVertexBuffers(0, 1, &m_pScreenRectVertexBuffer, &stride, &offset);
	m_d3d11DeviceContext->IASetInputLayout(m_pTextureLayout);
	m_d3d11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_d3d11DeviceContext->VSSetShader(m_pFinalPassVertexShader, 0, 0);
	m_d3d11DeviceContext->VSSetConstantBuffers(0, 1, &m_pcbMatrixTex);
	m_d3d11DeviceContext->PSSetShader(m_pBackgroundPixelShader, 0, 0);
	m_d3d11DeviceContext->PSSetShaderResources(0, 1, &m_pBackgroundSRV);
	m_d3d11DeviceContext->PSSetSamplers(0, 1, &m_pTiledSampler);
	m_d3d11DeviceContext->RSSetViewports(1, &m_viewport);
	m_d3d11DeviceContext->OMSetDepthStencilState(m_pDepthDisable, 1);
	m_d3d11DeviceContext->OMSetBlendState(m_pBlendOff, NULL, 0xffffffff);
	m_d3d11DeviceContext->OMSetRenderTargets(1, &m_pSceneRTV, nullptr);

	m_d3d11DeviceContext->Draw(6, 0);

	//unbind resources
	m_d3d11DeviceContext->PSSetShaderResources(0, 1, &pNullSRV);
	m_d3d11DeviceContext->OMSetRenderTargets(1, &pNullRTV, NULL);

	////////////////////////////////////////////
	//Render meshes
	m_d3d11DeviceContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 1);

	//set necessary states first
	m_d3d11DeviceContext->IASetInputLayout(m_pGeometryLayout);
	m_d3d11DeviceContext->VSSetShader(m_pLightingVertexShader, 0, 0);
	m_d3d11DeviceContext->VSSetConstantBuffers(1, 1, &m_pcbLight);
	m_d3d11DeviceContext->PSSetShader(m_pLightingPixelShader, &m_lightingClassInstances[m_lightingMode], 1);
	m_d3d11DeviceContext->PSSetConstantBuffers(0, 1, &m_pcbLighting);
	m_d3d11DeviceContext->PSSetSamplers(0, 1, &m_pAnisotropicSampler);
	m_d3d11DeviceContext->OMSetDepthStencilState(m_pDepthEnable, 1);
	m_d3d11DeviceContext->OMSetBlendState(m_pBlendOff, NULL, 0xffffffff);
	m_d3d11DeviceContext->OMSetRenderTargets(1, &m_pSceneRTV, m_pDepthStencilView);

	for (Meshes::iterator it = m_meshes.begin(); it != m_meshes.end(); it++)
	{
		(*it)->VPreRender(this, elapsedTime, totalTime);
		(*it)->VRender(this, elapsedTime, totalTime);
		(*it)->VPostRender(this, elapsedTime, totalTime);
	}

	m_d3d11DeviceContext->OMSetRenderTargets(1, &pNullRTV, nullptr);

	//*******************************************************************//

	////////////////////////////////////////////
	//Generate mask (for blurring)
	m_d3d11DeviceContext->IASetVertexBuffers(0, 1, &m_pRectVertexBuffer, &stride, &offset);
	m_d3d11DeviceContext->IASetInputLayout(m_pTextureLayout);
	m_d3d11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_d3d11DeviceContext->VSSetShader(m_pFinalPassVertexShader, 0, 0);
	m_d3d11DeviceContext->VSSetConstantBuffers(0, 1, &m_pcbMatrixTex);
	if (m_maskMode == MM_Everything)	m_d3d11DeviceContext->PSSetShader(m_pMaskPixelShader, 0, 0);
	if (m_maskMode == MM_OnlyObject)	m_d3d11DeviceContext->PSSetShader(m_pMaskModifiedPixelShader, 0, 0);
	m_d3d11DeviceContext->PSSetShaderResources(0, 1, &m_pDepthStencilSRV);
	m_d3d11DeviceContext->PSSetConstantBuffers(0, 1, &m_pcbScreen);
	m_d3d11DeviceContext->OMSetDepthStencilState(m_pDepthDisable, 1);
	m_d3d11DeviceContext->OMSetBlendState(m_pBlendOff, NULL, 0xffffffff);
	m_d3d11DeviceContext->OMSetRenderTargets(1, &m_pMaskRTV, nullptr);
	
	m_d3d11DeviceContext->Draw(6, 0);

	m_d3d11DeviceContext->PSSetShaderResources(0, 1, &pNullSRV);
	m_d3d11DeviceContext->OMSetRenderTargets(1, &pNullRTV, nullptr); 

	//if current mask mode is only object then smooth it a little bit
	if (m_maskMode == MM_OnlyObject)
	{
		//horizontal
		m_d3d11DeviceContext->CSSetShader(m_pGaussHorizontalComputeShader, 0, 0);
		m_d3d11DeviceContext->CSSetShaderResources(0, 1, &m_pMaskSRV);;
		m_d3d11DeviceContext->CSSetUnorderedAccessViews(0, 1, &m_pBlurredUAV, nullptr);

		m_d3d11DeviceContext->Dispatch(1, SCREEN_HEIGHT, 1);

		m_d3d11DeviceContext->CSSetShaderResources(0, 1, &pNullSRV);
		m_d3d11DeviceContext->CSSetUnorderedAccessViews(0, 1, &pNullUAV, nullptr);

		//vertical
		m_d3d11DeviceContext->CSSetShader(m_pGaussVerticalComputeShader, 0, 0);
		m_d3d11DeviceContext->CSSetShaderResources(1, 1, &m_pBlurredSRV);
		m_d3d11DeviceContext->CSSetUnorderedAccessViews(1, 1, &m_pMaskUAV, nullptr);

		m_d3d11DeviceContext->Dispatch(SCREEN_WIDTH, 1, 1);

		m_d3d11DeviceContext->CSSetShaderResources(1, 1, &pNullSRV);
		m_d3d11DeviceContext->CSSetUnorderedAccessViews(1, 1, &pNullUAV, nullptr);
	}
	//*******************************************************************//



	////////////////////////////////////////////
	//Apply blur
	m_d3d11DeviceContext->CSSetShader(m_pGaussComputeShader, 0, 0);
	m_d3d11DeviceContext->CSSetShaderResources(0, 1, &m_pSceneSRV);
	m_d3d11DeviceContext->CSSetShaderResources(1, 1, &m_pMaskSRV);
	m_d3d11DeviceContext->CSSetUnorderedAccessViews(0, 1, &m_pBlurredUAV, nullptr);

	m_d3d11DeviceContext->Dispatch((UINT)ceil(SCREEN_WIDTH / 32.0f), (UINT)ceil(SCREEN_HEIGHT / 32.0f), 1);
	
	m_d3d11DeviceContext->CSSetShaderResources(0, 1, &pNullSRV);
	m_d3d11DeviceContext->CSSetShaderResources(1, 1, &pNullSRV);
	m_d3d11DeviceContext->CSSetUnorderedAccessViews(0, 1, &pNullUAV, nullptr);

	//*******************************************************************//

	/////////////////////////////////////////////
	//Render texture to the screen
	m_d3d11DeviceContext->IASetVertexBuffers(0, 1, &m_pRectVertexBuffer, &stride, &offset);
	m_d3d11DeviceContext->IASetInputLayout(m_pTextureLayout);
	m_d3d11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_d3d11DeviceContext->VSSetShader(m_pFinalPassVertexShader, 0, 0);
	m_d3d11DeviceContext->VSSetConstantBuffers(0, 1, &m_pcbMatrixTex);
	m_d3d11DeviceContext->RSSetViewports(1, &m_viewport);
	m_d3d11DeviceContext->PSSetShader(m_pFinalPassPixelShader, 0, 0);
	//m_d3d11DeviceContext->PSSetShaderResources(0, 1, &m_pSceneSRV);
	m_d3d11DeviceContext->PSSetShaderResources(0, 1, &m_pBlurredSRV);
	m_d3d11DeviceContext->OMSetBlendState(m_pBlendOff, NULL, 0xffffffff);
	m_d3d11DeviceContext->OMSetRenderTargets(1, &m_pbbRenderTargetView, nullptr);
	m_d3d11DeviceContext->OMSetDepthStencilState(m_pDepthDisable, 1);
	m_d3d11DeviceContext->Draw(6, 0);

	//unbind resources
	m_d3d11DeviceContext->PSSetShaderResources(0, 1, &pNullSRV);
	m_d3d11DeviceContext->OMSetRenderTargets(1, &pNullRTV, NULL);

	/////////////////////////////////////////////
	//Render text
	m_d3d11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_d3d11DeviceContext->VSSetShader(m_pFontVertexShader, 0, 0);
	m_d3d11DeviceContext->PSSetShader(m_pFontPixelShader, 0, 0);
	m_d3d11DeviceContext->PSSetSamplers(0, 1, &m_pLinearSampler);
	m_d3d11DeviceContext->OMSetBlendState(m_pAdditiveBlendOn, NULL, 0xffffffff);
	m_d3d11DeviceContext->OMSetRenderTargets(1, &m_pbbRenderTargetView, nullptr);

	for (Texts::iterator it = m_texts.begin(); it != m_texts.end(); it++)
	{
		Text* pText = (*it);

		//if disabled then skip
		if (!pText->IsEnable()) continue;

		//otherwise render
		pText->VPreRender(this, elapsedTime, totalTime);
		pText->VRender(this, elapsedTime, totalTime);
		pText->VPostRender(this, elapsedTime, totalTime);
	}

	m_d3d11DeviceContext->OMSetRenderTargets(1, &pNullRTV, NULL);

	//flip back buffer
	m_SwapChain->Present(0, 0);

	//*******************************************************************//

	////////////////////////////////////////////////
	//Clear all render targets
	float color[4] = { 0, 0, 0, 1 };
	m_d3d11DeviceContext->ClearRenderTargetView(m_pbbRenderTargetView, color);
	m_d3d11DeviceContext->ClearRenderTargetView(m_pMaskRTV, color);
	m_d3d11DeviceContext->ClearRenderTargetView(m_pSceneRTV, color);

	//*******************************************************************//
}

void BlurApp::VKeyPressed(const Key key)
{
	switch(key)
	{
		//'A'
	case 0x41:
		m_lightingMode = static_cast<LightingMode>(m_lightingMode - 1);
		if (m_lightingMode < 0)
			m_lightingMode = static_cast<LightingMode>(LM_NumLightingModes - 1);
		break;

		//'D'
	case 0x44:
		m_lightingMode = static_cast<LightingMode>(m_lightingMode + 1);
		m_lightingMode = static_cast<LightingMode>(static_cast<int>(m_lightingMode) % (LM_NumLightingModes));
		break;

		//'Q'
	case 0x51:
		m_maskMode = static_cast<MaskMode>(m_maskMode + 1);
		m_maskMode = static_cast<MaskMode>(static_cast<int>(m_maskMode) % (MM_NumMaskModes));
		break;
	};
}