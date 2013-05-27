#pragma once

#include "App.h"
#include "../Scene/Mesh.h"
#include "../gui/Font.h"
#include "../gui/Text.h"

class _declspec(align(16)) BlurApp : public App
{
protected:
	typedef std::vector<Mesh*> Meshes;
	Meshes	m_meshes;

	/////////////////////////////////////
	//Input layouts
	/////////////////////////////////////
	ID3D11InputLayout*	m_pGeometryLayout;
	ID3D11InputLayout*	m_pTextureLayout;

	/////////////////////////////////////
	//Blend States
	/////////////////////////////////////
	ID3D11BlendState*	m_pBlendOff;
	ID3D11BlendState*	m_pAdditiveBlendOn;

	/////////////////////////////////////
	//Shaders
	/////////////////////////////////////
	ID3D11PixelShader*		m_pMaskPixelShader;
	ID3D11PixelShader*		m_pMaskModifiedPixelShader;

	ID3D11VertexShader*		m_pLightingVertexShader;
	ID3D11PixelShader*		m_pLightingPixelShader;


	ID3D11ComputeShader*	m_pBlurComputeShader;

	ID3D11VertexShader*		m_pFinalPassVertexShader;
	ID3D11PixelShader*		m_pFinalPassPixelShader;

	ID3D11PixelShader*		m_pBackgroundPixelShader;

	ID3D11ComputeShader*	m_pGaussHorizontalComputeShader;
	ID3D11ComputeShader*	m_pGaussVerticalComputeShader;
	ID3D11ComputeShader*	m_pGaussComputeShader;

	ID3D11VertexShader*		m_pFontVertexShader;
	ID3D11PixelShader*		m_pFontPixelShader;

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
	ID3D11SamplerState*	m_pLinearSampler;
	ID3D11SamplerState*	m_pTiledSampler;

	///////////////////////////////////////
	//Depth Buffer
	///////////////////////////////////////
	ID3D11Texture2D*			m_pDepthStencilTexture;
	ID3D11DepthStencilState*	m_pDepthEnable;
	ID3D11DepthStencilState*	m_pDepthDisable;
	ID3D11DepthStencilView*		m_pDepthStencilView;
	ID3D11ShaderResourceView*	m_pDepthStencilSRV;

	/////////////////////////////////////
	//constant buffers
	/////////////////////////////////////
	ID3D11Buffer*	m_pcbScreen;
	struct ScreenData
	{
		float		screen_width;
		float		screen_height;
		XMFLOAT2	pad;
	};

	ID3D11Buffer*	m_pcbLighting;
	struct LightingData
	{
		/*** Lambert ***/
		XMVECTOR	lambertLightColor;

		/*** Lambert Wrap Around ***/
		XMVECTOR	lambertWALightColorAndFactor; //store factor as last vector component

		/*** Phong ***/
		XMVECTOR	phongLightColorAndSpecPower; //store spec power as last vector component

		/*** Blinn ***/
		XMVECTOR	blinnLightColorAndSpecPower; //store spec power as last vector component

		/*** Toon ***/
		//XMVECTOR	toonLightColor;

		/*** Isotropic Ward ***/
		XMVECTOR	isotropicWardLightColorAndRoughness; //store roughness factor as last vector component
	};
	//LightingData	m_lightingData;

	ID3D11ShaderResourceView*	m_pBackgroundSRV;

	//////////////////////////////////////
	//Text rendering
	//////////////////////////////////////
	Font*	m_pFont;

	typedef std::vector<Text*> Texts;
	Texts	m_texts;

	/*** For quick access only ***/
	//NOTE: DO NOT clean them in destructor
	//(they will be automatically deleted when cleaning text map)
	/////////////////////////////////////////////////////////////
	Text*	m_pLambertText;
	Text*	m_pLambertWrapAroundText;
	Text*	m_pPhongText;
	Text*	m_pBlinnText;
	Text*	m_pIsotropicWardText;

	//////////////////////////////////////
	//Texture rendering resources
	//////////////////////////////////////
	//XMMATRIX	m_orthoProjection;

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
	void InitializeBlendStates();
	void InitializeText();

	D3D11_VIEWPORT m_viewport;

public:

	//////////////////////////////
	//Accessors
	XMMATRIX GetViewMatrix() const { return m_matrixView; }
	XMMATRIX GetProjMatrix() const { return m_matrixProj; } 

protected:

	virtual void VKeyPressed(const Key key);

	enum Shape
	{
		box_shape,
		sphere_shape,
		cylinder_shape,
	};
	Shape m_currentShape;

	enum LightingMode
	{
		LM_Lambert,
		LM_LambertWrapAround,
		LM_Phong,
		LM_Blinn,
		//LM_Toon,
		LM_IsotropicWard,
		LM_NumLightingModes,
	};
	LightingMode m_mode;

	enum MaskMode
	{
		MM_Everything,
		MM_OnlyObject,
		MM_NumMaskModes
	};
	MaskMode m_maskMode;

	typedef std::map<LightingMode, ID3D11ClassInstance*>	LightingModeMap;
	LightingModeMap	m_lightingClassInstances;

public:
	BlurApp();
	~BlurApp();
	virtual bool VInitSimulation();
	void VUpdate(float elapsedTime, float totalTime);
	void VRender(float elapsedTime, float totalTime);
};