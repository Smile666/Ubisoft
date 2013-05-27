#pragma once

#include "../Application/App.h"
#include "../Application/Interfaces.h"

void CreateNormalTangentBinormal(XMFLOAT3 * positions, XMFLOAT2 * texCoords,
		XMFLOAT3 & normal, XMFLOAT3 & tangent, XMFLOAT3 & binormal);

class Mesh : public IDrawable
{
public:
	Mesh();
	~Mesh();

	virtual bool VInitialize(ID3D11Device* pd3d11Device);
	virtual void VUpdate(App* pApp, const float elapsedTime, const float totalTime);
	
	virtual void VPreRender	(App* pApp, const float elapsedTime, const float totalTime);
	virtual void VRender	(App* pApp, const float elapsedTime, const float totalTime);
	virtual void VPostRender(App* pApp, const float elapsedTime, const float totalTime);

protected:
	////////////////////////////////////////////////////
	//Separate all buffer components for effeciency
	//purposes: for example, when we turn off lightning
	//we don't need to send normal data to GPU
	//
	//[0] - positions buffer
	//[1] - texture coordinates buffer
	//[2] - normal data buffer
	//
	////////////////////////////////////////////////////
	ID3D11Buffer** m_ppBuffers;

	UINT* m_pStride;
	UINT* m_pOffset;

	////////////////////////////////////////////////////
	//Shader Resource Views
	//
	////////////////////////////////////////////////////
	ID3D11ShaderResourceView*	m_pDiffuseSRV;
	ID3D11ShaderResourceView*	m_pNormalSRV;
	ID3D11ShaderResourceView*	m_pSpecularSRV;
	ID3D11ShaderResourceView*	m_pHeightSRV;

	////////////////////////////////////////////////////
	//Other rendering data
	////////////////////////////////////////////////////
	int	m_iNumVertices;

protected:

	// ============================================
	//		Define structures for vertex data
	// ============================================
	struct PositionData
	{
		XMFLOAT3 m_pos;

		//constructor
		PositionData(float x, float y, float z)
			: m_pos(x, y, z)
		{
		}
	};

	struct TexCoordData
	{
		XMFLOAT2 m_texCoord;

		//constructor
		TexCoordData(float tu, float tv)
			: m_texCoord(tu, tv)
		{
		}
	};

	struct NormalData
	{
		XMFLOAT3 m_normal;
		XMFLOAT3 m_tangent;
		XMFLOAT3 m_binormal;
	};

	// ============================================
	//				Geometry data
	// ============================================
	XMVECTOR	m_vPos;
	float		m_fPitch;
	float		m_fYaw;
	float		m_fRoll;

public:

	/////////////////////////////
	//Getters
	/////////////////////////////
	inline int GetNumVertices() const { return m_iNumVertices; }
};