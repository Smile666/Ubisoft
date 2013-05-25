#pragma once

#include "App.h"
#include "../Scene/Mesh.h"

class _declspec(align(16)) BlurApp : public App
{
protected:
	typedef std::vector<Mesh*> Meshes;
	Meshes m_meshes;

	/////////////////////////////////////
	//Input layouts
	/////////////////////////////////////
	ID3D11InputLayout*	m_pGeometryLayout;
	ID3D11InputLayout*	m_pTextureLayout;

	/////////////////////////////////////
	//Shaders
	/////////////////////////////////////
	//ID3D11ComputeShader*	m_pMaskGenerationComputeShader;
	ID3D11PixelShader*	m_pMaskPixelShader;
	ID3D11VertexShader*	m_pLightningVertexShader;
	ID3D11PixelShader*	m_pLightningPixelShader;
	ID3D11ComputeShader*	m_pBlurComputeShader;
	ID3D11VertexShader*	m_pFinalPassVertexShader;
	ID3D11PixelShader*	m_pFinalPassPixelShader;
	ID3D11PixelShader*	m_pBackgroundPixelShader;

	ID3D11ComputeShader*	m_pGaussHorizontalComputeShader;
	ID3D11ComputeShader*	m_pGaussVerticalComputeShader;

	/////////////////////////////////////
	//Texture resources and views
	/////////////////////////////////////
	ID3D11Texture2D*	m_pMaskTexture;
	ID3D11RenderTargetView*	m_pMaskRTV;
	ID3D11ShaderResourceView* m_pMaskSRV;

	ID3D11Texture2D*	m_pSceneTexture;
	ID3D11RenderTargetView*	m_pSceneRTV;
	ID3D11ShaderResourceView*	m_pSceneSRV;
	ID3D11UnorderedAccessView*	m_pSceneUAV;

	ID3D11Texture2D*	m_pBlurredTexture;
	ID3D11ShaderResourceView*	m_pBlurredSRV;
	ID3D11UnorderedAccessView*	m_pBlurredUAV;

	///////////////////////////////////////////
	//Sampler States
	///////////////////////////////////////////
	ID3D11SamplerState*	m_pAnisotropicSampler;
	ID3D11SamplerState*	m_pTiledSampler;

	///////////////////////////////////////
	//Depth Buffer
	///////////////////////////////////////
	ID3D11Texture2D*			m_pDepthStencilTexture;
	ID3D11DepthStencilState*	m_pDepthStencilTest;
	ID3D11DepthStencilView*		m_pDepthStencilView;
	ID3D11ShaderResourceView*	m_pDepthStencilSRV;

	/////////////////////////////////////
	//constant buffer
	/////////////////////////////////////
	ID3D11Buffer*	m_pcbScreen;
	struct ScreenData
	{
		float		screen_width;
		float		screen_height;
		XMFLOAT2	pad;
	};

	ID3D11ShaderResourceView*	m_pBackgroundSRV;

	//////////////////////////////////////
	//Texture rendering resources
	//////////////////////////////////////
	XMMATRIX	m_orthoProjection;
	ID3D11Buffer*	m_pcbMatrixTex;
	struct MatrixTexBuffer
	{
		XMMATRIX WorldViewProjection;
	};

	struct RectVertex
	{
		XMFLOAT3 pos;
		XMFLOAT2 texCoord;

		RectVertex(	const float px, const float py, const float pz,
					const float tu, const float tv)
					:	pos(px, py, pz), texCoord(tu, tv)
		{
		};
	};
	ID3D11Buffer*	m_pRectVertexBuffer;
	ID3D11Buffer*	m_pScreenRectVertexBuffer;

	void InitializeTextures();
	void InitializeDepthBuffer();
	void InitializeSamplerStates();

	D3D11_VIEWPORT m_viewport;

public:

	//////////////////////////////
	//Accessors
	XMMATRIX GetViewMatrix() const { return m_matrixView; }
	XMMATRIX GetProjMatrix() const { return m_matrixProj; } 

protected:

	enum Shape
	{
		box_shape,
		sphere_shape,
		cylinder_shape,
	};
	Shape m_currentShape;

public:
	BlurApp();
	~BlurApp();
	virtual bool VInitSimulation();
	void VUpdate(real elapsedTime, real totalTime);
	void VRender(real elapsedTime, real totalTime);
};